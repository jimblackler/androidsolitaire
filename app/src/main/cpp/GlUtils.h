#include <GLES3/gl32.h>
#include <android/log.h>

extern bool checkGlError(const char *funcName);
extern GLuint createProgram(const char *vtxSrc, const char *fragSrc);
