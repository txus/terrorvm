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

/*
 * Vector primitives
 */

VALUE
Primitive_Vector_at(void *a, void *b, void *_)
{
  VALUE vector = (VALUE)a;
  VALUE index  = (VALUE)b;

  CHECK_TYPE(vector, VectorType);
  CHECK_TYPE(index, IntegerType);

  VALUE result = (VALUE)DArray_at(VAL2ARY(vector), VAL2INT(index));

  if(!result) result = NilObject;

  return result;
}

