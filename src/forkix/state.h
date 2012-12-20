#ifndef _fx_state_h_
#define _fx_state_h_

#include <forkix/hashmap.h>
#include <forkix/darray.h>

struct state_s {
  Hashmap *functions;
  DArray *heap;
};
typedef struct state_s State;
#define STATE State*

STATE State_new(Hashmap *functions);

#endif
