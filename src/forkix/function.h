#ifndef _fx_function_h_
#define _fx_function_h_

#include <forkix/darray.h>

struct val_s;
typedef struct val_s* (*native_fn)(void*, void*, void*);

struct function_s {
  int *code;
  DArray *literals;
  native_fn c_fn;
};
typedef struct function_s Function;

Function* Function_new(int*, DArray*);
Function* Function_native_new(native_fn);
struct val_s* Function_native_call(Function *fn, struct val_s*, DArray *locals);

#endif
