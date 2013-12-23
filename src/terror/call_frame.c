#include <stdlib.h>
#include <terror/value.h>
#include <terror/call_frame.h>
#include <terror/runtime.h>

VALUE NilObject;

CallFrame*
CallFrame_new(VALUE self, Function *fn, int *ret)
{
  CallFrame* cf = calloc(1, sizeof(CallFrame));
  cf->self = self;
  cf->fn = fn;
  cf->name = NULL;
  cf->ret = ret;
  int locals_count = fn->locals_count > 0 ? fn->locals_count : 1;
  cf->locals = DArray_create(sizeof(VALUE), locals_count);
  debug("Locals count is %i, array %p max is %i", locals_count, cf->locals, cf->locals->max);
  cf->parent = NULL;
  cf->refcount = 0;
  return cf;
}

void
CallFrame_destroy(CallFrame *frame)
{
  if(frame->refcount <= 0) {
    if(frame->parent) {
      CallFrame_release(frame->parent);
    }
    DArray_destroy(frame->locals);
    free(frame);
  }
}

VALUE
CallFrame_getlocal(CallFrame *frame, int idx)
{
  VALUE local = (VALUE)DArray_at(frame->locals, idx);
  if (!local) local = NilObject;
  return local;
}

void
CallFrame_setlocal(CallFrame *frame, int idx, VALUE value)
{
  debug("SETTING LOCAL %i on fn %p with local count %i", idx, frame->fn, frame->fn->locals_count);
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

void
CallFrame_print(CallFrame *frame)
{
  printf("%s:%i", frame->fn->filename, frame->fn->line);
  if(frame->name) {
    printf(" (%s)", frame->name);
  }
  printf("\n");
}

void
CallFrame_print_backtrace(CallFrame *frame)
{
  CallFrame *p = frame;
  while(p) {
    CallFrame_print(p);
    p = p->parent;
  }
  printf("\n");
}
