#include "minunit.h"
#include <terror/value.h>
#include <terror/bstrlib.h>
#include <assert.h>

char *test_integer_new()
{
  VALUE intval = Number_new(123);

  mu_assert(intval->type == NumberType, "failed assigning type");
  mu_assert(VAL2NUM(intval) == 123, "failed assigning integer value");

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
  Function *fn = Function_new("myfile", NULL, NULL);
  VALUE closure = Closure_new(fn, NULL);
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

char *test_map_new()
{
  DArray *array = DArray_create(sizeof(VALUE), 10);
  VALUE map = Map_new(array);
  mu_assert(map->type == MapType, "failed creating map");
  mu_assert(VAL2HASH(map), "failed assigning hash to map");

  return NULL;
}

char *test_lobby_new()
{
  VALUE lobby = Lobby_new();
  mu_assert(lobby->type == ObjectType, "failed creating Lobby");

  return NULL;
}

char *test_destroy()
{
  VALUE obj = Number_new(123);
  Value_destroy(obj);
  mu_assert(!obj->type, "failed destroying object");

  return NULL;
}

char *test_get()
{
  VALUE obj = Number_new(123);
  Value_set(obj, "foo", Number_new(99));
  VALUE number = Value_get(obj, "foo");
  mu_assert(number->type == NumberType, "failed getting member of integer");

  return NULL;
}

char *test_get_undefined()
{
  VALUE obj = Number_new(123);
  VALUE closure = Value_get(obj, "foo");
  mu_assert(closure == NULL, "expected NULL, got something");

  return NULL;
}

char *test_set()
{
  VALUE obj = Number_new(123);
  VALUE integer = Number_new(99);
  Value_set(obj, "foo", integer);
  mu_assert(Value_get(obj, "foo") == integer, "failed assigning foo");

  return NULL;
}

char *test_each()
{
  DArray *ary = DArray_create(sizeof(VALUE), 10);
  VALUE k1 = String_new("foo");
  VALUE v1 = Number_new(1);
  VALUE k2 = String_new("bar");
  VALUE v2 = Number_new(2);
  DArray_push(ary, k1);
  DArray_push(ary, v1);
  DArray_push(ary, k2);
  DArray_push(ary, v2);

  VALUE map = Map_new(ary);

  __block bstring keys = bfromcstr("");
  __block int counter = 0;

  Value_each(map, ^ void (VALUE k, VALUE v) {
    bconcat(keys, bfromcstr(VAL2STR(k)));
    counter = counter + VAL2NUM(v);
  });

  mu_assert(counter == 3, "Value_each failed.");
  mu_assert(bstrcmp(keys, bfromcstr("foobar")) == 0, "Value_each failed.");
  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_integer_new);
  mu_run_test(test_string_new);
  mu_run_test(test_closure_new);
  mu_run_test(test_vector_new);
  mu_run_test(test_map_new);
  mu_run_test(test_lobby_new);
  mu_run_test(test_destroy);

  mu_run_test(test_get);
  mu_run_test(test_get_undefined);
  mu_run_test(test_set);
  mu_run_test(test_each);

  return NULL;
}

RUN_TESTS(all_tests);
