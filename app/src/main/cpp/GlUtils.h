#include <GLES3/gl32.h>
#include <android/log.h>

#define LOG_TAG "GLES3JNI"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// returns true if a GL error occurred
extern bool checkGlError(const char *funcName);
extern GLuint createProgram(const char *vtxSrc, const char *fragSrc);
