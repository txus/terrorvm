#include "minunit.h"
#include <forkix/value.h>
#include <forkix/opcode.h>
#include <forkix/runtime.h>
#include <forkix/call_frame.h>
#include <forkix/darray.h>
#include <forkix/state.h>
#include <forkix/bootstrap.h>
#include <forkix/vm.h>
#include <assert.h>

// Extern global objects declared in runtime.h
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

#define VM state
#define STACK frames
#define LOCAL(A) (VALUE)DArray_at(locals, (A))
#define PUSH_LITERAL(A, N) VALUE (N) = (A); DArray_push(literals, (N))
#define PUSH_LOCAL(A, N) VALUE (N) = (A); DArray_push(locals, (N))

#define SETUP() \
  DArray *literals = DArray_create(sizeof(VALUE), 10);  \
  Stack *frames = Stack_create();                       \
  Hashmap *fns = Hashmap_create(NULL, NULL);            \
  DArray *locals = DArray_create(sizeof(VALUE), 10);    \
  Runtime_init();                                       \

#define TEARDOWN()                                      \
  Runtime_destroy();                                    \
  return NULL;                                          \

#define DEFN(N, ...)                                    \
  Hashmap_set(                                          \
    fns,                                                \
    bfromcstr((N)),                                     \
    &(Function) { .code = (int[]){__VA_ARGS__} }        \
  )

#define RUN(...)                                        \
  int code[] = (int[]){__VA_ARGS__};                    \
                                                        \
  Function *fn = calloc(1, sizeof(Function));           \
  fn->code = code;                                      \
  fn->literals = literals;                              \
                                                        \
  STATE state = State_new(fns);                         \
  VALUE main = Value_new(MainType);                     \
  State_bootstrap(state);                               \
  CallFrame *top_frame = CallFrame_new(main, fn, NULL); \
  top_frame->locals = locals;                           \
  Stack_push(frames, top_frame);                        \
  VALUE result = VM_run(state, frames);                 \

char *test_pushself()
{
  SETUP();

  RUN(
    PUSHSELF,
    RET
  );

  mu_assert(result == main, "Pushself failed.");

  TEARDOWN();
}

char *test_push()
{
  SETUP();

  PUSH_LITERAL(Integer_new(123), integer);
  PUSH_LITERAL(String_new("foo"), string);

  RUN(
    PUSH, 0,
    PUSH, 1,
    POP,
    RET
  );

  mu_assert(result == integer, "Push failed.");

  TEARDOWN();
}

char *test_pushtrue()
{
  SETUP();

  RUN(
    PUSHTRUE,
    RET
  );

  mu_assert(result == TrueObject, "Pushtrue failed.");

  TEARDOWN();
}

char *test_pushfalse()
{
  SETUP();

  RUN(
    PUSHFALSE,
    RET
  );

  mu_assert(result == FalseObject, "Pushfalse failed.");

  TEARDOWN();
}

char *test_pushnil()
{
  SETUP();

  RUN(
    PUSHNIL,
    RET
  );

  mu_assert(result == NilObject, "Pushnil failed.");

  TEARDOWN();
}

char *test_pushlocal()
{
  SETUP();

  PUSH_LOCAL(Integer_new(123), integer);

  RUN(
    PUSHLOCAL, 0,
    RET
  );

  mu_assert(result == integer, "Pushlocal failed.");

  TEARDOWN();
}

char *test_setlocal()
{
  SETUP();

  RUN(
    PUSHTRUE,
    SETLOCAL, 0,
    RET
  );

  mu_assert(result == TrueObject, "Setlocal didn't respect stack size.");
  mu_assert(LOCAL(0) == TrueObject, "Setlocal didn't set the local.");

  TEARDOWN();
}

char *test_jmp()
{
  SETUP();

  RUN(
    PUSHTRUE,
    JMP, 2,
    PUSHFALSE,
    RET
  );

  mu_assert(result == TrueObject, "Jmp failed.");

  TEARDOWN();
}

