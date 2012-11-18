#include <stdlib.h>
#include <forkix/call_frame.h>

CallFrame*
CallFrame_new(int *ip, int *ret)
{
  CallFrame* cf = calloc(1, sizeof(CallFrame));
  cf->ip = ip;
  cf->ret = ret;
  cf->locals = DArray_create(sizeof(VALUE), 10);
  return cf;
}
