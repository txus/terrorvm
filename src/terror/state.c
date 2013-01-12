#include <terror/state.h>

State*
State_new(Hashmap *functions)
{
  State *state = calloc(1, sizeof(State));
  state->functions = functions;
  state->frames = Stack_create();
  state->stack = Stack_create();
  state->dbg = Debugger_new();
  state->heap = NULL;
  return state;
}
