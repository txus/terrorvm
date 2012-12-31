#include "minunit.h"
#include <terror/value.h>
#include <terror/call_frame.h>
#include <assert.h>

static inline CallFrame*
fixture()
{
  return CallFrame_new(NULL, NULL, NULL);
}

#define PUSH(F, L) DArray_push((F)->locals, (L))

char *test_getlocal()
{
  VALUE one = Number_new(1);
  VALUE two = Number_new(2);

  CallFrame *frame = fixture();

  PUSH(frame, one);
  PUSH(frame, two);

  mu_assert(CallFrame_getlocal(frame, 1) == two, "Getlocal failed.");

  return NULL;
}

char *test_setlocal()
{
  VALUE one = Number_new(1);
  VALUE two = Number_new(2);

  CallFrame *frame = fixture();

  PUSH(frame, one);

  CallFrame_setlocal(frame, 1, two);

  mu_assert(CallFrame_getlocal(frame, 1) == two, "Setlocal failed.");

  return NULL;
}

char *test_getlocaldepth()
{
  VALUE one = Number_new(1);
  VALUE two = Number_new(2);

  CallFrame *parent = fixture();
  PUSH(parent, one);
  PUSH(parent, two);

  CallFrame *frame = fixture();
  frame->parent = parent;

  mu_assert(CallFrame_getlocaldepth(frame, 1, 0) == one, "Getlocaldepth failed");

  return NULL;
}

char *test_setlocaldepth()
{
  VALUE one = Number_new(1);
  VALUE two = Number_new(2);

  CallFrame *parent = fixture();
  PUSH(parent, one);

  CallFrame *frame = fixture();
  frame->parent = parent;

  CallFrame_setlocaldepth(frame, 1, 1, two);

  mu_assert(CallFrame_getlocal(parent, 1) == two, "Setlocaldepth failed");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_getlocal);
  mu_run_test(test_setlocal);
  mu_run_test(test_getlocaldepth);
  mu_run_test(test_setlocaldepth);

  return NULL;
}

RUN_TESTS(all_tests);