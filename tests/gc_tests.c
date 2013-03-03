#include "minunit.h"
#include <terror/runtime.h>
#include <terror/input_reader.h>
#include <terror/bootstrap.h>
#include <terror/state.h>
#include <assert.h>

// Extern global objects declared in runtime.h
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

#define SETUP() \
  Hashmap *fns = Hashmap_create(NULL, NULL);            \
  STATE = State_new();                                  \
  Hashmap_destroy(state->functions);                    \
  state->functions = fns;                               \
  Runtime_init(state);                                  \
  VALUE lobby = Lobby_new(state);                       \
  state->lobby = lobby;                                 \
  State_bootstrap(state);                               \

#define TEARDOWN()                                      \
  State_destroy(state);                                 \
  return NULL;                                          \

char *test_empty()
{
  SETUP();
  TEARDOWN();
}


char *all_tests() {
  mu_suite_start();

  mu_run_test(test_empty);

  return NULL;
}

RUN_TESTS(all_tests);
