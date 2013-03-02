#include <terror/dbg.h>
#include <terror/value.h>
#include <terror/stack.h>
#include <terror/call_frame.h>
#include <terror/vm.h>
#include <terror/opcode.h>
#include <terror/state.h>
#include <terror/runtime.h>
#include <terror/bootstrap.h>
#include <terror/bstrlib.h>
#include <terror/function.h>

// Extern global objects declared in runtime.h
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;
int Debug;

void Stack_print(STATE, Stack* stack)
{
  printf("---STACK (%i)---\n", Stack_count(stack));
  int i = 0;
  STACK_FOREACH(stack, node) {
    if(node) {
      printf("\n%i. ", i);
      Value_print(state, (VALUE)node->value);
    }
    i++;
  }
  printf("\n");
}

void Value_print_all(STATE, DArray* objs)
{
  for(int i=0; i < DArray_count(objs); i++) {
    Value_print(state, (VALUE)DArray_at(objs, i));
    printf("\n");
  }
}

#define STATE_FN(A) (Function*)Hashmap_get(state->functions, (A))
#define LITERAL(A) (VALUE)DArray_at(CURR_FRAME->fn->literals, (A))
#define LOCAL(A) CallFrame_getlocal(CURR_FRAME, (A))
#define LOCALSET(A, B) CallFrame_setlocal(CURR_FRAME, (A), (B))
#define DEEPLOCAL(D, A) CallFrame_getlocaldepth(CURR_FRAME, (D), (A))
#define DEEPLOCALSET(D, A, B) CallFrame_setlocaldepth(CURR_FRAME, (D), (A), (B))

void VM_start(bstring filename)
{
  STATE = State_new();

  Runtime_init(state);

  VALUE lobby  = Lobby_new(state); // toplevel object
  state->lobby = lobby;

  BytecodeFile *file = BytecodeFile_new(state, filename);
  state->functions = file->functions;

  bstring main_fn = bfromcstr("0_main");
  CallFrame *top_frame = CallFrame_new(lobby, STATE_FN(main_fn), NULL);
  bdestroy(main_fn);
  top_frame->name = "main";

  Stack_push(FRAMES, top_frame);

  // now we're ready to bootstrap
  State_bootstrap(state);

  // and run the codes!
  VM_run(state);

  State_destroy(state);
}

