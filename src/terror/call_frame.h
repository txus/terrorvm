#ifndef _tvm_call_frame_h_
#define _tvm_call_frame_h_

#include <terror/darray.h>
#include <terror/function.h>

struct val_s;

struct call_frame_s {
  struct val_s *self;
  char *name;
  DArray *locals;
  Function *fn;
  int *ret;
  int refcount;
  struct call_frame_s *parent;
};

typedef struct call_frame_s CallFrame;

CallFrame* CallFrame_new(struct val_s* self, Function *fn, int *ret);
void CallFrame_destroy(CallFrame *frame);

struct val_s* CallFrame_getlocal(CallFrame *frame, int idx);
void CallFrame_setlocal(CallFrame *frame, int idx, struct val_s* value);
struct val_s* CallFrame_getlocaldepth(CallFrame *frame, int depth, int idx);
void CallFrame_setlocaldepth(CallFrame *frame, int depth, int idx, struct val_s* value);
void CallFrame_print(CallFrame *frame);
void CallFrame_print_backtrace(CallFrame *frame);
#define CallFrame_is_captured(A) (A)->refcount > 0

#endif
