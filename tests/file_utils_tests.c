#include "minunit.h"
#include <terror/file_utils.h>
#include <assert.h>

char *test_getlines()
{
  bstring str = bfromcstr("one\ntwo\nthree\nfour\nfive\n");
  struct bstrList *file = bsplit(str, '\n');

  DArray *lines = getlines(file, 2, 4);

  mu_assert(DArray_count(lines) == 3, "Wrong number of lines.");
  mu_assert(bstrcmp((bstring)DArray_at(lines, 0), bfromcstr("two")) == 0, "First line is wrong.");
  mu_assert(bstrcmp((bstring)DArray_at(lines, 1), bfromcstr("three")) == 0, "Second line is wrong.");
  mu_assert(bstrcmp((bstring)DArray_at(lines, 2), bfromcstr("four")) == 0, "Third line is wrong.");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_getlines);

  return NULL;
}

RUN_TESTS(all_tests);
