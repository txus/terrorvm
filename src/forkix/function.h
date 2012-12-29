#ifndef _fx_function_h_
#define _fx_function_h_

#include <forkix/darray.h>
#include <forkix/stack.h>
#include <forkix/state.h>

struct val_s;
typedef struct val_s* (*native_fn)(STATE, void*, void*, void*);

struct function_s {
  int *code;
  DArray *literals;
  native_fn c_fn;
};
typedef struct function_s Function;

Function* Function_new(int*, DArray*);
Function* Function_native_new(native_fn);
struct val_s* Function_native_call(STATE, Function *fn, struct val_s*, DArray *locals);
int* Function_call(STATE, Function *fn, struct val_s* receiver, DArray *locals, int *ret);

#endif
