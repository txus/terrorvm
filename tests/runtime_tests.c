#include "minunit.h"
#include <terror/runtime.h>
#include <terror/state.h>
#include <assert.h>

STATE = NULL;

char *test_init()
{
  Runtime_init(state);

  mu_assert(TrueObject->type == TrueType, "TrueObject didn't initialize");
  mu_assert(FalseObject->type == FalseType, "FalseObject didn't initialize");
  mu_assert(NilObject->type == NilType, "NilObject didn't initialize");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  state = State_new();

  mu_run_test(test_init);

  return NULL;
}

RUN_TESTS(all_tests);
