#ifndef _fx_value_h_
#define _fx_value_h_

#include <forkix/value_type.h>
#include <forkix/gc_header.h>
#include <forkix/hashmap.h>

struct val_s {
  GCHeader gc;
  ValueType type;
  union {
    int as_int;
    char *as_str;
    void *as_data;
  } data;
  Hashmap *table;
};

typedef struct val_s val_t;
#define VALUE val_t*

VALUE Value_new(ValueType);
void Value_destroy(VALUE);
void Value_print(VALUE);

VALUE Integer_new(int);
#define VAL2INT(o) (o->data.as_int)

VALUE String_new(char*);
#define VAL2STR(o) (o->data.as_str)

#define VALSET(S, K, V) Hashmap_set((S)->table, bfromcstr((K)), (V));
#define VALGET(S, K) (VALUE)Hashmap_get((S)->table, bfromcstr((K)))

#endif

