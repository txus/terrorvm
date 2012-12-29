#include <stdio.h>
#include <forkix/dbg.h>
#include <forkix/bstrlib.h>
#include <forkix/vm.h>

int
main(int argc, char *argv[])
{
  bstring filename = bfromcstr(argv[1]);
  VM_start(filename);
  return 0;
}
