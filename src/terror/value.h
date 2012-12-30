#ifndef _tvm_value_h_
#define _tvm_value_h_

#include <terror/value_type.h>
#include <terror/gc_header.h>
#include <terror/function.h>
#include <terror/call_frame.h>
#include <terror/hashmap.h>

struct val_s {
  GCHeader gc;
  ValueType type;
  union {
    double as_num;
    char *as_str;
    void *as_data;
  } data;
  Hashmap *table;
  struct val_s *prototype;
};

typedef struct val_s val_t;
#define VALUE val_t*

VALUE Value_new(ValueType);
VALUE Value_from_prototype(ValueType, VALUE);
void Value_destroy(VALUE);
void Value_print(VALUE);

VALUE Lobby_new();
VALUE Number_new(double);
#define VAL2NUM(o) (o->data.as_num)
#define VAL2INT(o) ((int)(o->data.as_num))

VALUE String_new(char*);
#define VAL2STR(o) (o->data.as_str)

VALUE Closure_new(Function*, CallFrame *scope);
#define VAL2FN(o) ((Function*)(o->data.as_data))

VALUE Vector_new(DArray *array);
#define VAL2ARY(o) ((DArray*)(o->data.as_data))

VALUE Map_new(DArray *array);
#define VAL2HASH(o) ((Hashmap*)(o->table))

void Value_set(VALUE receiver, char *key, VALUE value);
VALUE Value_get(VALUE receiver, char *key);

#endif

