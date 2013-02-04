#include "minunit.h"
#include <terror/value.h>
#include <terror/opcode.h>
#include <terror/runtime.h>
#include <terror/call_frame.h>
#include <terror/darray.h>
#include <terror/state.h>
#include <terror/bootstrap.h>
#include <terror/vm.h>
#include <terror/state.h>
#include <assert.h>

// Extern global objects declared in runtime.h
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

#define LOCAL(A) (VALUE)DArray_at(locals, (A))
#define PUSH_LITERAL(A, N) VALUE (N) = (A); DArray_push(literals, (N))
#define PUSH_LOCAL(A, N) VALUE (N) = (A); DArray_push(locals, (N))

#define SETUP() \
  DArray *literals = DArray_create(sizeof(VALUE), 10);  \
  Hashmap *fns = Hashmap_create(NULL, NULL);            \
  DArray *locals = DArray_create(sizeof(VALUE), 10);    \
  STATE = State_new();                                  \
  state->functions = fns;                               \
  Runtime_init(state);                                  \

#define TEARDOWN()                                      \
  State_destroy(state);                                 \
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
  fn->code     = code;                                  \
  fn->literals = literals;                              \
                                                        \
  VALUE lobby = Lobby_new(state);                       \
  state->lobby = lobby;                                 \
  CallFrame *top_frame = CallFrame_new(lobby, fn, NULL);\
  top_frame->locals = locals;                           \
  Stack_push(FRAMES, top_frame);                        \
  State_bootstrap(state);                               \
  VALUE result = VM_run(state);                         \

char *test_pushself()
{
  SETUP();

  RUN(
    PUSHSELF,
    RET
  );

  mu_assert(result == lobby, "Pushself failed.");

  TEARDOWN();
}

char *test_pushlobby()
{
  SETUP();

  RUN(
    PUSHLOBBY,
    RET
  );

  mu_assert(result == lobby, "Pushlobby failed.");

  TEARDOWN();
}

char *test_push()
{
  SETUP();

  PUSH_LITERAL(Number_new(state, 123), integer);
  PUSH_LITERAL(String_new(state, "foo"), string);

  RUN(
    PUSH, 0,
    PUSH, 1,
    POP, 1,
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

  PUSH_LOCAL(Number_new(state, 123), integer);

  RUN(
    PUSHLOCAL, 0,
    RET
  );

  mu_assert(result == integer, "Pushlocal failed.");

  TEARDOWN();
}

char *test_pushlocaldepth()
{
  SETUP();

  DEFN(
    "foo",
    PUSHLOCALDEPTH, 1, 0,
    RET
  );

  PUSH_LITERAL(Number_new(state, 123), integer);
  PUSH_LITERAL(String_new(state, "foo"), method);
  PUSH_LITERAL(String_new(state, "fn"), function);

  // a = 123
  // self.fn = -> { a }
  // fn()
  RUN(
    PUSH, 0,
    SETLOCAL, 0,
    POP, 1,

    PUSHSELF,
    DEFN, 1,
    SETSLOT, 2,
    POP, 1,

    PUSHSELF,
    SEND, 2, 0,
    RET
  );

  mu_assert(result == integer, "Pushlocaldepth failed.");

  TEARDOWN();
}

char *test_setlocaldepth()
{
  SETUP();

  DEFN(
    "foo",
    PUSHNIL,
    SETLOCALDEPTH, 1, 0,
    RET
  );

  PUSH_LITERAL(Number_new(state, 123), integer);
  PUSH_LITERAL(String_new(state, "foo"), method);
  PUSH_LITERAL(String_new(state, "fn"), function);

  // a = 123
  // self.fn = -> { a }
  // fn()
  RUN(
    PUSH, 0,
    SETLOCAL, 0,

    PUSHSELF,
    DEFN, 1,
    SETSLOT, 2,

    PUSHSELF,
    SEND, 2, 0,
    RET
  );

  mu_assert(result == NilObject, "Setlocaldepth failed.");

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
    JMP, 1,
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
    PUSHNIL,
    PUSHFALSE,
    JIF, 1,
    PUSHTRUE,
    RET
  );

  mu_assert(result == NilObject, "Jif failed.");

  TEARDOWN();
}

char *test_jit()
{
  SETUP();

  RUN(
    PUSHNIL,
    PUSHTRUE,
    JIT, 1,
    PUSHFALSE,
    RET
  );

  mu_assert(result == NilObject, "Jit failed.");

  TEARDOWN();
}

