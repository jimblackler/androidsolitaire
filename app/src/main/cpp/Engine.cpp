#include "Engine.h"

#include "Renderer.h"
#include "game/GameState.h"
#include "game/GameController.h"
#include <android/sensor.h>
#include <memory>


#define ALOGI(...) __android_log_print(ANDROID_LOG_ERROR, "Engine", __VA_ARGS__)

struct State {
  bool placeholder;
};


class LocalEngine : public Engine {

public:
  explicit LocalEngine(android_app *app) {
    this->app = app;
    sensorManager = ASensorManager_getInstance();
    if (app->savedState) {
      state = *(struct State *) app->savedState;  // TODO ... gameState into savedState
    }
    this->renderer = newRenderer(app);
    this->gameState = newGameState();
    this->gameState->newGame();
  }

private:
  struct android_app *app;
  Renderer *renderer;
  GameController *controller;
  GameState *gameState;
  bool active;
  ASensorManager *sensorManager;
  State state;

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
          renderer->initDisplay();
          controller = newGameController(renderer, this->gameState);
          renderer->setDragHandler(this->controller);
          controller->render();
          controller->animate();  // TODO ... required to do here?
          renderer->drawFrame();  // TODO ... required to do here?
        }
        break;
      case APP_CMD_TERM_WINDOW:
        renderer->closeDisplay();
        delete this->controller;
        this->controller = nullptr;
        break;
      case APP_CMD_GAINED_FOCUS:
        active = true;
        break;
      case APP_CMD_LOST_FOCUS:
        active = false;
        controller->animate();  // TODO ... required to do here?
        renderer->drawFrame();  // TODO ... required to do here?
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
          renderer->closeDisplay();
          return;
        }
      }
      if (active) {
        controller->animate();
        renderer->drawFrame();
      }
    }
  }
};

struct Engine *NewEngine(android_app *app) {
  return new LocalEngine(app);
}
