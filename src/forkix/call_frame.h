#ifndef _fx_call_frame_h_
#define _fx_call_frame_h_

#include <forkix/value.h>
#include <forkix/darray.h>

struct call_frame_s {
  DArray *locals;
  int *ip;
  int *ret;
};

typedef struct call_frame_s CallFrame;

CallFrame* CallFrame_new(int *ip, int *ret);

#endif
