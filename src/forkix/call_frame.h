#ifndef _fx_call_frame_h_
#define _fx_call_frame_h_

#include <forkix/value.h>
#include <forkix/darray.h>
#include <forkix/function.h>

struct call_frame_s {
  VALUE self;
  DArray *locals;
  Function *fn;
  int *ret;
};

typedef struct call_frame_s CallFrame;

CallFrame* CallFrame_new(VALUE self, Function *fn, int *ret);

#endif
