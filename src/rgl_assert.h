#ifndef _RGL_ASSERT_H_
#define _RGL_ASSERT_H_

#include <stdio.h>

#ifdef RGL_ASSERT
inline void _rglAssert(int test, const char * s, int line, const char * file) {
  if (!test) {
    fprintf(stderr, "z64 assert failed (%s : %d) : %s\n", file, line, s);
    fflush(stdout);
    fflush(stderr);
    *(unsigned int *)0 = 0xdeadbeef; // hopefully will generate a segfault
    exit(-1);
  }
}
#define rglAssert(test) _rglAssert((test), #test, __LINE__, __FILE__)
#else
#define rglAssert(test)
#endif

#endif
