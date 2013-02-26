#ifndef _tvm_input_reader_h_
#define _tvm_input_reader_h_

#include <terror/bstrlib.h>
#include <terror/function.h>
#include <terror/darray.h>
#include <terror/hashmap.h>

struct state_s;

typedef struct {
  bstring filename;
  bstring compiled_filename;
  int *code;
  DArray *function_names;
  Hashmap *functions;
} BytecodeFile;

BytecodeFile *BytecodeFile_new(struct state_s*, bstring filename);
void BytecodeFile_destroy(BytecodeFile *file);
int Hashmap_Function_destroy(HashmapNode *node);

#endif
