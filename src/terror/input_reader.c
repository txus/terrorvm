#include <terror/file_utils.h>
#include <terror/input_reader.h>
#include <terror/value.h>
#include <terror/state.h>

static inline bstring
resolve_path(STATE, bstring path)
{
  char *absolute_path = malloc(PATH_MAX);
	bstring h = executable_name(state->binary);
	struct bstrList *x = bsplit(h, '/');
	bdestroy(h);
	x->qty--;
	h = bjoin(x, bfromcstr("/"));
	x->qty++;
	bstrListDestroy(x);
	bstring slash = bfromcstr("/");
	bconcat(h, slash);
	bconcat(h, path);
	bdestroy(slash);

  realpath(bdata(h), absolute_path);
  return bfromcstr(absolute_path);
}

static inline void
parse_string(STATE, bstring buf, BytecodeFile *file)
{
  struct bstrList *lines = bsplit(buf, '\n');
  int i = 0;
  int cnt = 0;

  bstring *line = lines->entry;

  // Get filename
  file->filename = resolve_path(state, *line);

  line++; cnt++;

  bstring underscore = bfromcstr("_");

  while(1) {
    // eof
    bstring prefix = bmidstr(*line, 0, 1);
    if (bstrcmp(prefix, underscore) != 0) {
      bdestroy(prefix);
      break;
    }
    bdestroy(prefix);

    // Get method name
    int num_literals, num_instructions, linenum;
    {
      struct bstrList *fn_params = bsplit(*line, '_');
      bstring *fnptr = fn_params->entry;
      fnptr++; // linenum
      linenum = atoi(bdata(*fnptr));
      bstrListDestroy(fn_params);
    }

    bstring method = bmidstr(*line, 1, (*line)->mlen);
    line++; cnt++;

    // Get counts
    sscanf(bdata(*line), ":%i:%i",
      &num_literals, &num_instructions);

    line++; cnt++;

    // We have to create the function here for all the literals to be
    // automatically included in the rootset, otherwise they will be GC'd as we
    // create them. And we don't want that now do we.
    DArray *literals = DArray_create(sizeof(VALUE), 10);
    int *instructions = calloc(num_instructions, sizeof(int));
    Function *fn = Function_new(bdata(file->filename), instructions, literals);

    fn->line = linenum;
    DArray_push(file->function_names, method);
    Hashmap_set(file->functions, method, fn);

    // Parse the actual literals
    if (num_literals > 0) {
      for(i=0; i < num_literals; i++) {
        if (bdata(*line)[0] == '"') {
          bstring strData = bmidstr(*line, 1, (*line)->mlen);
          DArray_push(literals, String_new(state, bdata(strData)));
          bdestroy(strData);
        } else {
          DArray_push(literals, Number_new(state, atof(bdata(*line))));
        }
        line++; cnt++;
      }
    }

    // Parse the instructions
    for(i=0; i < num_instructions; i++) {
      int num;
      sscanf(bdata(*line), "%i", &num);
      instructions[i] = num;

      line++; cnt++;
    }

    if(cnt >= lines->qty) break; // EOF
  }

  bdestroy(underscore);
  bstrListDestroy(lines);
}

BytecodeFile *BytecodeFile_new(STATE, bstring compiled_filename)
{
  BytecodeFile *file = calloc(1, sizeof(BytecodeFile));
  check_mem(file);
  file->compiled_filename = compiled_filename;
  file->functions = Hashmap_create(NULL, NULL);
  file->function_names = DArray_create(sizeof(bstring), 10);

  DArray_push(state->files, file);

  bstring buf = readfile(compiled_filename);
  check(buf, "Cannot read file %s", bdata(compiled_filename));
  parse_string(state, buf, file);

  bdestroy(buf);

  return file;

error:
  debug("Aborted.");
  exit(EXIT_FAILURE);
  return NULL;
}

void
BytecodeFile_destroy(BytecodeFile *file)
{
  if(file->filename) bdestroy(file->filename);
  if(file->compiled_filename) bdestroy(file->compiled_filename);

  Hashmap_traverse(file->functions, Hashmap_Function_destroy);
  Hashmap_destroy(file->functions);

  for(int i=0; i < DArray_count(file->function_names); i++) {
    bdestroy((bstring)DArray_at(file->function_names, i));
  }
  DArray_destroy(file->function_names);

  free(file);
}

int
Hashmap_Function_destroy(HashmapNode *node)
{
  Function_destroy((Function*)node->data);
  return 0;
}

