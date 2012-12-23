#include <forkix/primitives.h>
#include <forkix/runtime.h>
#include <assert.h>

/*
 * Generic primitives
 */

VALUE
Primitive_print(void *_, void *b, void *__)
{
  Value_print((VALUE)b);
  return NilObject;
}

VALUE
Primitive_puts(void *a, void *b, void *c)
{
  Primitive_print(a, b, c);
  printf("\n");
  return NilObject;
}


/*
 * Integer primitives
 */

VALUE
Primitive_Integer_add(void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) + VAL2INT(right));
}

VALUE
Primitive_Integer_sub(void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) - VAL2INT(right));
}

VALUE
Primitive_Integer_mul(void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) * VAL2INT(right));
}

VALUE
Primitive_Integer_div(void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  assert(VAL2INT(right) != 0 && "Cannot divide by zero");

  return Integer_new(VAL2INT(left) / VAL2INT(right));
}

