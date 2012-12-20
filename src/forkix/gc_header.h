#ifndef _fx_gc_header_h_
#define _fx_gc_header_h_

struct gc_header_s {
  short int marked;
};

typedef struct gc_header_s GCHeader;

#endif
