#include <malloc.h>
#include "GlUtils.h"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "GlUtils", __VA_ARGS__)

bool checkGlError(const char *funcName) {
  GLint err = glGetError();
  if (err != GL_NO_ERROR) {
    ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
    return true;
  }
  return false;
}

static GLuint createShader(GLenum shaderType, const char *src) {
  GLuint shader = glCreateShader(shaderType);
  if (!shader) {
    checkGlError("glCreateShader");
    return 0;
  }
  glShaderSource(shader, 1, &src, nullptr);

  GLint compiled = GL_FALSE;
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled) {
    return shader;
  }

  GLint infoLogLength = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0) {
    auto infoLog = (GLchar *) malloc((size_t) infoLogLength);
    if (infoLog) {
      glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
      ALOGE("Could not compile %s shader:\n%s\n",
            shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
            infoLog);
      free(infoLog);
    }
  }
  glDeleteShader(shader);
  return 0;
}

GLuint createProgram(const char *vtxSrc, const char *fragSrc) {
  GLuint program = 0;

  GLuint vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc);
  if (vertexShader) {
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (fragmentShader) {
      program = glCreateProgram();
      if (program) {
        GLint linked = GL_FALSE;
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
          ALOGE("Could not link program");
          GLint infoLogLen = 0;
          glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
          if (infoLogLen) {
            auto infoLog = (GLchar *) malloc((size_t) infoLogLen);
            if (infoLog) {
              glGetProgramInfoLog(program, infoLogLen, nullptr, infoLog);
              ALOGE("Could not link program:\n%s\n", infoLog);
              free(infoLog);
            }
          }
          glDeleteProgram(program);
          program = 0;
        }
      } else {
        checkGlError("glCreateProgram");
      }
      glDeleteShader(fragmentShader);
    }
    glDeleteShader(vertexShader);
  }
  return program;
}
