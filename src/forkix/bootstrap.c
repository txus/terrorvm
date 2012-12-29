#include <forkix/bootstrap.h>
#include <forkix/darray.h>
#include <forkix/bstrlib.h>
#include <forkix/input_reader.h>
#include <dirent.h>

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

void
State_bootstrap(STATE)
{
  DArray *filenames = kernel_files();
  int count = DArray_count(filenames);

  Hashmap *fns = state->functions;

  // Load all files.
  for(int i=0; i < count; i++) {
    bstring filename = (bstring)DArray_at(filenames, i);
    debug("[BOOTSTRAP] Loading %s...", bdata(filename));

    BytecodeFile *file = BytecodeFile_new(filename);
    int fn_count = DArray_count(file->function_names);
    for(int j=0; i < fn_count; i++) {
      bstring fn_name = (bstring)DArray_at(file->function_names, j);
      Function *fn = (Function*)Hashmap_get(file->functions, fn_name);
      Hashmap_set(fns, fn_name, fn);
    }
  }
}

