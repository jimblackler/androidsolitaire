#include "Renderer.h"

#include "GlUtils.h"

#include <EGL/egl.h>

#include <malloc.h>

#include "glm/glm.hpp"

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
    -1.0F, 1.0F, 0.0F,
    1.0F, 1.0F, 0.0F,
    -1.0F, -1.0F, 0.0F,
    1.0F, -1.0F, 0.0F,
};

static const GLfloat gUvBufferData[] = {
    0.0F, 0.0F,
    1.0F, 0.0F,
    0.0F, 1.0F,
    1.0F, 1.0F
};

class LocalRenderer : public Renderer {

  EGLContext eglContext;
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
    glClearColor(0.25F, 0.75F, 0.25F, 1.0F);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glm::mat4 mvp = glm::mat4(1.0F);
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);

    glBindTexture(GL_TEXTURE_2D, texture);
    jclass glServiceType = env->GetObjectClass(glService);
    jmethodID methodId = env->GetMethodID(glServiceType, "loadTexture", "()V");
    char *outString;
    asprintf(&outString, "#%d", counter);
    jstring string = env->NewStringUTF(outString);
    env->CallVoidMethod(glService, methodId, string);
    free(outString);

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
  }

  bool init(JNIEnv *env, jobject glService) override {
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
