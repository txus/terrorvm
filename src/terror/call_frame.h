#ifndef _tvm_call_frame_h_
#define _tvm_call_frame_h_

#include <terror/value.h>
#include <terror/darray.h>
#include <terror/function.h>

struct call_frame_s {
  VALUE self;
  DArray *locals;
  Function *fn;
  int *ret;
};

typedef struct call_frame_s CallFrame;

CallFrame* CallFrame_new(VALUE self, Function *fn, int *ret);

#endif
