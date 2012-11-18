#include "minunit.h"
#include <forkix/value.h>
#include <assert.h>

VALUE val = NULL;

char *test_integer_new()
{
  val = Integer_new(123);

  mu_assert(val->type == IntegerType, "failed assigning type");
  mu_assert(VAL2INT(val) == 123, "failed assigning integer value");

  return NULL;
}

char *test_destroy()
{
  Value_destroy(val);
  mu_assert(!val->type, "failed destroying value")

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_integer_new);
  mu_run_test(test_destroy);

  return NULL;
}

RUN_TESTS(all_tests);
