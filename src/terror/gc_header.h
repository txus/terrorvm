#ifndef _tvm_gc_header_h_
#define _tvm_gc_header_h_

struct gc_header_s;
typedef struct gc_header_s {
  struct gc_header_s *next;
  struct gc_header_s *prev;
  // char ecru;
} GCHeader;

#endif
