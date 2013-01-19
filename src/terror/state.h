#ifndef _tvm_state_h_
#define _tvm_state_h_

#include <terror/hashmap.h>
#include <terror/darray.h>
#include <terror/value.h>
#include <terror/stack.h>
#include <terror/debugger.h>
#include <terror/gc.h>

struct state_s {
  Hashmap *functions;
  Stack *frames;
  Stack *stack;
  Heap *heap;
  VALUE lobby;
  int *ret;
  Debugger *dbg;
};
typedef struct state_s State;

State* State_new();

#define STATE State* state
#define STACK (state->stack)
#define FRAMES (state->frames)
#define CURR_FRAME ((CallFrame*)(Stack_peek(FRAMES)))
#define DEBUGGER (state->dbg)

#endif

