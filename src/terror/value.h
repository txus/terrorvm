#ifndef _tvm_value_h_
#define _tvm_value_h_

#include <terror/value_type.h>
#include <terror/function.h>
#include <terror/call_frame.h>
#include <terror/hashmap.h>
#include <sweeper/header.h>
#include <Block.h>

struct state_s;

struct val_s {
  SWPHeader gc;
  ValueType type;
  union {
    double as_num;
    char *as_str;
    void *as_data;
  } data;
  DArray *fields;
  Hashmap *table;
  struct val_s *prototype;
};

typedef struct val_s val_t;
#define VALUE val_t*

VALUE Value_new(struct state_s*, ValueType);
VALUE Value_from_prototype(struct state_s*, ValueType, VALUE);
void Value_destroy(VALUE);
void Value_print(struct state_s*, VALUE);
VALUE Value_to_s(struct state_s*, VALUE);

typedef void (^Slots_iter)(VALUE, VALUE);
void Value_each(VALUE obj, Slots_iter iter);

VALUE Lobby_new(struct state_s*);
VALUE Number_new(struct state_s*, double);
#define VAL2NUM(o) (o->data.as_num)
#define VAL2INT(o) ((int)(o->data.as_num))
#define VAL2BOOL(o) VAL2INT((o))
#define INT2BOOL(o) ((o) ? TrueObject : FalseObject)

VALUE String_new(struct state_s*, char*);
#define VAL2STR(o) ((o)->data.as_str)

VALUE Closure_new(struct state_s*, Function*, CallFrame *scope);
#define VAL2FN(o) ((Function*)(o->data.as_data))

VALUE Vector_new(struct state_s*, DArray *array);
#define VAL2ARY(o) ((DArray*)(o->data.as_data))

VALUE Map_new(struct state_s*, DArray *array);
#define VAL2HASH(o) (o->table)

void Value_set(struct state_s*, VALUE receiver, char *key, VALUE value);
VALUE Value_get(VALUE receiver, char *key);

#endif

