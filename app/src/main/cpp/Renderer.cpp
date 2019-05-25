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

const int TARGET_WIDTH = 864;

const int BLANK_ROW = 4;
const int CARDBACK_COLUMN = 0;
const int PLACEHOLDER_COLUMN = 1;

const int GEARS_LEFT = 112;
const int GEARS_TOP = 717;
const int GEARS_WIDTH = 38;
const int GEARS_HEIGHT = 38;
const int GEARS_X = 796;
const int GEARS_Y = 28;

struct PlaceHolder {
  Sprite *sprite;
  std::function<void()> onClick;
};

class LocalRenderer : public Renderer {
public:
  explicit LocalRenderer(android_app *app) {
    this->app = app;
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

    targetHeight = TARGET_WIDTH * (float) height / width;

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    char *data = load("sprites3348x2250.rgba", false, app->activity->assetManager);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3348, 2250, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    delete data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    program = loadProgram("vertex.vsh", "fragment.fsh",
                          app->activity->assetManager);

    matrixId = glGetUniformLocation(program, "MVP");
    textureSamplerId = glGetUniformLocation(program, "textureSampler");

    cardSprites.resize(NUMBER_CARDS);
    for (int cardNumber = 0; cardNumber < NUMBER_CARDS; cardNumber++) {
      order.push_back(cardNumber);
      cardSprites[cardNumber] = newSprite(CARD_WIDTH, CARD_HEIGHT);
    }

    gearsSprite = newSprite(GEARS_WIDTH, GEARS_HEIGHT);
    gearsSprite->setPosition({GEARS_X, GEARS_Y, 0});
    gearsSprite->setUVs((float) GEARS_LEFT / TEXTURE_WIDTH,
                        (float) (GEARS_LEFT + GEARS_WIDTH) / TEXTURE_WIDTH,
                        (float) GEARS_TOP / TEXTURE_HEIGHT,
                        (float) (GEARS_TOP + GEARS_HEIGHT) / TEXTURE_HEIGHT);
  }

private:
  std::vector<Sprite *> cardSprites;
  std::list<int> order;
  std::set<int> draggable;
  float previousX;
  float previousY;
  bool click;
  std::list<int> draggingCards;
  std::list<PlaceHolder> placeHolders;
  Sprite *gearsSprite;

  GLuint program;
  GLuint texture;
  GLint matrixId;
  GLint textureSamplerId;
  EGLDisplay display = nullptr;
  EGLSurface surface;
  EGLContext context;
  EGLint width;
  EGLint height;
  float targetHeight;

  DragHandler *dragHandler;

  android_app *app;

