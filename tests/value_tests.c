#include "minunit.h"
#include <forkix/value.h>
#include <assert.h>

VALUE intval = NULL;
VALUE strval = NULL;

char *test_integer_new()
{
  intval = Integer_new(123);

  mu_assert(intval->type == IntegerType, "failed assigning type");
  mu_assert(VAL2INT(intval) == 123, "failed assigning integer value");

  return NULL;
}

char *test_string_new()
{
  strval = String_new("abc");

  mu_assert(strval->type == StringType, "failed assigning type");
  mu_assert(strcmp(VAL2STR(strval), "abc") == 0, "failed assigning string value");

  return NULL;
}

char *test_destroy()
{
  Value_destroy(intval);
  mu_assert(!intval->type, "failed destroying integer value")

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_integer_new);
  mu_run_test(test_string_new);
  mu_run_test(test_destroy);

  return NULL;
}

RUN_TESTS(all_tests);
