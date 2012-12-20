#include "minunit.h"
#include <forkix/input_reader.h>
#include <forkix/opcode.h>
#include <forkix/value.h>
#include <assert.h>

char *test_load()
{
  BytecodeFile *file = BytecodeFile_new(bfromcstr("tests/testfile.fx"));

  Function *main = Hashmap_get(file->functions, bfromcstr("main"));
  Function *add = Hashmap_get(file->functions, bfromcstr("add"));

  mu_assert(*main->code == PUSHINT, "error parsing main");
  VALUE first_lit = DArray_first(main->literals);
  mu_assert(strcmp(VAL2STR(first_lit), "add") == 0, "error parsing literal in main");
  mu_assert(*add->code == PUSHSELF, "error parsing add");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_load);

  return NULL;
}

RUN_TESTS(all_tests);
