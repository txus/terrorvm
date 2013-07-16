#include <terror/dbg.h>
#include <terror/file_utils.h>

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
  off_t len = file_size(filename);
  check(len >= 0, "Invalid file length (%s).", bdata(filename));

  char *buf = malloc(len + 1);
  check_mem(buf);

  int fd = open(bdata(filename), O_RDONLY);
  check(fd >= 0, "Error opening file (%s).", bdata(filename));

  ssize_t size = read(fd, buf, len);
  check(size == len, "Read length is invalid");
  check(buf, "Read nothing");

  bstring retval = bfromcstr(buf);
  free(buf);
  return retval;

error:
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
