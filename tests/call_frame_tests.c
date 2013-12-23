#include "minunit.h"
#include <terror/value.h>
#include <terror/call_frame.h>
#include <terror/state.h>
#include <assert.h>

STATE = NULL;

static inline CallFrame*
fixture()
{
  Function *fn = Function_new(NULL, 1, NULL, NULL);
  return CallFrame_new(NULL, fn, NULL);
}

#define PUSH(F, L) DArray_push((F)->locals, (L))

char *test_getlocal()
{
  VALUE one = Number_new(state, 1);
  VALUE two = Number_new(state, 2);

  CallFrame *frame = fixture();

  PUSH(frame, one);
  PUSH(frame, two);

  mu_assert(CallFrame_getlocal(frame, 1) == two, "Getlocal failed.");

  CallFrame_destroy(frame);
  return NULL;
}

char *test_setlocal()
{
  VALUE one = Number_new(state, 1);
  VALUE two = Number_new(state, 2);

  CallFrame *frame = fixture();

  PUSH(frame, one);

  CallFrame_setlocal(frame, 1, two);

  mu_assert(CallFrame_getlocal(frame, 1) == two, "Setlocal failed.");

  CallFrame_destroy(frame);
  return NULL;
}

char *test_getlocaldepth()
{
  VALUE one = Number_new(state, 1);
  VALUE two = Number_new(state, 2);

  CallFrame *parent = fixture();
  PUSH(parent, one);
  PUSH(parent, two);

  CallFrame *frame = fixture();
  frame->parent = parent;

  mu_assert(CallFrame_getlocaldepth(frame, 1, 0) == one, "Getlocaldepth failed");

  CallFrame_destroy(frame);
  CallFrame_destroy(parent);
  return NULL;
}

char *test_setlocaldepth()
{
  VALUE one = Number_new(state, 1);
  VALUE two = Number_new(state, 2);

  CallFrame *parent = fixture();
  PUSH(parent, one);

  CallFrame *frame = fixture();
  frame->parent = parent;

  CallFrame_setlocaldepth(frame, 1, 1, two);

  mu_assert(CallFrame_getlocal(parent, 1) == two, "Setlocaldepth failed");

  CallFrame_destroy(frame);
  CallFrame_destroy(parent);
  return NULL;
}

char *all_tests() {
  mu_suite_start();

  state = State_new();

  mu_run_test(test_getlocal);
  mu_run_test(test_setlocal);
  mu_run_test(test_getlocaldepth);
  mu_run_test(test_setlocaldepth);

  State_destroy(state);
  return NULL;
}

RUN_TESTS(all_tests);
