#include <forkix/function.h>
#include <forkix/value.h>

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
Function_native_call(Function *fn, VALUE receiver, DArray *locals)
{
  VALUE result = NULL;
  if (DArray_at(locals, 1)) {
    result = fn->c_fn(receiver, DArray_at(locals, 0), DArray_at(locals, 1));
  } else if (DArray_at(locals, 0)) {
    result = fn->c_fn(receiver, DArray_at(locals, 0), NULL);
  } else {
    result =fn->c_fn(receiver, NULL, NULL);
  }

  return result;
}
