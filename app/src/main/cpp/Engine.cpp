#include "Engine.h"

#include "GlUtils.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "FileUtils.h"
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/sensor.h>
#include <memory>
#include <atomic>

#define ALOGI(...) __android_log_print(ANDROID_LOG_ERROR, "Engine", __VA_ARGS__)

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

struct State {
  bool placeholder;
};

class LocalEngine : public Engine {

public:
  explicit LocalEngine(android_app *app) {
    this->app = app;
    sensorManager = ASensorManager_getInstance();
    if (app->savedState) {
      state = *(struct State *) app->savedState;
    }
  }

private:
  struct android_app *app;

  bool active;
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;

  EGLint width;
  EGLint height;

  ASensorManager *sensorManager;

  GLuint vertexBuffer;
  GLuint uvBuffer;
  GLuint program;
  GLuint texture;
  GLint matrixId;
  GLint textureSamplerId;

  State state;

  void initDisplay() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    const EGLint attribList[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                 EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                 EGL_BLUE_SIZE, 8,
                                 EGL_GREEN_SIZE, 8,
                                 EGL_RED_SIZE, 8,
                                 EGL_NONE};
    EGLint configs;
    EGLConfig config;
    eglChooseConfig(display, attribList, &config, 1, &configs);
    if (!configs) {
      return;
    }

    surface = eglCreateWindowSurface(display, config, app->window, nullptr);
    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2,
                                     EGL_NONE};
    context = eglCreateContext(display, config, nullptr, contextAttribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
      return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);

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

    char *data = load("cards103x143.rgba", app->activity->assetManager);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1339, 900, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    delete data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    program = loadProgram("vertex.vsh", "fragment.fsh", app->activity->assetManager);

    matrixId = glGetUniformLocation(program, "MVP");
    textureSamplerId = glGetUniformLocation(program, "textureSampler");
  }

  void drawFrame() {
    if (!display) {
      return;
    }

    glClearColor(0.25F, 0.75F, 0.25F, 1.0F);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glm::mat4 mvp = glm::mat4();
    mvp = glm::scale(mvp, glm::vec3(0.5F, 0.5F, 0));
    mvp = glm::translate(mvp, glm::vec3(0.5F, 0.5F, 0));
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureSamplerId, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    eglSwapBuffers(display, surface);
  }

  void closeDisplay() {
    if (display != EGL_NO_DISPLAY) {
      eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
      if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display, context);
      }
      if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
      }
      eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    active = false;
  }

  int32_t handleInput(AInputEvent *event) override {
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) {
      return 0;
    }
    auto x = (int32_t) AMotionEvent_getX(event, 0);
    auto y = (int32_t) AMotionEvent_getY(event, 0);
    return 1;
  }

  void handleCmd(int32_t cmd) override {
    switch (cmd) {
      case APP_CMD_INIT_WINDOW:
        if (app->window) {
          initDisplay();
          drawFrame();
        }
        break;
      case APP_CMD_TERM_WINDOW:
        closeDisplay();
        break;
      case APP_CMD_GAINED_FOCUS:
        active = true;
        break;
      case APP_CMD_LOST_FOCUS:
        active = false;
        drawFrame();
        break;
      case APP_CMD_SAVE_STATE:
        app->savedStateSize = sizeof(struct State);
        app->savedState = malloc(app->savedStateSize);
        *((struct State *) app->savedState) = state;
        break;
      case APP_CMD_INPUT_CHANGED:
      case APP_CMD_WINDOW_RESIZED:
      case APP_CMD_WINDOW_REDRAW_NEEDED:
      case APP_CMD_CONTENT_RECT_CHANGED:
      case APP_CMD_CONFIG_CHANGED:
      case APP_CMD_LOW_MEMORY:
      case APP_CMD_START:
      case APP_CMD_RESUME:
      case APP_CMD_PAUSE:
      case APP_CMD_STOP:
      case APP_CMD_DESTROY:
      default:
        break;
    }
  }

  void mainLoop() override {
    while (true) {
      while (true) {
        struct android_poll_source *source;
        int events;
        int id = ALooper_pollAll(active ? 0 : -1,
                                 nullptr, &events, (void **) &source);
        if (id <= 0) {
          break;
        }
        if (source) {
          source->process(app, source);
        }
        if (app->destroyRequested) {
          closeDisplay();
          return;
        }
      }
      if (active) {
        // <-- do process here
        drawFrame();
      }
    }
  }
};

struct Engine *NewEngine(android_app *app) {
  return new LocalEngine(app);
}
