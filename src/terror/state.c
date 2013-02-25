#include <terror/state.h>
#include <terror/dbg.h>
#include <terror/vm.h>

VALUE Object_bp;
VALUE Number_bp;
VALUE String_bp;
VALUE Vector_bp;
VALUE Map_bp;
VALUE Closure_bp;
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

State*
State_new()
{
  State *state = calloc(1, sizeof(State));
  state->functions = Hashmap_create(NULL, NULL);
  state->frames    = Stack_create();
  state->stack     = Stack_create();
  state->dbg       = Debugger_new();
  state->rootset   = DArray_create(sizeof(VALUE), 10);
  return state;
}

void
State_destroy(STATE)
{
  Stack_destroy(state->frames);

  while(Stack_count(state->stack) > 0) Stack_pop(state->stack);
  Stack_destroy(state->stack);

  Debugger_destroy(state->dbg);
  DArray_destroy(state->rootset);

  TrueObject  = NULL;
  FalseObject = NULL;
  NilObject   = NULL;
  Object_bp   = NULL;
  Number_bp   = NULL;
  String_bp   = NULL;
  Vector_bp   = NULL;
  Map_bp      = NULL;
  Closure_bp  = NULL;

  // TODO destroy functions
}

#define ROOT(A) if((A)) DArray_push(rootset, (A))

DArray*
State_rootset(STATE)
{
  DArray_destroy(state->rootset);
  DArray *rootset = DArray_create(sizeof(VALUE), 20);
  state->rootset = rootset;

  // Runtime values
  ROOT(Object_bp);
  ROOT(Number_bp);
  ROOT(String_bp);
  ROOT(Vector_bp);
  ROOT(Map_bp);
  ROOT(Closure_bp);
  ROOT(TrueObject);
  ROOT(FalseObject);
  ROOT(NilObject);

  // Lobby
  ROOT(state->lobby);

  {
    // Stack
    STACK_FOREACH(state->stack, node) {
      ROOT(node->value);
    }
  }

  {
    // Frames
    STACK_FOREACH(state->frames, node) {

      CallFrame *frame = (CallFrame*)node->value;
      ROOT(frame->self);

      if(frame->parent) {
        CallFrame *parent = frame->parent;
        for(int i=0; i < DArray_count(parent->locals); i++) {
          ROOT(DArray_at(parent->locals, i));
        }
      }

      if(frame->fn->scope) {
        CallFrame *scope = frame->fn->scope;
        for(int i=0; i < DArray_count(scope->locals); i++) {
          ROOT(DArray_at(scope->locals, i));
        }
      }

      for(int i=0; i < DArray_count(frame->locals); i++) {
        ROOT(DArray_at(frame->locals, i));
      }
    }
  }

  return rootset;
}
