#ifndef _fx_input_reader_h_
#define _fx_input_reader_h_

#include <forkix/bstrlib.h>
#include <forkix/function.h>
#include <forkix/darray.h>
#include <forkix/hashmap.h>

typedef struct {
  bstring filename;
  int *code;
  DArray *function_names;
  Hashmap *functions;
} BytecodeFile;

BytecodeFile *BytecodeFile_new(bstring filename);
void BytecodeFile_destroy(BytecodeFile *file);

#endif
