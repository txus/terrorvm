#ifndef _fx_function_h_
#define _fx_function_h_

#include <forkix/darray.h>

struct function_s {
  int *code;
  DArray *literals;
};
typedef struct function_s Function;

Function* Function_new(int*, DArray*);

#endif
