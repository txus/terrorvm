#include <terror/value.h>
#include <terror/map.h>

void Map_each(VALUE map, Map_iter iter)
{
  Hashmap *hash = VAL2HASH(map);

  for(int i = 0; i < DArray_count(hash->buckets); i++) {
    DArray *bucket = DArray_get(hash->buckets, i);
    if(bucket) {
      for(int j = 0; j < DArray_count(bucket); j++) {
        HashmapNode *node = DArray_get(bucket, j);
        iter((bstring)node->key, (VALUE)node->data);
      }
    }
  }
}

