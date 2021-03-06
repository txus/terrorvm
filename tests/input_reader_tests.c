#include "minunit.h"
#include <terror/input_reader.h>
#include <terror/opcode.h>
#include <terror/value.h>
#include <terror/state.h>
#include <assert.h>

STATE = NULL;

char *test_load()
{
  BytecodeFile *file = BytecodeFile_new(state, bfromcstr("tests/testfile.tvm"));

  bstring main_name = bfromcstr("0_main");
  Function *main = Hashmap_get(file->functions, main_name);
  bstring add_name = bfromcstr("4_add");
  Function *add = Hashmap_get(file->functions, add_name);

  mu_assert(*main->code == PUSH, "error parsing main");

  VALUE first_lit = DArray_first(main->literals);
  mu_assert(strcmp(VAL2STR(first_lit), "add") == 0, "error parsing literal in main");

  VALUE first_num = DArray_at(main->literals, 1);
  mu_assert(VAL2NUM(first_num) == 1.2, "error parsing float literal in main");

  mu_assert(*add->code == PUSHSELF, "error parsing add");

  bdestroy(main_name);
  bdestroy(add_name);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  state = State_new();
  mu_run_test(test_load);

  return NULL;
}

RUN_TESTS(all_tests);
