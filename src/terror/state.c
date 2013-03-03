#include <terror/state.h>
#include <terror/dbg.h>
#include <terror/vm.h>
#include <treadmill/darray.h>
#include <treadmill/gc.h>
#include <terror/gc.h>

VALUE Object_bp;
VALUE Number_bp;
VALUE String_bp;
VALUE Vector_bp;
VALUE Map_bp;
VALUE Closure_bp;
VALUE TrueObject;
VALUE FalseObject;
VALUE NilObject;

State*
State_new()
{
  State *state      = calloc(1, sizeof(State));
  state->functions  = Hashmap_create(NULL, NULL);
  state->frames     = Stack_create();
  state->files      = DArray_create(sizeof(BytecodeFile*), 10);
  state->native_fns = DArray_create(sizeof(Function*), 10);
  state->stack      = Stack_create();
  state->dbg        = Debugger_new();

  state->heap       = TmHeap_new(
    (TmStateHeader*)state,
    10,                  // initial size of the heap
    10,                  // growth rate in number of cells
    5,                   // scan every 200 allocations
    sizeof(val_t),
    GC_release,
    GC_scan_pointers
    );
  state->gc.rootset = GC_rootset;

  return state;
}

void
State_destroy(STATE)
{
  while(Stack_count(state->frames) > 0) {
    CallFrame_destroy((CallFrame*)Stack_pop(state->stack));
  }

  Stack_destroy(state->frames);

  while(Stack_count(state->stack) > 0) Stack_pop(state->stack);

  Stack_destroy(state->stack);

  Debugger_destroy(state->dbg);

  TmHeap_destroy(state->heap);

  TrueObject  = NULL;
  FalseObject = NULL;
  NilObject   = NULL;
  Object_bp   = NULL;
  Number_bp   = NULL;
  String_bp   = NULL;
  Vector_bp   = NULL;
  Map_bp      = NULL;
  Closure_bp  = NULL;

  for(int i = 0; i < DArray_count(state->files); i++) {
    BytecodeFile_destroy((BytecodeFile*)DArray_at(state->files, i));
  }
  DArray_destroy(state->files);

  for(int i = 0; i < DArray_count(state->native_fns); i++) {
    Function_destroy((Function*)DArray_at(state->native_fns, i));
  }
  DArray_destroy(state->native_fns);
  Hashmap_destroy(state->functions);

  free(state);
}
