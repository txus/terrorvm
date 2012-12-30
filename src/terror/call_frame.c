#include <stdlib.h>
#include <terror/value.h>
#include <terror/call_frame.h>

CallFrame*
CallFrame_new(VALUE self, Function *fn, int *ret)
{
  CallFrame* cf = calloc(1, sizeof(CallFrame));
  cf->self = self;
  cf->fn = fn;
  cf->ret = ret;
  cf->locals = DArray_create(sizeof(VALUE), 10);
  cf->parent = NULL;
  return cf;
}

VALUE
CallFrame_getlocal(CallFrame *frame, int idx)
{
  VALUE local = (VALUE)DArray_at(frame->locals, idx);
  check(local, "Couldn't retrieve local at index %i.", idx);
  return local;
error:
  return NULL;
}

void
CallFrame_setlocal(CallFrame *frame, int idx, VALUE value)
{
  DArray_set(frame->locals, idx, value);
}

VALUE
CallFrame_getlocaldepth(CallFrame *frame, int depth, int idx)
{
  while (depth--) {
    frame = frame->parent;
    check(frame, "Tried to get local at too much depth: %i.", depth);
  }
  return CallFrame_getlocal(frame, idx);
error:
  return NULL;
}

void
CallFrame_setlocaldepth(CallFrame *frame, int depth, int idx, VALUE value)
{
  while (depth--) {
    frame = frame->parent;
    check(frame, "Tried to set local at too much depth: %i.", depth);
  }
  return CallFrame_setlocal(frame, idx, value);
error:
  debug("Error.");
}