  ~LocalRenderer() {
    delete gearsSprite;

    for (Sprite *sprite: cardSprites) {
      delete sprite;
    }

    for (PlaceHolder &placeHolder: placeHolders) {
      delete placeHolder.sprite;
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
  }

  void drawFrame() override {
    if (!display) {
      return;
    }

    glClearColor(0.25F, 0.75F, 0.25F, 1.0F);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glm::mat4 mvp = glm::mat4();
    mvp = glm::scale(mvp, {1, -1, 1});
    mvp = glm::translate(mvp, {-1, -1, 0});
    mvp = glm::scale(mvp, {2, 2, 1});
    float targetHeight = TARGET_WIDTH * (float) height / width;
    mvp = glm::scale(mvp, {1.0F / TARGET_WIDTH, 1.0F / targetHeight, 1});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureSamplerId, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gearsSprite->draw(mvp, matrixId);

    for (PlaceHolder &placeHolder: placeHolders) {
      placeHolder.sprite->draw(mvp, matrixId);
    }

    for (int pass = 0; pass < 2; pass++) {
      for (int cardNumber : order) {
        const Sprite *sprite = cardSprites[cardNumber];
        if ((pass == 0) != (sprite->getPosition().z == 0)) {
          continue;
        }

        sprite->draw(mvp, matrixId);
      }
    }
    eglSwapBuffers(display, surface);
  }

  void placeHolder(const int x, const int y, std::function<void()> onClick)
  override {
    Sprite *sprite = newSprite(CARD_WIDTH, CARD_HEIGHT);
    sprite->setPosition({x, y, 0});
    _setBy(sprite, BLANK_ROW, PLACEHOLDER_COLUMN);
    placeHolders.push_back(PlaceHolder{sprite, onClick});
  }

  void _setBy(int cardNumber, int suit, int type) {
    _setBy(cardSprites[cardNumber], suit, type);
  }

  void _setBy(Sprite *sprite, int suit, int type) {
    float left = ((float) CARD_WIDTH * type) / TEXTURE_WIDTH;
    float right = ((float) CARD_WIDTH * type + CARD_WIDTH) / TEXTURE_WIDTH;
    float top = ((float) CARD_HEIGHT * suit) / TEXTURE_HEIGHT;
    float bottom = ((float) CARD_HEIGHT * suit + CARD_HEIGHT) / TEXTURE_HEIGHT;
    sprite->setUVs(left, right, top, bottom);
  }

  void faceDown(int cardNumber) override {
    _setBy(cardNumber, BLANK_ROW, CARDBACK_COLUMN);
  }

  void faceUp(int cardNumber) override {
    int suit = Rules::getSuit(cardNumber);
    int type = Rules::getType(cardNumber);
    _setBy(cardNumber, suit, type);
  }

  void positionCard(int cardNumber, float x, float y, float z) override {
    Sprite *sprite = cardSprites[cardNumber];
    sprite->setPosition({x, y, z});
  }

  void setDraggable(int cardNumber, bool draggable) override {
    if (draggable) {
      this->draggable.insert(cardNumber);
    } else {
      this->draggable.erase(cardNumber);
    }
  }

  void raiseCard(int cardNumber) override {
    order.remove(cardNumber);
    order.push_back(cardNumber);
  }

  const glm::vec3 &getCardPosition(int cardNumber) const override {
    Sprite *sprite = cardSprites[cardNumber];
    return sprite->getPosition();
  }

  void setDragHandler(DragHandler *dragHandler) override {
    this->dragHandler = dragHandler;
  }

  void _pointerDown(float x, float y) {
    for (auto it = order.rbegin(); it != order.rend(); it++) {
      int cardNumber = *it;
      const Sprite *sprite = cardSprites[cardNumber];
      if (!sprite->hits(x, y)) {
        continue;
      }

      if (this->draggable.count(cardNumber)) {
        std::list<int> cards = dragHandler->startDrag(cardNumber);
        javaCall("vibrate");
        click = true;
        this->draggingCards = cards;
        for (int draggingCard : draggingCards) {
          this->raiseCard(draggingCard);
        }
        return;
      }
    }
    for (const PlaceHolder &placeHolder: placeHolders) {
      if (!placeHolder.onClick) {
        continue;
      }
      if (!placeHolder.sprite->hits(x, y)) {
        continue;
      }
      javaCall("vibrate");
      placeHolder.onClick();
      return;
    }
    if (gearsSprite->hits(x, y, 50)) {
      javaCall("vibrate");
      javaCall("gearsPressed");
      return;
    }
  }

  void motionEvent(int type, float x, float y) override {
    x = x / width * TARGET_WIDTH;
    y = y / height * targetHeight;

    switch (type) {
      case AMOTION_EVENT_ACTION_DOWN:
        _pointerDown(x, y);
        break;
      case AMOTION_EVENT_ACTION_MOVE:
        click = false;
        for (int cardNumber : draggingCards) {
          Sprite *sprite = cardSprites[cardNumber];
          auto position = sprite->getPosition();
          sprite->setPosition({position.x + x - previousX,
                               position.y + y - previousY,
                               position.z});
        }
        break;
      case AMOTION_EVENT_ACTION_UP:
        if (!draggingCards.empty()) {
          int firstCard = draggingCards.front();
          dragHandler->cardClickedOrDropped(firstCard, click);
          draggingCards.clear();
        }
        break;
      default:
        break;
    }

    previousX = x;
    previousY = y;
  }

  void javaCall(const char *method) const {
    JNIEnv *jni;
    app->activity->vm->AttachCurrentThread(&jni, nullptr);
    jclass clazz = jni->GetObjectClass(app->activity->clazz);
    jmethodID methodID = jni->GetMethodID(clazz, method, "()V");
    jni->CallVoidMethod(app->activity->clazz, methodID);
    app->activity->vm->DetachCurrentThread();
  }

  float getTargetHeight() const override {
    return targetHeight;
  }
};

Renderer *newRenderer(android_app *app) {
  return new LocalRenderer(app);
}


