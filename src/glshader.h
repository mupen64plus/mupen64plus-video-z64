#ifndef _GLSHADER_H_
#define _GLSHADER_H_

#include "rdp.h"

typedef struct {
  GLhandleARB vs, fs, prog;
#ifdef RDP_DEBUG
  const char * vsrc, * fsrc;
#endif
} rglShader_t;

rglShader_t * rglCreateShader(const char * vsrc, const char * fsrc);
void rglUseShader(rglShader_t * shader);
void rglDeleteShader(rglShader_t * shader);

#endif
