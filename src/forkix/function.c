#include <forkix/function.h>

Function*
Function_new(int *code, DArray *literals)
{
  Function *fn = calloc(1, sizeof(Function));
  fn->code = code;
  fn->literals = literals;
  return fn;
}
