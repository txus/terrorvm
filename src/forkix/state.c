#include <forkix/state.h>

STATE
State_new(Hashmap *functions)
{
  STATE state = calloc(1, sizeof(STATE));
  state->functions = functions;
  return state;
}
