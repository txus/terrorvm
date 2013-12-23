#ifndef _tvm_function_h_
#define _tvm_function_h_

#include <terror/darray.h>
#include <terror/stack.h>

struct state_s;
struct val_s;
struct call_frame_s;
typedef struct val_s* (*native_fn)(struct state_s*, void*, void*, void*);

struct function_s {
  char *filename;
  int line;
  int *code;
  DArray *literals;
  unsigned int locals_count;
  struct call_frame_s *scope;
  native_fn c_fn;
};
typedef struct function_s Function;

Function* Function_new(char*, unsigned int, int*, DArray*);
void Function_destroy(Function*);
Function* Function_native_new(struct state_s*, native_fn);
struct val_s* Function_native_call(struct state_s*, Function*, struct val_s*, DArray*);
int* Function_call(struct state_s*, Function*, struct val_s*, DArray*, char* name);

struct val_s* Closure_invoke(struct state_s*, struct val_s* closure, struct val_s* receiver, DArray *args, char *name);
#endif

