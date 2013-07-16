#include <terror/bootstrap.h>
#include <terror/darray.h>
#include <terror/bstrlib.h>
#include <terror/runtime.h>
#include <terror/primitives.h>
#include <terror/debugger.h>
#include <dirent.h>

VALUE Number_bp;
VALUE String_bp;
VALUE Vector_bp;
VALUE Map_bp;
VALUE Closure_bp;
int Debug;

static inline DArray*
kernel_files()
{
  DArray *entries = DArray_create(sizeof(bstring), 10);

  DIR *dirp = opendir("kernel"); // todo -- make it an absolute path
  struct dirent *dp;
  readdir(dirp); // .
  readdir(dirp); // ..

  while ((dp = readdir(dirp)) != NULL) {
    DArray_push(entries, bfromcstr(dp->d_name));
  }
  (void)closedir(dirp);

  return entries;
}

#define DEFPRIM(N, F) Value_set(state, primitives, (N), Closure_new(state, Function_native_new(state, (F)), NULL))
#define DEFVALUE(N, V) Value_set(state, types, (N), (V))

static inline void
expose_VM(STATE, VALUE lobby)
{
  VALUE vm = Value_new(state, ObjectType);
  Value_set(state, lobby, "VM", vm);

  // VM.primitives map
  DArray *prims    = DArray_create(sizeof(VALUE), 10);
  VALUE primitives = Map_new(state, prims);
  Value_set(state, vm, "primitives", primitives);

  // Object
  DEFPRIM("to_s", Primitive_to_s);
  DEFPRIM("prototype", Primitive_prototype);
  DEFPRIM("or", Primitive_or);

  DEFPRIM("equals", Primitive_equals);
  DEFPRIM("is", Primitive_is);
  DEFPRIM("print", Primitive_print);
  DEFPRIM("puts", Primitive_puts);
  DEFPRIM("require", Primitive_require);
  DEFPRIM("clone", Primitive_clone);

  // Vector
  DEFPRIM("vector_[]", Primitive_Vector_at);
  DEFPRIM("vector_push", Primitive_Vector_push);
  DEFPRIM("vector_to_map", Primitive_Vector_to_map);
  DEFPRIM("vector_each", Primitive_Vector_each);
  DEFPRIM("vector_each_with_index", Primitive_Vector_each_with_index);

  // Number
  DEFPRIM("number_+", Primitive_Number_add);
  DEFPRIM("number_-", Primitive_Number_sub);
  DEFPRIM("number_*", Primitive_Number_mul);
  DEFPRIM("number_/", Primitive_Number_div);
  DEFPRIM("number_<", Primitive_Number_lt);
  DEFPRIM("number_>", Primitive_Number_gt);

  // String
  DEFPRIM("string_+", Primitive_String_concat);

  // Map
  DEFPRIM("map_each", Primitive_Map_each);

  // VM.types map
  DArray *ts = DArray_create(sizeof(VALUE), 10);
  VALUE types = Map_new(state, ts);
  Value_set(state, vm, "types", types);

  DEFVALUE("object", Object_bp);
  DEFVALUE("number", Number_bp);
  DEFVALUE("string", String_bp);
  DEFVALUE("vector", Vector_bp);
  DEFVALUE("map", Map_bp);
  DEFVALUE("closure", Closure_bp);
}

void
State_bootstrap(STATE)
{
  /* GC_disable(state->heap); // disable GC */

  DArray *filenames = kernel_files();
  int count = DArray_count(filenames);

  // Expose toplevel constants
  expose_VM(state, state->lobby);

  int reenable_debugger = 0;

  // Disable debugger while loading kernel files
  if(Debug) {
    reenable_debugger = 1;
    Debug = 0;
  }

  // Load all files.
  for(int i=0; i < count; i++) {
    bstring filename = (bstring)DArray_at(filenames, i);
    bstring path = bfromcstr("kernel/");
    bconcat(path, filename);
    debug("[BOOTSTRAP] Loading %s...", bdata(path));
    Primitive_require(state, String_new(state, bdata(path)), NULL, NULL);

    bdestroy(path);
    bdestroy(filename);
  }

  DArray_destroy(filenames);

  // Reenable debugger if needed
  if(reenable_debugger) Debug = 1;

  debug("[BOOTSTRAP] Done!");
  /* GC_enable(state->heap); // disable GC */
}
