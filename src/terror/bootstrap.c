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

#define DEFPRIM(M, N, F) DArray_push((M), String_new((N))); DArray_push((M), Closure_new(Function_native_new((F)), NULL))
#define DEFVALUE(M, N, V) DArray_push((M), String_new((N))); DArray_push((M), (V));

static inline void
expose_VM(VALUE lobby)
{
  VALUE vm = Value_new(ObjectType);
  Value_set(lobby, "VM", vm);

  // VM.primitives map
  DArray *primitives = DArray_create(sizeof(VALUE), 10);

  // Object
  DEFPRIM(primitives, "prototype", Primitive_prototype);
  DEFPRIM(primitives, "or", Primitive_or);

  DEFPRIM(primitives, "equals", Primitive_equals);
  DEFPRIM(primitives, "is", Primitive_is);
  DEFPRIM(primitives, "print", Primitive_print);
  DEFPRIM(primitives, "puts", Primitive_puts);
  DEFPRIM(primitives, "require", Primitive_require);
  DEFPRIM(primitives, "clone", Primitive_clone);

  // Vector
  DEFPRIM(primitives, "vector_[]", Primitive_Vector_at);
  DEFPRIM(primitives, "vector_push", Primitive_Vector_push);
  DEFPRIM(primitives, "vector_to_map", Primitive_Vector_to_map);
  DEFPRIM(primitives, "vector_each", Primitive_Vector_each);
  DEFPRIM(primitives, "vector_each_with_index", Primitive_Vector_each_with_index);

  // Number
  DEFPRIM(primitives, "number_+", Primitive_Number_add);
  DEFPRIM(primitives, "number_-", Primitive_Number_sub);
  DEFPRIM(primitives, "number_*", Primitive_Number_mul);
  DEFPRIM(primitives, "number_/", Primitive_Number_div);

  // String
  DEFPRIM(primitives, "string_+", Primitive_String_concat);

  // Map
  DEFPRIM(primitives, "map_each", Primitive_Map_each);

  Value_set(vm, "primitives", Map_new(primitives));

  // VM.types map
  DArray *types = DArray_create(sizeof(VALUE), 10);

  DEFVALUE(types, "object", Object_bp);
  DEFVALUE(types, "number", Number_bp);
  DEFVALUE(types, "string", String_bp);
  DEFVALUE(types, "vector", Vector_bp);
  DEFVALUE(types, "map", Map_bp);
  DEFVALUE(types, "closure", Closure_bp);

  Value_set(vm, "types", Map_new(types));
}

void
State_bootstrap(STATE)
{
  DArray *filenames = kernel_files();
  int count = DArray_count(filenames);

  // Expose toplevel constants
  expose_VM(state->lobby);

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
    Primitive_require(state, String_new(bdata(path)), NULL, NULL);
  }

  // Reenable debugger if needed
  if(reenable_debugger) Debug = 1;

  debug("[BOOTSTRAP] Done!");
}
