#include "minunit.h"
#include <terror/value.h>
#include <terror/vector.h>
#include <terror/state.h>
#include <assert.h>

STATE = NULL;

char *test_each()
{
  DArray *ary = DArray_create(sizeof(VALUE), 10);
  VALUE one   = Number_new(state, 1);
  VALUE two   = Number_new(state, 2);
  VALUE three = Number_new(state, 3);
  DArray_push(ary, one);
  DArray_push(ary, two);
  DArray_push(ary, three);

  VALUE vector = Vector_new(state, ary);

  __block int counter = 0;

  Vector_each(vector, ^ void (VALUE val) {
    counter = counter + VAL2NUM(val);
  });

  mu_assert(counter == 6, "Vector_each failed.");
  return NULL;
}


char *all_tests() {
  mu_suite_start();

  state = State_new();

  mu_run_test(test_each);

  return NULL;
}

RUN_TESTS(all_tests);
