#include "minunit.h"
#include <forkix/runtime.h>
#include <assert.h>

char *test_init()
{
  Runtime_init();

  mu_assert(TrueObject->type == TrueType, "TrueObject didn't initialize");
  mu_assert(FalseObject->type == FalseType, "FalseObject didn't initialize");
  mu_assert(NilObject->type == NilType, "NilObject didn't initialize");

  return NULL;
}

char *test_destroy()
{
  Runtime_destroy();

  mu_assert(TrueObject == NULL, "TrueObject wasn't destroyed with the runtime");
  mu_assert(FalseObject == NULL, "FalseObject wasn't destroyed with the runtime");
  mu_assert(NilObject == NULL, "NilObject wasn't destroyed with the runtime");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_init);
  mu_run_test(test_destroy);

  return NULL;
}

RUN_TESTS(all_tests);
