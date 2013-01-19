#include <terror/state.h>

State*
State_new()
{
  State *state = calloc(1, sizeof(State));
  state->functions = Hashmap_create(NULL, NULL);
  state->frames    = Stack_create();
  state->stack     = Stack_create();
  state->dbg       = Debugger_new();
  state->heap      = Heap_new();
  return state;
}
