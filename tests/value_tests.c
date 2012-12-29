#include "minunit.h"
#include <forkix/value.h>
#include <assert.h>

char *test_integer_new()
{
  VALUE intval = Integer_new(123);

  mu_assert(intval->type == IntegerType, "failed assigning type");
  mu_assert(VAL2INT(intval) == 123, "failed assigning integer value");

  return NULL;
}

char *test_string_new()
{
  VALUE strval = String_new("abc");

  mu_assert(strval->type == StringType, "failed assigning type");
  mu_assert(strcmp(VAL2STR(strval), "abc") == 0, "failed assigning string value");

  return NULL;
}

char *test_closure_new()
{
  Function *fn = Function_new(NULL, NULL);
  VALUE closure = Closure_new(fn);
  mu_assert(closure->type == ClosureType, "failed creating closure");
  mu_assert(VAL2FN(closure) == fn, "failed assigning function to closure");

  return NULL;
}

char *test_vector_new()
{
  DArray *array = DArray_create(sizeof(VALUE), 10);
  VALUE vector = Vector_new(array);
  mu_assert(vector->type == VectorType, "failed creating vector");
  mu_assert(VAL2ARY(vector) == array, "failed assigning array to vector");

  return NULL;
}


char *test_main_new()
{
  VALUE main = Main_new();
  mu_assert(main->type == ObjectType, "failed creating Main");

  return NULL;
}

char *test_destroy()
{
  VALUE obj = Integer_new(123);
  Value_destroy(obj);
  mu_assert(!obj->type, "failed destroying object");

  return NULL;
}

char *test_get()
{
  VALUE obj = Integer_new(123);
  VALUE closure = Value_get(obj, "+");
  mu_assert(closure->type == ClosureType, "failed getting closure");

  return NULL;
}

char *test_get_undefined()
{
  VALUE obj = Integer_new(123);
  VALUE closure = Value_get(obj, "foo");
  mu_assert(closure == NULL, "expected NULL, got something");

  return NULL;
}

char *test_set()
{
  VALUE obj = Integer_new(123);
  VALUE integer = Integer_new(99);
  Value_set(obj, "foo", integer);
  mu_assert(Value_get(obj, "foo") == integer, "failed assigning foo");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_integer_new);
  mu_run_test(test_string_new);
  mu_run_test(test_closure_new);
  mu_run_test(test_vector_new);
  mu_run_test(test_main_new);
  mu_run_test(test_destroy);

  mu_run_test(test_get);
  mu_run_test(test_get_undefined);
  mu_run_test(test_set);

  return NULL;
}

RUN_TESTS(all_tests);
