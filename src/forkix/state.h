#ifndef _fx_state_h_
#define _fx_state_h_

#include <forkix/hashmap.h>

struct state_s {
  Hashmap *functions;
};
typedef struct state_s State;
#define STATE State*

STATE State_new(Hashmap *functions);

#endif
