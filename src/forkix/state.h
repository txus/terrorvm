#ifndef _fx_state_h_
#define _fx_state_h_

#include <forkix/hashmap.h>
#include <forkix/darray.h>
#include <forkix/stack.h>

struct state_s {
  Hashmap *functions;
  Stack *frames;
  Stack *stack;
  DArray *heap;
};
typedef struct state_s State;

State* State_new(Hashmap *functions);

#define STATE State* state
#define STACK (state->stack)
#define FRAMES (state->frames)
#define CURR_FRAME ((CallFrame*)(Stack_peek(FRAMES)))

#endif
