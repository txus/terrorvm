#ifndef _tvm_vector_h
#define _tvm_vector_h

#include <terror/value.h>
#include <Block.h>
typedef void (^Vector_iter)(VALUE);
typedef void (^Vector_iter_idx)(VALUE, int);

void Vector_each(VALUE, Vector_iter);
void Vector_each_with_index(VALUE, Vector_iter_idx);
#define Vector_at(V, I) (VALUE)(DArray_at(VAL2ARY((V)), (I)))
#define Vector_first(V) (VALUE)(DArray_first(VAL2ARY((V))))
#define Vector_last(V) (VALUE)(DArray_last(VAL2ARY((V))))
#define Vector_count(V) DArray_count(VAL2ARY((V)))

#endif
