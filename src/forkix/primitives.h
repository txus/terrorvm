#ifndef _fx_primitives_h_
#define _fx_primitives_h_

#include <forkix/value.h>
#include <assert.h>

#define CHECK_TYPE(O, T) assert((O)->type == (T) && "Type mismatch in primitive operation")

// Generic primitive methods
VALUE Primitive_print(void*, void*, void*);
VALUE Primitive_puts(void*, void*, void*);

// Integer primitive methods
VALUE Primitive_Integer_add(void*, void*, void*);
VALUE Primitive_Integer_sub(void*, void*, void*);
VALUE Primitive_Integer_mul(void*, void*, void*);
VALUE Primitive_Integer_div(void*, void*, void*);

#endif
