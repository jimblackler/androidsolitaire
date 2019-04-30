#include "Renderer.h"

#include "GlUtils.h"

#include <EGL/egl.h>
#include <cstdlib>

#include "glm/glm.hpp"
#include "Textures.h"

auto gVertexShader = R"(#version 300 es

  // Input vertex data, different for all executions of this shader.
  layout(location = 0) in vec3 vertexPosition_modelspace;
  layout(location = 1) in vec2 vertexUV;

  // Output data ; will be interpolated for each fragment.
  out vec2 UV;

  // Values that stay constant for the whole mesh.
  uniform mat4 MVP;

  void main(){

      // Output position of the vertex, in clip space : MVP * position
      gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

      // UV of the vertex. No special space for this one.
      UV = vertexUV;
  }
)";

auto gFragmentShader = R"(#version 300 es

  // Interpolated values from the vertex shaders
  in highp vec2 UV;

  // Output data
  out highp vec4 color;

  // Values that stay constant for the whole mesh.
  uniform sampler2D textureSampler;

  void main(){

      // Output color = color of the texture at the specified UV
      color = texture(textureSampler, UV).rgba;
  }
)";

static const GLfloat gVertexBufferData[] = {
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
};

static const GLfloat gUvBufferData[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

class LocalRenderer : public Renderer {

  const EGLContext eglContext;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  GLuint program;
  GLuint texture;
  GLint matrixId;
  GLint textureSamplerId;
  jobject glService;
  JNIEnv *env;

  void resize(int w, int h) override {
    glViewport(0, 0, w, h);
  }

  void render(int counter) override {
    glClearColor(.3, .3, .3, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glm::mat4 mvp = glm::mat4(1.0f);
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);

    if (true) {
      glBindTexture(GL_TEXTURE_2D, texture);
      jclass glServiceType = env->GetObjectClass(glService);
      jmethodID methodId = env->GetMethodID(glServiceType, "loadTexture", "(Ljava/lang/String;)V");
      char *outString;
      asprintf(&outString, "#%d", counter);
      jstring string = env->NewStringUTF(outString);
      env->CallVoidMethod(glService, methodId, string);
      free(outString);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureSamplerId, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (false) {
      size_t size = 1000000;
      void *data = malloc(size);
      if (data) {
        char *ptr = (char *) data;
        for (int count = 0; count < size; count++) {
          *ptr++ = 0x7f;
        }
      } else {
        __android_log_write(ANDROID_LOG_INFO, "Renderer", "failed");
      }
    }
  }


  bool init(JNIEnv *env, jobject glService, const jint *data, jint width, jint height) override {
    this->env = env;
    this->glService = env->NewGlobalRef(glService);
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData, GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gUvBufferData), gUvBufferData, GL_STATIC_DRAW);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (true) {
      auto data2 = GenerateTexture(width, height);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    program = createProgram(gVertexShader, gFragmentShader);
    if (!program) {
      return false;
    }

    matrixId = glGetUniformLocation(program, "MVP");
    textureSamplerId = glGetUniformLocation(program, "textureSampler");

    return true;
  }

public:

  LocalRenderer() : eglContext(eglGetCurrentContext()) {

  }

  ~LocalRenderer() override {
    if (eglGetCurrentContext() != eglContext) {
      return;
    }

    if (glService) {
      env->DeleteGlobalRef(glService);
    }
  }
};

Renderer *newRenderer() {
  return new LocalRenderer();
}
