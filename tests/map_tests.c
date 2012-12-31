#include "minunit.h"
#include <terror/value.h>
#include <terror/map.h>
#include <terror/bstrlib.h>
#include <assert.h>

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

  Map_each(map, ^ void (bstring k, VALUE v) {
    bconcat(keys, k);
    counter = counter + VAL2NUM(v);
  });

  mu_assert(counter == 3, "Map_each failed.");
  mu_assert(bstrcmp(keys, bfromcstr("foobar")) == 0, "Map_each failed.");
  return NULL;
}


char *all_tests() {
  mu_suite_start();

  mu_run_test(test_each);

  return NULL;
}

RUN_TESTS(all_tests);
