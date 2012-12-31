#include <terror/value.h>
#include <terror/vector.h>

void Vector_each(VALUE vector, Vector_iter iter)
{
  DArray *array = VAL2ARY(vector);
  int count = DArray_count(array);

  for(int i=0; i<count; i++) {
    iter((VALUE)DArray_at(array, i));
  }
}

void Vector_each_with_index(VALUE vector, Vector_iter_idx iter)
{
  DArray *array = VAL2ARY(vector);
  int count = DArray_count(array);

  for(int i=0; i<count; i++) {
    iter((VALUE)DArray_at(array, i), i);
  }
}
