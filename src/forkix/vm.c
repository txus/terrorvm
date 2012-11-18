#include <forkix/dbg.h>
#include <forkix/value.h>
#include <forkix/stack.h>
#include <forkix/call_frame.h>
#include <forkix/vm.h>
#include <forkix/state.h>
#include <forkix/bstrlib.h>

static void dump(Stack* stack)
{
  printf("---STACK---\n");
  int i = 0;
  STACK_FOREACH(stack, node) {
    if(node) {
      printf("%i. ", i);
      Value_print((VALUE)node->value);
    }
    i++;
  }
}

#define STATE_FN(A) (int*)Hashmap_get(state->functions, bfromcstr((A)))
#define LITERAL(A) DArray_at(literals, (A))
#define LOCAL(A) DArray_at(current_frame->locals, (A))

void VM_start()
{
  int program[] = {
    // main
    PUSHINT, 1,
    PUSHINT, 4,
    SEND, 0, 2,
    DUMP,
    RET,
    // add
    PUSHLOCAL, 0,
    PUSHLOCAL, 1,
    ADD,
    RET
  };

  Hashmap *fns = Hashmap_create(NULL, NULL);
  int *main = &program[0];
  int *add  = &program[9];
  Hashmap_set(fns, bfromcstr("main"), main);
  Hashmap_set(fns, bfromcstr("add"), add);

  STATE state = State_new(fns);

  Stack *frames = Stack_create();
  CallFrame *top_frame = CallFrame_new(STATE_FN("main"), NULL);
  Stack_push(frames, top_frame);
  VM_run(state, frames);
}

void VM_run(STATE state, Stack *frames)
{
  Stack *stack = Stack_create();

  DArray *literals = DArray_create(sizeof(VALUE), 10);
  DArray_push(literals, String_new("add"));

  CallFrame *current_frame = (CallFrame*)(Stack_peek(frames));
  printf("Starting at %i\n", *current_frame->ip);
  int *ip = current_frame->ip;

  while(1) {
    switch(*ip) {
      case PUSHINT: {
        ip++;
        debug("PUSHINT %i", *ip);
        VALUE value = Integer_new(*ip);
        Stack_push(stack, value);
        break;
      }
      case ADD: {
        debug("ADD");
        VALUE op1 = Stack_pop(stack);
        VALUE op2 = Stack_pop(stack);
        int result = VAL2INT(op1) + VAL2INT(op2);

        Stack_push(stack, Integer_new(result));
        break;
      }
      case SEND: {
        ip++;
        int op1 = *ip;
        ip++;
        int op2 = *ip;

        VALUE name = LITERAL(op1);
        int argcount = op2;

        int *fn = STATE_FN(VAL2STR(name));
        CallFrame *new_frame = CallFrame_new(fn, ip++);

        while(argcount--) {
          DArray_push(new_frame->locals, Stack_pop(stack));
        }
        Stack_push(frames, new_frame);

        current_frame = (CallFrame*)(Stack_peek(frames));
        ip = current_frame->ip;
        ip--;

        debug("SEND %i %i", op1, op2);
        break;
      }
      case PUSHLOCAL: {
        ip++;
        Stack_push(stack, LOCAL(*ip));
        debug("PUSHLOCAL %i", *ip);
        break;
      }
      case POP: {
        debug("POP");
        Stack_pop(stack);
        break;
      }
      case RET: {
        debug("RET");
        CallFrame *old_frame = Stack_pop(frames);

        ip = old_frame->ret;
        if (ip == NULL) return; // if there's nowhere to return, exit

        current_frame = (CallFrame*)(Stack_peek(frames));
        break;
      }
      case DUMP: {
        dump(stack);
        break;
      }
    }
    ip++;
  }
}