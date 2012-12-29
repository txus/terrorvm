#include <forkix/dbg.h>
#include <forkix/value.h>
#include <forkix/stack.h>
#include <forkix/call_frame.h>
#include <forkix/vm.h>
#include <forkix/opcode.h>
#include <forkix/state.h>
#include <forkix/runtime.h>
#include <forkix/bootstrap.h>
#include <forkix/bstrlib.h>
#include <forkix/function.h>

// Extern global objects declared in runtime.h
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

static inline void dump(Stack* stack)
{
#ifndef NDEBUG
  printf("---STACK---\n");
  int i = 0;
  STACK_FOREACH(stack, node) {
    if(node) {
      printf("%i. ", i);
      Value_print((VALUE)node->value);
    }
    i++;
  }
#endif
}

#define STATE_FN(A) (Function*)Hashmap_get(state->functions, bfromcstr((A)))
#define LITERAL(A) (VALUE)DArray_at(CURR_FRAME->fn->literals, (A))
#define LOCAL(A) (VALUE)DArray_at(CURR_FRAME->locals, (A))
#define LOCALSET(A, B) DArray_set(CURR_FRAME->locals, (A), (B))

void VM_start(BytecodeFile *file)
{
  STATE = State_new(file->functions);

  VALUE main = Main_new(); // toplevel object

  Runtime_init();
  State_bootstrap(state);

  CallFrame *top_frame = CallFrame_new(main, STATE_FN("main"), NULL);

  FRAMES = Stack_create();
  Stack_push(FRAMES, top_frame);

  VM_run(state);

  Runtime_destroy();
}

VALUE VM_run(STATE)
{
  Stack *stack = Stack_create();
  STACK = stack;

  int *ip = CURR_FRAME->fn->code;

  while(1) {
    switch(*ip) {
      case NOOP:
        break;
      case PUSH: {
        ip++;
        debug("PUSH %i", *ip);
        VALUE value = LITERAL(*ip);
        Stack_push(STACK, value);
        break;
      }
      case PUSHTRUE: {
        debug("PUSHTRUE");
        Stack_push(STACK, TrueObject);
        break;
      }
      case PUSHFALSE: {
        debug("PUSHFALSE");
        Stack_push(STACK, FalseObject);
        break;
      }
      case PUSHNIL: {
        debug("PUSHNIL");
        Stack_push(STACK, NilObject);
        break;
      }
      case JMP: {
        ip++;
        int jump = *ip;
        debug("JMP %i", jump);
        for(int i=1; i < jump; i++) {
          ip++;
        }
        break;
      }
      case JIF: {
        ip++;
        int jump = *ip;
        debug("JIF %i", jump);

        VALUE value = Stack_peek(STACK);
        if (value == FalseObject || value == NilObject) {
          for(int i=1; i < jump; i++) {
            ip++;
          }
        }

        break;
      }
      case JIT: {
        ip++;
        int jump = *ip;
        debug("JIT %i", jump);

        VALUE value = Stack_peek(STACK);
        if (value != FalseObject && value != NilObject) {
          for(int i=1; i < jump; i++) {
            ip++;
          }
        }

        break;
      }
      case GETSLOT: {
        ip++;
        debug("GETSLOT %i", *ip);
        VALUE receiver = Stack_pop(STACK);
        VALUE slot     = LITERAL(*ip);

        check(slot->type == StringType, "Slot name must be a String.");

        VALUE value = Value_get(receiver, VAL2STR(slot));
        check(value, "Undefined slot %s.", VAL2STR(slot));

        Stack_push(STACK, value);
        break;
      }
      case SETSLOT: {
        ip++;
        debug("SETSLOT %i", *ip);
        VALUE value    = Stack_pop(STACK);
        VALUE receiver = Stack_pop(STACK);
        VALUE slot     = LITERAL(*ip);

        check(slot->type == StringType, "Slot name must be a String.");

        Value_set(receiver, VAL2STR(slot), value);
        Stack_push(STACK, value); // push the rhs back to the stack
        break;
      }
      case DEFN: {
        ip++;
        debug("DEFN %i", *ip);
        VALUE fn_name = LITERAL(*ip);
        VALUE closure = Closure_new(STATE_FN(VAL2STR(fn_name)));
        Stack_push(STACK, closure);
        break;
      }
      case MAKEVEC: {
        ip++;
        debug("MAKEVEC %i", *ip);
        int count = *ip;
        DArray *array = DArray_create(sizeof(VALUE), 5);
        while(count--) {
          VALUE elem = Stack_pop(STACK);
          check(elem, "Stack underflow.");
          DArray_push(array, elem);
        }

        VALUE vector = Vector_new(array);
        Stack_push(STACK, vector);
        break;
      }
      case SEND: {
        ip++;
        int op1 = *ip;
        ip++;
        int op2 = *ip;

        debug("SEND %i %i", op1, op2);

        VALUE name = LITERAL(op1);
        int argcount = op2;

        DArray *locals = DArray_create(sizeof(VALUE), 10);
        while(argcount--) {
          DArray_push(locals, Stack_pop(STACK));
        }
        VALUE receiver = Stack_pop(STACK);

        // Special chicken-egg case. We cannot define "apply" as a native method
        // on Closure, since that triggers the creation of a new closure ad
        // infinitum, so we have to handle this special function here.
        if(receiver->type == ClosureType &&
           strcmp(VAL2STR(name), "apply") == 0) {

          DArray *apply_locals = DArray_create(sizeof(VALUE), op2 + 1);
          int argc = op2;
          VALUE rcv = DArray_at(locals, 0);
          for(int i=0; i < argc; i++) {
            DArray_push(apply_locals, DArray_at(locals, i+1));
          }
          ip = Function_call(state, VAL2FN(receiver), rcv, apply_locals, ip);
          ip--;
          break;
        }

        VALUE closure = Value_get(receiver, VAL2STR(name));
        check(closure, "Undefined slot %s.", VAL2STR(name));

        if (op2 == 0 && closure->type != ClosureType && closure != NilObject) {
          // GETSLOT
          Stack_push(STACK, closure);
          break;
        }

        ip = Function_call(state, VAL2FN(closure), receiver, locals, ip);
        ip--; // because we increment after each while cycle
        break;
      }
      case PUSHSELF: {
        debug("PUSHSELF");
        Stack_push(STACK, CURR_FRAME->self);
        break;
      }
      case PUSHLOCAL: {
        ip++;
        Stack_push(STACK, LOCAL(*ip));
        debug("PUSHLOCAL %i", *ip);
        break;
      }
      case SETLOCAL: {
        ip++;
        debug("SETLOCAL %i", *ip);
        LOCALSET(*ip, Stack_peek(STACK));
        break;
      }
      case POP: {
        debug("POP");
        Stack_pop(STACK);
        break;
      }
      case RET: {
        debug("RET");
        CallFrame *old_frame = Stack_pop(FRAMES);

        ip = old_frame->ret;
        if (ip == NULL) return Stack_pop(STACK); // if there's nowhere to return, exit

        break;
      }
      case DUMP: {
        debug("DUMP");
        dump(STACK);
        break;
      }
    }
    ip++;
  }
error:
  debug("Aborted.");
  return NULL;
}