VALUE VM_run(STATE)
{
  Debugger_load_current_file(state);

  int *ip = CURR_FRAME->fn->code;

  while(1) {
    switch(*ip) {
      case NOOP:
        break;
      case SETLINE: { // debugging
        ip++;
        Debugger_setline(state, *ip);
        break;
      }
      case PUSH: {
        Debugger_evaluate(state);
        ip++;
        debugi("PUSH %i", *ip);
        VALUE value = LITERAL(*ip);
        Stack_push(STACK, value);
        break;
      }
      case PUSHTRUE: {
        Debugger_evaluate(state);
        debugi("PUSHTRUE");
        Stack_push(STACK, TrueObject);
        break;
      }
      case PUSHFALSE: {
        Debugger_evaluate(state);
        debugi("PUSHFALSE");
        Stack_push(STACK, FalseObject);
        break;
      }
      case PUSHNIL: {
        Debugger_evaluate(state);
        debugi("PUSHNIL");
        Stack_push(STACK, NilObject);
        break;
      }
      case JMP: {
        Debugger_evaluate(state);
        ip++;
        int jump = *ip;
        debugi("JMP %i", jump);
        while(jump--) ip++;
        break;
      }
      case JIF: {
        Debugger_evaluate(state);
        ip++;
        int jump = *ip;
        debugi("JIF %i", jump);

        VALUE value = Stack_pop(STACK);
        if (value == FalseObject || value == NilObject) {
          while(jump--) ip++;
        }

        break;
      }
      case JIT: {
        Debugger_evaluate(state);
        ip++;
        int jump = *ip;
        debugi("JIT %i", jump);

        VALUE value = Stack_pop(STACK);
        if (value != FalseObject && value != NilObject) {
          while(jump--) ip++;
        }

        break;
      }
      case GOTO: {
        Debugger_evaluate(state);
        ip++;
        int jump = *ip - 2;

        debugi("GOTO %i", jump);

        ip = CURR_FRAME->fn->code;
        while(jump--) ip++;

        break;
      }
      case GETSLOT: {
        Debugger_evaluate(state);
        ip++;
        debugi("GETSLOT %i", *ip);
        VALUE receiver = Stack_pop(STACK);
        VALUE slot     = LITERAL(*ip);

        check(receiver->type != NilType, "Tried to get a slot from nil.");
        check(slot->type == StringType, "Slot name must be a String.");

        VALUE value = Value_get(receiver, VAL2STR(slot));
        check(value, "Undefined slot %s on object type %i.", VAL2STR(slot), receiver->type);

        Stack_push(STACK, value);
        break;
      }
      case SETSLOT: {
        Debugger_evaluate(state);
        ip++;
        debugi("SETSLOT %i", *ip);
        VALUE value    = Stack_pop(STACK);
        VALUE receiver = Stack_pop(STACK);
        VALUE slot     = LITERAL(*ip);

        check(receiver->type != NilType, "Tried to set a slot on nil.");
        check(slot->type == StringType, "Slot name must be a String.");

        Value_set(state, receiver, VAL2STR(slot), value);
        Stack_push(STACK, value); // push the rhs back to the stack
        break;
      }
      case DEFN: {
        Debugger_evaluate(state);
        ip++;
        debugi("DEFN %i", *ip);
        VALUE fn_name = LITERAL(*ip);
        bstring state_fn = bfromcstr(VAL2STR(fn_name));
        VALUE closure = Closure_new(state, STATE_FN(state_fn), CURR_FRAME);
        bdestroy(state_fn);
        Stack_push(STACK, closure);
        break;
      }
      case MAKEVEC: {
        Debugger_evaluate(state);
        ip++;
        debugi("MAKEVEC %i", *ip);
        int count = *ip;
        DArray *array = DArray_create(sizeof(VALUE), 5);
        while(count--) {
          VALUE elem = Stack_pop(STACK);
          check(elem, "Stack underflow.");
          DArray_push(array, elem);
        }

        VALUE vector = Vector_new(state, array);
        Stack_push(STACK, vector);
        break;
      }
      case SEND: {
        Debugger_evaluate(state);
        ip++;
        int op1 = *ip;
        ip++;
        int op2 = *ip;

        debugi("SEND %i %i", op1, op2);

        VALUE name = LITERAL(op1);
        int argcount = op2;

        DArray *locals = DArray_create(sizeof(VALUE), argcount);
        while(argcount--) {
          DArray_push(locals, Stack_pop(STACK));
        }
        VALUE receiver = Stack_pop(STACK);

        // Special chicken-egg case. We cannot define "apply" as a native method
        // on Closure, since that triggers the creation of a new closure ad
        // infinitum, so we have to handle this special function here.
        if(receiver->type == ClosureType &&
           strcmp(VAL2STR(name), "apply") == 0) {

          state->ret = ip; // save where we want to return
          ip = Function_call(state, VAL2FN(receiver), CURR_FRAME->self, locals, VAL2STR(name));
          break;
        }

        VALUE closure = Value_get(receiver, VAL2STR(name));
        check(closure, "Undefined slot %s on object type %i.", VAL2STR(name), receiver->type);

        if (closure->type != ClosureType && closure != NilObject) {
          // GETSLOT
          Stack_push(STACK, closure);
          break;
        }

#ifdef OPTIMIZE_SEND
        if(op2 == 1 && strcmp(VAL2STR(name), "[]") == 0) { // getslot
          VALUE key = (VALUE)DArray_at(locals, 0);
          Stack_push(STACK, Value_get(receiver, VAL2STR(key)));
          break;
        }

        if(op2 == 2 && strcmp(VAL2STR(name), "[]=") == 0) { // setslot
          VALUE key   = (VALUE)DArray_at(locals, 0);
          VALUE value = (VALUE)DArray_at(locals, 1);
          Value_set(receiver, VAL2STR(key), value);
          Stack_push(STACK, value);
          break;
        }
#endif

        state->ret = ip; // save where we want to return
        ip = Function_call(state, VAL2FN(closure), receiver, locals, VAL2STR(name));
        break;
      }
      case PUSHLOBBY: {
        Debugger_evaluate(state);
        debugi("PUSHLOBBY");
        Stack_push(STACK, state->lobby);
        break;
      }
      case PUSHSELF: {
        Debugger_evaluate(state);
        debugi("PUSHSELF");
        Stack_push(STACK, CURR_FRAME->self);
        break;
      }
      case PUSHLOCAL: {
        Debugger_evaluate(state);
        ip++;
        Stack_push(STACK, LOCAL(*ip));
        debugi("PUSHLOCAL %i", *ip);
        break;
      }
      case PUSHLOCALDEPTH: {
        Debugger_evaluate(state);
        ip++;
        int depth = *ip;
        ip++;
        Stack_push(STACK, DEEPLOCAL(depth, *ip));
        debugi("PUSHLOCALDEPTH %i %i", depth, *ip);
        break;
      }
      case SETLOCAL: {
        Debugger_evaluate(state);
        ip++;
        debugi("SETLOCAL %i", *ip);
        LOCALSET(*ip, Stack_peek(STACK));
        break;
      }
      case SETLOCALDEPTH: {
        Debugger_evaluate(state);
        ip++;
        int depth = *ip;
        ip++;
        debugi("SETLOCAL %i %i", depth, *ip);
        DEEPLOCALSET(depth, *ip, Stack_peek(STACK));
        break;
      }
      case POP: {
        Debugger_evaluate(state);
        ip++;
        int count = *ip;
        debugi("POP %i", count);
        check(Stack_count(STACK) >= count, "Stack underflow.");
        while(count--) Stack_pop(STACK);
        break;
      }
      case CLEAR: {
        Debugger_evaluate(state);
        ip++;
        int count = *ip;
        debugi("CLEAR %i", count);
        check(Stack_count(STACK) > count, "Stack underflow.");
        VALUE tos = Stack_pop(STACK);
        while(count--) Stack_pop(STACK);
        Stack_push(STACK, tos);
        break;
      }
      case RET: {
        Debugger_evaluate(state);
        debugi("RET");
        CallFrame *old_frame = Stack_pop(FRAMES);

        ip = old_frame->ret;

        CallFrame_destroy(old_frame);

        check(Stack_count(STACK) > 0, "Stack underflow.");

        if (ip == NULL) return Stack_pop(STACK); // if there's nowhere to return, exit

        break;
      }
      case DUMP: {
        Debugger_evaluate(state);
        debugi("DUMP");
        Stack_print(state, STACK);
        DArray *literals = CURR_FRAME->fn->literals;
        printf("--LITERALS (%i)--\n", DArray_count(literals));
        Value_print_all(state, literals);

        DArray *locals = CURR_FRAME->locals;
        printf("--LOCALS (%i)--\n", DArray_count(locals));
        Value_print_all(state, locals);
        break;
      }
    }
    ip++;
  }
error:
  debug("Aborted.");
  CallFrame_print_backtrace(CURR_FRAME);
  exit(EXIT_FAILURE);
  return NULL;
}
