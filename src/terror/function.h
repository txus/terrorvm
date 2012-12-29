#ifndef _tvm_function_h_
#define _tvm_function_h_

#include <terror/darray.h>
#include <terror/stack.h>

struct state_s;
struct val_s;
typedef struct val_s* (*native_fn)(struct state_s*, void*, void*, void*);

struct function_s {
  int *code;
  DArray *literals;
  native_fn c_fn;
};
typedef struct function_s Function;

Function* Function_new(int*, DArray*);
Function* Function_native_new(native_fn);
struct val_s* Function_native_call(struct state_s*, Function*, struct val_s*, DArray*);
int* Function_call(struct state_s*, Function*, struct val_s*, DArray*);

#endif

