#ifndef _tvm_map_h
#define _tvm_map_h

#include <terror/value.h>
#include <terror/bstrlib.h>
#include <terror/darray.h>
#include <terror/hashmap.h>
#include <Block.h>
typedef void (^Map_iter)(bstring, VALUE);

void Map_each(VALUE, Map_iter);

#endif
