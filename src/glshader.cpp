#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rgl_assert.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "glshader.h"

static void printInfoLog(GLhandleARB obj, const char * src)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;
	
  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                            &infologLength);
	
  if (infologLength > 0)
  {
		infoLog = (char *)malloc(infologLength);
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
    if (*infoLog)
      printf("%s\n%s", src, infoLog);
		free(infoLog);
  }
}

//#define rglAssert(...)
rglShader_t * rglCreateShader(const char * vsrc, const char * fsrc)
{
  GLhandleARB vs, fs, prog;

  //printf("Compiling shader :\n%s", fsrc);
  
  vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  rglAssert(glGetError() == GL_NO_ERROR);
	fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  rglAssert(glGetError() == GL_NO_ERROR);

	glShaderSourceARB(vs, 1, &vsrc,NULL);
  rglAssert(glGetError() == GL_NO_ERROR);
	glShaderSourceARB(fs, 1, &fsrc,NULL);
  rglAssert(glGetError() == GL_NO_ERROR);
	glCompileShaderARB(vs);
  rglAssert(glGetError() == GL_NO_ERROR);
	glCompileShaderARB(fs);
  rglAssert(glGetError() == GL_NO_ERROR);
  printInfoLog(vs, vsrc);
  printInfoLog(fs, fsrc);
	prog = glCreateProgramObjectARB();
	glAttachObjectARB(prog, fs);
  rglAssert(glGetError() == GL_NO_ERROR);
	glAttachObjectARB(prog, vs);
  rglAssert(glGetError() == GL_NO_ERROR);

	glLinkProgramARB(prog);
  rglAssert(glGetError() == GL_NO_ERROR);

  rglShader_t * s = (rglShader_t *) malloc(sizeof(rglShader_t));
  s->vs = vs;
  s->fs = fs;
  s->prog = prog;
  //LOG("Creating shader %d %d %d\n", s->vs, s->fs, s->prog);
#ifdef RDP_DEBUG
  s->vsrc = strdup(vsrc);
  s->fsrc = strdup(fsrc);
#endif
  return s;
}

void rglUseShader(rglShader_t * shader)
{
  if (!shader)
    glUseProgramObjectARB(0);
  else
    glUseProgramObjectARB(shader->prog);
}

void rglDeleteShader(rglShader_t * shader)
{
  //LOG("Deleting shader %d %d %d\n", shader->vs, shader->fs, shader->prog);
  glDeleteObjectARB(shader->prog);
  rglAssert(glGetError() == GL_NO_ERROR);
  glDeleteObjectARB(shader->vs);
  rglAssert(glGetError() == GL_NO_ERROR);
  glDeleteObjectARB(shader->fs);
  rglAssert(glGetError() == GL_NO_ERROR);
  free(shader);
}
