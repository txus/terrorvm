#include <terror/dbg.h>
#include <terror/file_utils.h>

#ifdef __APPLE__
  #include <mach-o/dyld.h>
#elif defined(__linux__)
  #include <limits.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <linux/limits.h>
#endif

bstring
resolve_path(STATE, bstring path)
{
  char *absolute_path = malloc(PATH_MAX);
	bstring h = executable_name(state->binary);
	struct bstrList *x = bsplit(h, '/');
	bdestroy(h);
	bstring slash = bfromcstr("/");
	x->qty--;
	h = bjoin(x, slash);
	x->qty++;
	bstrListDestroy(x);
	bconcat(h, slash);
	bconcat(h, path);
	bdestroy(slash);

  realpath(bdata(h), absolute_path);
  bstring bpath = bfromcstr(absolute_path);
  bdestroy(h);
  free(absolute_path);
  return bpath;
}

bstring
executable_name(bstring binary) {
  char *name = malloc(PATH_MAX);
  memset(name, 0, PATH_MAX);
  char *binary_name = bdata(binary);

#ifdef __APPLE__
  uint32_t size = PATH_MAX;
  if(_NSGetExecutablePath(name, &size) == 0) {
    bstring bname = bfromcstr(name);
    free(name);
    return bname;
  } else if(realpath(binary_name, name)) {
    bstring bname = bfromcstr(name);
    free(name);
    return bname;
  }
#elif defined(__FreeBSD__)
  size_t size = PATH_MAX;
  int oid[4];

  oid[0] = CTL_KERN;
  oid[1] = KERN_PROC;
  oid[2] = KERN_PROC_PATHNAME;
  oid[3] = getpid();

  if(sysctl(oid, 4, name, &size, 0, 0) == 0) {
    bstring bname = bfromcstr(name);
    free(name);
    return bname;
  } else if(realpath(binary_name, name)) {
    bstring bname = bfromcstr(name);
    free(name);
    return bname;
  }
#elif defined(__linux__)
  {
    if(readlink("/proc/self/exe", name, PATH_MAX) >= 0) {
      bstring bname = bfromcstr(name);
      free(name);
      return bname;
    } else if(realpath(binary_name, name)) {
      bstring bname = bfromcstr(name);
      free(name);
      return bname;
    }
  }
#else
  if(realpath(binary_name, name)) {
    bstring bname = bfromcstr(name);
    free(name);
    return bname;
  }
#endif

  free(name);
  return bstrcpy(binary);
}

/*
 * Return the filesize of `filename` or -1.
 */

static inline off_t
file_size(bstring filename) {
  struct stat s;
  int rc = stat(bdata(filename), &s);

  if (rc < 0) return -1;
  return s.st_size;
}

/*
 * Read the contents of `filename` or return NULL.
 */

bstring
readfile(bstring filename) {
  int fd = 0;
  off_t len = file_size(filename);
  check(len >= 0, "Invalid file length (%s).", bdata(filename));

  char *buf = calloc(1, len + 1);
  check_mem(buf);

  fd = open(bdata(filename), O_RDONLY);
  check(fd >= 0, "Error opening file (%s).", bdata(filename));

  ssize_t size = read(fd, buf, len);
  check(size == len, "Read length is invalid");
  check(buf, "Read nothing");

  bstring retval = bfromcstr(buf);
  free(buf);
  close(fd);
  return retval;

error:
  if(fd) close(fd);
  return NULL;
}

DArray*
getlines(struct bstrList *file, int start, int end)
{
  if (start < 0) start = 1;
  if (end > file->qty) end = file->qty;

  int count = end - start + 1;
  DArray *lines = DArray_create(sizeof(bstring), count);

  bstring *line = file->entry;

  while(--start > 0) line++; // advance to the start
  while(count--) {
    DArray_push(lines, *line);
    line++;
  }

  return lines;
}
