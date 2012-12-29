#include <stdio.h>
#include <terror/dbg.h>
#include <terror/bstrlib.h>
#include <terror/vm.h>

int
main(int argc, char *argv[])
{
  bstring filename = bfromcstr(argv[1]);
  VM_start(filename);
  return 0;
}
