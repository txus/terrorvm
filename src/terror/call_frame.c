#include <stdlib.h>
#include <terror/call_frame.h>

CallFrame*
CallFrame_new(VALUE self, Function *fn, int *ret)
{
  CallFrame* cf = calloc(1, sizeof(CallFrame));
  cf->self = self;
  cf->fn = fn;
  cf->ret = ret;
  cf->locals = DArray_create(sizeof(VALUE), 10);
  return cf;
}
