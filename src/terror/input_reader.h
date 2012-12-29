#ifndef _tvm_input_reader_h_
#define _tvm_input_reader_h_

#include <terror/bstrlib.h>
#include <terror/function.h>
#include <terror/darray.h>
#include <terror/hashmap.h>

typedef struct {
  bstring filename;
  int *code;
  DArray *function_names;
  Hashmap *functions;
} BytecodeFile;

BytecodeFile *BytecodeFile_new(bstring filename);
void BytecodeFile_destroy(BytecodeFile *file);

#endif
