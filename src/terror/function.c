#include <terror/function.h>
#include <terror/value.h>
#include <terror/stack.h>
#include <terror/state.h>
#include <terror/call_frame.h>
#include <terror/vm.h>

Function*
Function_new(char *filename, int *code, DArray *literals)
{
  Function *fn = calloc(1, sizeof(Function));
  fn->filename = filename;
  fn->code = code;
  fn->line = 0;
  fn->literals = literals;
  fn->scope = NULL;
  fn->c_fn = NULL;
  return fn;
}

void
Function_destroy(Function *fn)
{
  if(fn) {
    if(fn->literals) DArray_destroy(fn->literals);
    free(fn);
  }
}

Function*
Function_native_new(native_fn c_fn)
{
  Function *fn = calloc(1, sizeof(Function));
  char *name = malloc(9 * sizeof(char));
  strcpy(name, "(native)");

  fn->filename = name;
  fn->code     = NULL;
  fn->literals = NULL;
  fn->scope    = NULL;
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
  char *name)     // optional name for the backtrace
{
  int *ret = state->ret;
  if(!receiver) receiver = CURR_FRAME->self;

  // Native function dispatch
  if(fn->c_fn) {
    VALUE result = Function_native_call(state, fn, receiver, locals);
    DArray_destroy(locals);
    Stack_push(STACK, result);
    return ret; // return where we were
  }

  // Normal dispatch
  CallFrame *new_frame = CallFrame_new(receiver, fn, ret);
  new_frame->locals = locals;
  new_frame->name = name;

  // If it is a closure, we nest the call frames.
  if(fn->scope) {
    new_frame->parent = fn->scope;
    new_frame->parent->refcount++;
  }

  Stack_push(FRAMES, new_frame);

  ret = new_frame->fn->code;
  ret--;

  return ret;
}

VALUE
Closure_invoke(STATE, VALUE closure, VALUE receiver, DArray *args, char *name)
{
  state->ret = NULL; // don't return anywhere
  Function_call(state, VAL2FN(closure), receiver, args, name);
  return VM_run(state);
}
