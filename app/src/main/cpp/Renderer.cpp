#include "Renderer.h"

#include "GlUtils.h"
#include "FileUtils.h"
#include "game/GameController.h"
#include "game/GameState.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <android_native_app_glue.h>
#include <cassert>

#include "Sprite.h"


const int TEXTURE_WIDTH = 1339;
const int TEXTURE_HEIGHT = 900;

const int CARD_WIDTH = 103;
const int CARD_HEIGHT = 143;

const int TARGET_WIDTH = 860;


class LocalRenderer : public Renderer {
public:
  explicit LocalRenderer(android_app *app) {
    this->app = app;
  }

private:
  std::vector<Sprite*> sprites;

  GLuint program;
  GLuint texture;
  GLint matrixId;
  GLint textureSamplerId;

  EGLDisplay display = nullptr;
  EGLSurface surface;
  EGLContext context;

  EGLint width;
  EGLint height;

  DragHandler *dragHandler;

  android_app *app;


  ~LocalRenderer() = default;

  void initDisplay() override {
    assert(!display);

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
    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3,
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

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    char *data = load("cards103x143.rgba", false, app->activity->assetManager);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    delete data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    program = loadProgram("vertex.vsh", "fragment.fsh",
                          app->activity->assetManager);

    matrixId = glGetUniformLocation(program, "MVP");
    textureSamplerId = glGetUniformLocation(program, "textureSampler");

    for (int cardNumber = 0; cardNumber < NUMBER_CARDS; cardNumber++) {
      sprites[cardNumber] = newSprite();
    }

  }

  void drawFrame() override {
    if (!display) {
      return;
    }

    glClearColor(0.25F, 0.75F, 0.25F, 1.0F);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glm::mat4 mvp = glm::mat4();
    mvp = glm::scale(mvp, glm::vec3(1, -1, 1));
    mvp = glm::translate(mvp, glm::vec3(-1, -1, 0));
    mvp = glm::scale(mvp, glm::vec3(2, 2, 1));
    mvp = glm::scale(mvp, glm::vec3(1.0F / TARGET_WIDTH,
                                    1.0F / TARGET_WIDTH * (float) width / height,
                                    1));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureSamplerId, 0);

    glm::mat4 mvp2 = glm::translate(mvp, glm::vec3(CARD_WIDTH, CARD_HEIGHT, 0));
    mvp2 = glm::scale(mvp2, glm::vec3(CARD_WIDTH, CARD_HEIGHT, 1));
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp2[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int cardNumber = 0; cardNumber < NUMBER_CARDS; cardNumber++) {
      sprites[cardNumber]->draw();
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    eglSwapBuffers(display, surface);
  }

  void closeDisplay() override {

    for (int cardNumber = 0; cardNumber < NUMBER_CARDS; cardNumber++) {
      delete sprites[cardNumber];
    }

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
  }


  void placeHolder(const int x, const int y, void (*onClick)()) override {

  }

  void _setBy(int cardNumber, int suit, int type) {
    Sprite* sprite = sprites[cardNumber];

    float left = ((float) CARD_WIDTH * type) / TEXTURE_WIDTH;
    float right = ((float) CARD_WIDTH * type + CARD_WIDTH) / TEXTURE_WIDTH;
    float top = ((float) CARD_HEIGHT * suit) / TEXTURE_HEIGHT;
    float bottom = ((float) CARD_HEIGHT * suit + CARD_HEIGHT) / TEXTURE_HEIGHT;

    sprite->setUVs(left, right, top, bottom);
  }

  void faceDown(int cardNumber) override {
    _setBy(cardNumber, 4, 0);
  }

  void faceUp(int cardNumber) override {
    int suit = Rules::getSuit(cardNumber);
    int type = Rules::getType(cardNumber);
    _setBy(cardNumber, suit, type);
  }

  void positionCard(int cardNumber, float x, float y, float z) override {}

  void raiseCard(int cardNumber) override {}

  std::vector<float> getCardPosition(int cardNumber) override {
    return std::vector<float>();
  }

  void setDragHandler(DragHandler *dragHandler) override {
    this->dragHandler = dragHandler;
  }

};

Renderer *newRenderer(android_app *app) {
  return new LocalRenderer(app);
}

