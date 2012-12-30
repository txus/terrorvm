#include <terror/bootstrap.h>
#include <terror/darray.h>
#include <terror/bstrlib.h>
#include <terror/runtime.h>
#include <terror/primitives.h>
#include <dirent.h>

VALUE Number_bp;
VALUE String_bp;
VALUE Vector_bp;
VALUE Map_bp;
VALUE Closure_bp;

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

#define DEFPRIM(M, N, F) DArray_push((M), String_new((N))); DArray_push((M), Closure_new(Function_native_new((F))))
#define DEFVALUE(M, N, V) DArray_push((M), String_new((N))); DArray_push((M), (V));

static inline void
expose_VM(VALUE lobby)
{
  VALUE vm = Value_new(ObjectType);
  Value_set(lobby, "VM", vm);

  // VM.primitives map
  DArray *primitives = DArray_create(sizeof(VALUE), 10);

  // Object
  DEFPRIM(primitives, "print", Primitive_print);
  DEFPRIM(primitives, "puts", Primitive_puts);
  DEFPRIM(primitives, "require", Primitive_require);
  DEFPRIM(primitives, "clone", Primitive_clone);

  // Vector
  DEFPRIM(primitives, "vector_[]", Primitive_Vector_at);
  DEFPRIM(primitives, "vector_to_map", Primitive_Vector_to_map);

  // Number
  DEFPRIM(primitives, "number_+", Primitive_Number_add);
  DEFPRIM(primitives, "number_-", Primitive_Number_sub);
  DEFPRIM(primitives, "number_*", Primitive_Number_mul);
  DEFPRIM(primitives, "number_/", Primitive_Number_div);

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

  // Load all files.
  for(int i=0; i < count; i++) {
    bstring filename = (bstring)DArray_at(filenames, i);
    bstring path = bfromcstr("kernel/");
    bconcat(path, filename);
    debug("[BOOTSTRAP] Loading %s...", bdata(path));
    Primitive_require(state, String_new(bdata(path)), NULL, NULL);
  }
  debug("[BOOTSTRAP] Done!");
}