char *test_jif()
{
  SETUP();

  RUN(
    PUSHFALSE,
    JIF, 2,
    PUSHTRUE,
    RET
  );

  mu_assert(result == FalseObject, "Jif failed.");

  TEARDOWN();
}

char *test_jit()
{
  SETUP();

  RUN(
    PUSHTRUE,
    JIT, 2,
    PUSHFALSE,
    RET
  );

  mu_assert(result == TrueObject, "Jit failed.");

  TEARDOWN();
}

char *test_getslot()
{
  SETUP();

  VALUE receiver = String_new("foo");
  Value_set(receiver, "tainted", TrueObject);
  DArray_push(literals, receiver);

  PUSH_LITERAL(String_new("tainted"), slot);

  RUN(
    PUSH, 0, // receiver, the "foo" string
    GETSLOT, 1,
    RET
  );

  mu_assert(result == TrueObject, "Getslot failed.");

  TEARDOWN();
}

char *test_setslot()
{
  SETUP();

  VALUE receiver = String_new("foo");
  Value_set(receiver, "tainted", TrueObject);
  DArray_push(literals, receiver);

  PUSH_LITERAL(String_new("tainted"), slot);

  RUN(
    PUSH, 0, // receiver, the "foo" string
    PUSHFALSE,
    SETSLOT, 1,
    RET
  );

  mu_assert(result == FalseObject, "Setslot didn't push the rhs back to the stack.");
  mu_assert(Value_get(receiver, "tainted") == FalseObject, "Setslot didn't set the slot.");

  TEARDOWN();
}

char *test_pop()
{
  SETUP();

  RUN(
    PUSHTRUE,
    PUSHFALSE,
    POP,
    RET
  );

  mu_assert(result == TrueObject, "Pop failed.");

  TEARDOWN();
}

char *test_send()
{
  SETUP();

  DEFN(
    "echo",
    PUSHLOCAL, 0,
    RET
  );

  PUSH_LITERAL(Integer_new(1), a);
  PUSH_LITERAL(String_new("echo"), method);

  RUN(
    PUSHSELF,
    DEFN, 1,
    SETSLOT, 1,

    PUSHSELF,
    PUSH, 0,
    SEND, 1, 1,
    RET
  );

  mu_assert(VAL2INT(result) == 1, "Send failed.");

  TEARDOWN();
}

char *test_send_getslot()
{
  SETUP();

  PUSH_LITERAL(Integer_new(1), a);
  PUSH_LITERAL(String_new("value"), slot);

  RUN(
    PUSHSELF,
    PUSH, 0,
    SETSLOT, 1,
    POP,

    PUSHSELF,
    SEND, 1, 0,
    RET
  )

  mu_assert(VAL2INT(result) == 1, "Send didn't fall back to getslot.");

  TEARDOWN();
}

char *test_defn()
{
  SETUP();

  DEFN(
    "add",
    PUSHSELF,
    PUSHLOCAL, 0,
    RET
  );

  PUSH_LITERAL(String_new("add"), method);

  RUN(
    DEFN, 0,
    RET
  );

  mu_assert(result->type == ClosureType, "Defn failed.");
  mu_assert(*(VAL2FN(result)->code) == PUSHSELF, "Defn failed.");

  TEARDOWN();
}

char *all_tests() {
  mu_suite_start();

  // Instructions
  mu_run_test(test_pushself);
  mu_run_test(test_push);
  mu_run_test(test_pushtrue);
  mu_run_test(test_pushfalse);
  mu_run_test(test_pushnil);
  mu_run_test(test_pushlocal);
  mu_run_test(test_setlocal);
  mu_run_test(test_jmp);
  mu_run_test(test_jif);
  mu_run_test(test_jit);
  mu_run_test(test_getslot);
  mu_run_test(test_setslot);
  mu_run_test(test_pop);
  mu_run_test(test_send);
  mu_run_test(test_send_getslot);
  mu_run_test(test_defn);

  return NULL;
}

RUN_TESTS(all_tests);
