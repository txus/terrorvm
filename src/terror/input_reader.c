#include <terror/file_utils.h>
#include <terror/input_reader.h>
#include <terror/value.h>

static inline void
parse_string(bstring buf, BytecodeFile *file)
{
  struct bstrList *lines = bsplit(buf, '\n');
  int i = 0;
  int cnt = 0;

  bstring *line = lines->entry;

  // Get filename
  file->filename = bstrcpy(*line);
  line++; cnt++;

  while(1) {
    // eof
    if (bstrcmp(bmidstr(*line, 0, 1), bfromcstr("_")) != 0) break;

    // Get method name
    int num_literals, num_instructions, linenum;
    {
      struct bstrList *fn_params = bsplit(*line, '_');
      bstring *fnptr = fn_params->entry;
      fnptr++; // linenum
      linenum = atoi(bdata(*fnptr));
    }

    bstring method = bmidstr(*line, 1, (*line)->mlen);
    line++; cnt++;

    // Get counts
    sscanf(bdata(*line), ":%i:%i",
      &num_literals, &num_instructions);

    line++; cnt++;

    DArray *literals = DArray_create(sizeof(VALUE), 10);
    // Parse the actual literals
    if (num_literals > 0) {
      literals = DArray_create(sizeof(VALUE), num_literals);

      for(i=0; i < num_literals; i++) {
        if (bdata(*line)[0] == '"') {
          bstring strData = bmidstr(*line, 1, (*line)->mlen);
          // TODO: This causes a leak in the string literals because they are
          // never freed on destruction.
          DArray_push(literals, String_new(bdata(bstrcpy(strData))));
          bdestroy(strData);
        } else {
          DArray_push(literals, Number_new(atof(bdata(*line))));
        }
        line++; cnt++;
      }
    }

    // Parse the instructions

    int *instructions = calloc(num_instructions, sizeof(int));

    for(i=0; i < num_instructions; i++) {
      int num;
      sscanf(bdata(*line), "%i", &num);
      instructions[i] = num;

      line++; cnt++;
    }

    Function *fn = Function_new(bdata(file->filename), instructions, literals);
    fn->line = linenum;
    DArray_push(file->function_names, method);
    debug("Parsed %s...", bdata(method));
    Hashmap_set(file->functions, method, fn);

    if(cnt >= lines->qty) break; // EOF
  }

  bstrListDestroy(lines);
}

BytecodeFile *BytecodeFile_new(bstring compiled_filename)
{
  BytecodeFile *file = calloc(1, sizeof(BytecodeFile));
  check_mem(file);

  file->compiled_filename = compiled_filename;
  file->functions = Hashmap_create(NULL, NULL);
  file->function_names = DArray_create(sizeof(bstring), 10);

  bstring buf = readfile(compiled_filename);
  check(buf, "Cannot read file %s", bdata(compiled_filename));
  parse_string(buf, file);

  bdestroy(buf);

  return file;

error:
  return NULL;
}

void
BytecodeFile_destroy(BytecodeFile *file)
{
  if(file->filename) bdestroy(file->filename);
  if(file->compiled_filename) bdestroy(file->compiled_filename);
  if(file->functions) Hashmap_destroy(file->functions);

  free(file);
}
