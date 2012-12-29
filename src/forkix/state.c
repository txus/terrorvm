#include <forkix/state.h>

State*
State_new(Hashmap *functions)
{
  State *state = calloc(1, sizeof(State));
  state->functions = functions;
  state->frames = NULL;
  state->stack = NULL;
  state->heap = NULL;
  return state;
}
