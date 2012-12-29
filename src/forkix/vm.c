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
#define LITERAL(A) (VALUE)DArray_at(current_frame->fn->literals, (A))
#define LOCAL(A) (VALUE)DArray_at(current_frame->locals, (A))
#define LOCALSET(A, B) DArray_set(current_frame->locals, (A), (B))

void VM_start(BytecodeFile *file)
{
  STATE state = State_new(file->functions);

  VALUE main = Main_new(); // toplevel object

  Runtime_init();
  State_bootstrap(state);

  Stack *frames = Stack_create();

  CallFrame *top_frame = CallFrame_new(main, STATE_FN("main"), NULL);

  Stack_push(frames, top_frame);
  VM_run(state, frames);

  Runtime_destroy();
}

VALUE VM_run(STATE state, Stack *frames)
{
  Stack *stack = Stack_create();

  CallFrame *current_frame = (CallFrame*)(Stack_peek(frames));
  int *ip = current_frame->fn->code;

  while(1) {
    switch(*ip) {
      case NOOP:
        break;
      case PUSH: {
        ip++;
        debug("PUSH %i", *ip);
        VALUE value = LITERAL(*ip);
        Stack_push(stack, value);
        break;
      }
      case PUSHTRUE: {
        debug("PUSHTRUE");
        Stack_push(stack, TrueObject);
        break;
      }
      case PUSHFALSE: {
        debug("PUSHFALSE");
        Stack_push(stack, FalseObject);
        break;
      }
      case PUSHNIL: {
        debug("PUSHNIL");
        Stack_push(stack, NilObject);
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

        VALUE value = Stack_peek(stack);
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

        VALUE value = Stack_peek(stack);
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
        VALUE receiver = Stack_pop(stack);
        VALUE slot     = LITERAL(*ip);

        check(slot->type == StringType, "Slot name must be a String.");

        VALUE value = Value_get(receiver, VAL2STR(slot));
        check(value, "Undefined slot %s.", VAL2STR(slot));

        Stack_push(stack, value);
        break;
      }
      case SETSLOT: {
        ip++;
        debug("SETSLOT %i", *ip);
        VALUE value    = Stack_pop(stack);
        VALUE receiver = Stack_pop(stack);
        VALUE slot     = LITERAL(*ip);

        check(slot->type == StringType, "Slot name must be a String.");

        Value_set(receiver, VAL2STR(slot), value);
        Stack_push(stack, value); // push the rhs back to the stack
        break;
      }
      case DEFN: {
        ip++;
        debug("DEFN %i", *ip);
        VALUE fn_name = LITERAL(*ip);
        VALUE closure = Closure_new(STATE_FN(VAL2STR(fn_name)));
        Stack_push(stack, closure);
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
          DArray_push(locals, Stack_pop(stack));
        }
        VALUE receiver = Stack_pop(stack);

        VALUE closure = Value_get(receiver, VAL2STR(name));
        if (op2 == 0 && closure->type != ClosureType && closure != NilObject) {
          // GETSLOT
          Stack_push(stack, closure);
          break;
        }
        check(closure->type == ClosureType, "Cannot find function %s.", VAL2STR(name));
        Function *fn = VAL2FN(closure);

        if(fn->c_fn) {
          // Native function dispatch
          VALUE result = Function_native_call(fn, receiver, locals);
          Stack_push(stack, result);
        } else {
          CallFrame *new_frame = CallFrame_new(receiver, fn, ip++);
          new_frame->locals = locals;

          Stack_push(frames, new_frame);

          current_frame = (CallFrame*)(Stack_peek(frames));

          ip = current_frame->fn->code;
          ip--;
        }
        break;
      }
      case PUSHSELF: {
        debug("PUSHSELF");
        Stack_push(stack, current_frame->self);
        break;
      }
      case PUSHLOCAL: {
        ip++;
        Stack_push(stack, LOCAL(*ip));
        debug("PUSHLOCAL %i", *ip);
        break;
      }
      case SETLOCAL: {
        ip++;
        debug("SETLOCAL %i", *ip);
        LOCALSET(*ip, Stack_peek(stack));
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
        if (ip == NULL) return Stack_pop(stack); // if there's nowhere to return, exit

        current_frame = (CallFrame*)(Stack_peek(frames));
        break;
      }
      case DUMP: {
        debug("DUMP");
        dump(stack);
        break;
      }
    }
    ip++;
  }
error:
  debug("Aborted.");
  return NULL;
}