#include <terror/gc.h>
#include <terror/state.h>

VALUE Object_bp;
VALUE Number_bp;
VALUE String_bp;
VALUE Vector_bp;
VALUE Map_bp;
VALUE Closure_bp;
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

void
GC_release(void *value)
{
  Value_destroy((VALUE)value);
}

void
GC_scan_pointers(TmHeap *heap, TmObjectHeader *object, TmCallbackFn callback)
{
  VALUE val = (VALUE)object;

  for(int i = 0; i < DArray_count(val->fields); i++) {
    callback(heap, (TmObjectHeader*)DArray_at(val->fields, i));
  }

  Value_each(val, ^ void (VALUE key, VALUE val) {
    callback(heap, (TmObjectHeader*)key);
    callback(heap, (TmObjectHeader*)val);
  });
}

#define ROOT(A) if((A)) Tm_DArray_push(rootset, (A))

Tm_DArray*
GC_rootset(TmStateHeader *state_header)
{
  STATE = (State*)state_header;

  Tm_DArray *rootset = Tm_DArray_create(sizeof(VALUE), 20);

  // Runtime values
  ROOT(Object_bp);
  ROOT(Number_bp);
  ROOT(String_bp);
  ROOT(Vector_bp);
  ROOT(Map_bp);
  ROOT(Closure_bp);
  ROOT(TrueObject);
  ROOT(FalseObject);
  ROOT(NilObject);

  // Lobby
  ROOT(state->lobby);

  {
    // Stack
    STACK_FOREACH(state->stack, node) {
      ROOT(node->value);
    }
  }

  {
    // Frames
    STACK_FOREACH(state->frames, node) {

      CallFrame *frame = (CallFrame*)node->value;
      ROOT(frame->self);

      if(frame->parent) {
        CallFrame *parent = frame->parent;
        for(int i=0; i < DArray_count(parent->locals); i++) {
          ROOT(DArray_at(parent->locals, i));
        }
      }

      if(frame->fn->scope) {
        CallFrame *scope = frame->fn->scope;
        for(int i=0; i < DArray_count(scope->locals); i++) {
          ROOT(DArray_at(scope->locals, i));
        }
      }

      for(int i=0; i < DArray_count(frame->locals); i++) {
        ROOT(DArray_at(frame->locals, i));
      }
    }
  }

  return rootset;
}