char *test_goto()
{
  SETUP();

  RUN(
    PUSHFALSE,
    PUSHTRUE,
    GOTO, 5,
    RET
  );

  mu_assert(result == TrueObject, "Goto failed.");

  TEARDOWN();
}

char *test_getslot()
{
  SETUP();

  VALUE receiver = String_new(state, "foo");
  Value_set(state, receiver, "tainted", TrueObject);
  DArray_push(literals, receiver);

  PUSH_LITERAL(String_new(state, "tainted"), slot);

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

  VALUE receiver = String_new(state, "foo");
  Value_set(state, receiver, "tainted", TrueObject);
  DArray_push(literals, receiver);

  PUSH_LITERAL(String_new(state, "tainted"), slot);

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
    PUSHNIL,
    PUSHTRUE,
    PUSHFALSE,
    POP, 2,
    RET
  );

  mu_assert(result == NilObject, "Pop failed.");

  TEARDOWN();
}

char *test_clear()
{
  SETUP();

  RUN(
    PUSHNIL,
    PUSHTRUE,
    PUSHFALSE,
    CLEAR, 2,
    RET
  );

  mu_assert(result == FalseObject, "Clear failed.");
  mu_assert(Stack_count(STACK) == 0, "Clear failed");

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

  PUSH_LITERAL(Number_new(state, 1), a);
  PUSH_LITERAL(String_new(state, "echo"), method);

  RUN(
    PUSHSELF,
    DEFN, 1,
    SETSLOT, 1,

    PUSHSELF,
    PUSH, 0,
    SEND, 1, 1,
    RET
  );

  mu_assert(VAL2NUM(result) == 1, "Send failed.");

  TEARDOWN();
}

char *test_send_getslot()
{
  SETUP();

  PUSH_LITERAL(Number_new(state, 1), a);
  PUSH_LITERAL(String_new(state, "value"), slot);

  RUN(
    PUSHSELF,
    PUSH, 0,
    SETSLOT, 1,
    POP, 1,

    PUSHSELF,
    SEND, 1, 0,
    RET
  )

  mu_assert(VAL2NUM(result) == 1, "Send didn't fall back to getslot.");

  TEARDOWN();
}

char *test_send_apply()
{
  SETUP();

  DEFN(
    "echo",
    PUSHLOCAL, 0,
    RET
  );

  PUSH_LITERAL(Number_new(state, 1), a);
  PUSH_LITERAL(String_new(state, "echo"), method);
  PUSH_LITERAL(String_new(state, "apply"), apply);

  RUN(
    DEFN, 1,

    // self == closure, method = apply, first arg == Number(1)
    PUSH, 0,
    SEND, 2, 1,
    RET
  );

  mu_assert(VAL2NUM(result) == 1, "Send apply failed.");

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

  PUSH_LITERAL(String_new(state, "add"), method);

  RUN(
    DEFN, 0,
    RET
  );

  mu_assert(result->type == ClosureType, "Defn failed.");
  mu_assert(*(VAL2FN(result)->code) == PUSHSELF, "Defn failed.");

  TEARDOWN();
}

char *test_makevec()
{
  SETUP();

  RUN(
    PUSHFALSE,
    PUSHTRUE,
    MAKEVEC, 2,
    RET
  );

  mu_assert(result->type == VectorType, "Makevec failed.");
  DArray *array = VAL2ARY(result);
  mu_assert(((VALUE)DArray_first(array))->type == TrueType, "First value is not true.");
  mu_assert(((VALUE)DArray_last(array))->type == FalseType, "Last value is not false.");

  TEARDOWN();
}

char *all_tests() {
  mu_suite_start();

  // Instructions
  mu_run_test(test_pushself);
  mu_run_test(test_pushlobby);
  mu_run_test(test_push);
  mu_run_test(test_pushtrue);
  mu_run_test(test_pushfalse);
  mu_run_test(test_pushnil);
  mu_run_test(test_pushlocal);
  mu_run_test(test_setlocal);
  mu_run_test(test_pushlocaldepth);
  mu_run_test(test_setlocaldepth);
  mu_run_test(test_jmp);
  mu_run_test(test_jif);
  mu_run_test(test_jit);
  mu_run_test(test_goto);
  mu_run_test(test_getslot);
  mu_run_test(test_setslot);
  mu_run_test(test_pop);
  mu_run_test(test_clear);
  mu_run_test(test_send);
  mu_run_test(test_send_getslot);
  mu_run_test(test_send_apply);
  mu_run_test(test_defn);
  mu_run_test(test_makevec);

  return NULL;
}

RUN_TESTS(all_tests);
