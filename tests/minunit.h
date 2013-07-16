#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <sweeper/dbg.h>
#include <stdlib.h>

#define mu_suite_start() char *message = NULL
#define mu_assert(test, message) assertions++; if (!(test)) { printf("\e[31mF\e[0m\n"); log_err(message); return message; } else { printf("\e[32m.\e[0m"); }
#define mu_run_test(test) \
  message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
  argc = 1; \
    char *result = name();\
    if (result != 0) {\
      printf("FAILED: %s", result);\
    }\
  printf("\n%d tests, %d assertions\n", tests_run, assertions);\
    exit(result != 0);\
}

int tests_run;
int assertions;

#endif
