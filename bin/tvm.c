#include <stdio.h>
#include <terror/dbg.h>
#include <terror/bstrlib.h>
#include <terror/debugger.h>
#include <terror/vm.h>

int Debug;

int
main(int argc, char *argv[])
{
  bstring filename = bfromcstr(argv[1]);
  if(argc > 2 && strcmp(argv[2], "-d") == 0) {
    Debug = 1;
  }
  VM_start(filename);
  return 0;
}
