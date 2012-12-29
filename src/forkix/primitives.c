#include <forkix/primitives.h>
#include <forkix/runtime.h>
#include <assert.h>

/*
 * Generic primitives
 */

VALUE
Primitive_print(STATE, void *_, void *b, void *__)
{
  Value_print((VALUE)b);
  return NilObject;
}

VALUE
Primitive_puts(STATE, void *a, void *b, void *c)
{
  Primitive_print(state, a, b, c);
  printf("\n");
  return NilObject;
}


/*
 * Integer primitives
 */

VALUE
Primitive_Integer_add(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) + VAL2INT(right));
}

VALUE
Primitive_Integer_sub(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) - VAL2INT(right));
}

VALUE
Primitive_Integer_mul(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) * VAL2INT(right));
}

VALUE
Primitive_Integer_div(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  assert(VAL2INT(right) != 0 && "Cannot divide by zero");

  return Integer_new(VAL2INT(left) / VAL2INT(right));
}

/*
 * Closure primitives
 */

VALUE
Primitive_Closure_apply(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) + VAL2INT(right));
}

/*
 * Vector primitives
 */

VALUE
Primitive_Vector_at(STATE, void *a, void *b, void *_)
{
  VALUE vector = (VALUE)a;
  VALUE index  = (VALUE)b;

  CHECK_TYPE(vector, VectorType);
  CHECK_TYPE(index, IntegerType);

  VALUE result = (VALUE)DArray_at(VAL2ARY(vector), VAL2INT(index));

  if(!result) result = NilObject;

  return result;
}

VALUE
Primitive_Vector_to_map(STATE, void *a, void *_, void *__)
{
  VALUE vector = (VALUE)a;
  CHECK_TYPE(vector, VectorType);

  return Map_new(VAL2ARY(vector));
}

/*
 * Map primitives
 */

VALUE
Primitive_Map_get(STATE, void *a, void *b, void *_)
{
  VALUE map = (VALUE)a;
  VALUE key = (VALUE)b;

  CHECK_TYPE(map, MapType);
  CHECK_TYPE(key, StringType);

  VALUE result = Value_get(map, VAL2STR(key));
  if(!result) result = NilObject;

  return result;
}

VALUE
Primitive_Map_set(STATE, void *a, void *b, void *c)
{
  VALUE map   = (VALUE)a;
  VALUE key   = (VALUE)b;
  VALUE value = (VALUE)c;

  CHECK_TYPE(map, MapType);
  CHECK_TYPE(key, StringType);
  CHECK_PRESENCE(value);

  Value_set(map, VAL2STR(key), value);

  return value;
}

