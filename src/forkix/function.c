#include <forkix/function.h>
#include <forkix/value.h>
#include <forkix/stack.h>
#include <forkix/call_frame.h>

Function*
Function_new(int *code, DArray *literals)
{
  Function *fn = calloc(1, sizeof(Function));
  fn->code = code;
  fn->literals = literals;
  fn->c_fn = NULL;
  return fn;
}

Function*
Function_native_new(native_fn c_fn)
{
  Function *fn = calloc(1, sizeof(Function));
  fn->code     = NULL;
  fn->literals = NULL;
  fn->c_fn     = c_fn;
  return fn;
}

VALUE
Function_native_call(STATE, Function *fn, VALUE receiver, DArray *locals)
{
  return fn->c_fn(
    state,
    receiver,
    DArray_at(locals, 0),
    DArray_at(locals, 1)
    );
}

int*
Function_call(
  STATE,          // the State
  Function *fn,   // the Function we're calling
  VALUE receiver, // the Receiver value
  DArray *locals, // the arguments to the function call
  int *ret)       // where to return
{
  // Native function dispatch
  if(fn->c_fn) {
    VALUE result = Function_native_call(state, fn, receiver, locals);
    Stack_push(STACK, result);
    return ret; // return where we were
  }

  // Normal dispatch
  CallFrame *new_frame = CallFrame_new(receiver, fn, ret);
  new_frame->locals = locals;
  Stack_push(FRAMES, new_frame);

  return new_frame->fn->code;
}
