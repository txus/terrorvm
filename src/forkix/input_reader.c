#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <forkix/dbg.h>
#include <forkix/bstrlib.h>
#include <forkix/input_reader.h>
#include <forkix/value.h>

/*
 * Return the filesize of `filename` or -1.
 */

static inline off_t
file_size(bstring filename) {
  struct stat s;
  int rc = stat(bdata(filename), &s);

  if (rc < 0) return -1;
  return s.st_size;
}

/*
 * Read the contents of `filename` or return NULL.
 */

static inline bstring
file_read(bstring filename) {
  off_t len = file_size(filename);
  check(len >= 0, "Invalid file length.");

  char *buf = malloc(len + 1);
  check_mem(buf);

  int fd = open(bdata(filename), O_RDONLY);
  check(fd >= 0, "Error opening file.");

  ssize_t size = read(fd, buf, len);
  check(size == len, "Read length is invalid");

  bstring retval = bfromcstr(buf);
  free(buf);
  return retval;

error:
  return NULL;
}

static inline void
parse_string(bstring buf, BytecodeFile *file)
{
  struct bstrList *lines = bsplit(buf, '\n');
  int i = 0;
  int cnt = 0;

  bstring *line = lines->entry;

  while(1) {
    // eof
    if (bstrcmp(bmidstr(*line, 0, 1), bfromcstr("_")) != 0) break;

    // Get method name
    int num_literals, num_instructions;
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
          DArray_push(literals, Integer_new(atoi(bdata(*line))));
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

    Function *fn = Function_new(instructions, literals);
    Hashmap_set(file->functions, method, fn);

    if(cnt >= lines->qty) break; // EOF
  }

  bstrListDestroy(lines);
}

BytecodeFile *BytecodeFile_new(bstring filename)
{
  BytecodeFile *file = calloc(1, sizeof(BytecodeFile));
  check_mem(file);

  file->filename = filename;
  file->functions = Hashmap_create(NULL, NULL);

  bstring buf = file_read(filename);
  check(buf, "Cannot read file %s", bdata(filename));
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
  if(file->functions) Hashmap_destroy(file->functions);

  free(file);
}
