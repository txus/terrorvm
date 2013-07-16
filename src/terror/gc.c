#include <terror/gc.h>
#include <sweeper/header.h>
#include <terror/state.h>
#include <terror/input_reader.h>

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
GC_release(SWPHeader *value)
{
  Value_destroy((VALUE)value);
}

void
GC_add_children(SWPHeader *object, SWPArray *children)
{
  VALUE obj = (VALUE)object;

  for(int i = 0; i < DArray_count(obj->fields); i++) {
    SWPArray_push(children, (SWPHeader*)DArray_at(obj->fields, i));
  }

  Value_each(obj, ^ void (VALUE key, VALUE val) {
    SWPArray_push(children, (SWPHeader*)key);
    SWPArray_push(children, (SWPHeader*)val);
  });
}

#define ROOT(A) if((A)) SWPArray_push(roots, (A))

void
GC_add_roots(void *st, SWPArray *roots)
{
  STATE = (State*)st;

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

  //Literals
  for(int i=0; i < DArray_count(state->files); i++) {
    BytecodeFile *file = (BytecodeFile*)DArray_at(state->files, i);

    for(int i=0; i < DArray_count(file->function_names); i++) {
      Function *fn = (Function*)Hashmap_get(file->functions, DArray_at(file->function_names, i));
      for(int i=0; i < DArray_count(fn->literals); i++) {
        ROOT((VALUE)DArray_at(fn->literals, i));
      }
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
}
