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
    if (app->savedState) {
      state = *(struct State *) app->savedState;  // TODO ... gameState into savedState
    }
    this->gameState = newGameState();
    this->gameState->newGame();
    active = false;
  }

private:
  struct android_app *app = nullptr;
  Renderer *renderer = nullptr;
  GameController *controller = nullptr;;
  GameState *gameState = nullptr;
  bool active;
  State state;

  int32_t handleInput(AInputEvent *event) override {
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) {
      return 0;
    }

    auto action = AMotionEvent_getAction(event);
    auto flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
      case AMOTION_EVENT_ACTION_MOVE:
      case AMOTION_EVENT_ACTION_DOWN:
      case AMOTION_EVENT_ACTION_UP:
        renderer->motionEvent(flags, AMotionEvent_getX(event, 0),
                              AMotionEvent_getY(event, 0));
        break;
      default:
        break;
    }

    return 1;
  }

  void handleCmd(int32_t cmd) override {
    switch (cmd) {
      case APP_CMD_INIT_WINDOW:
        if (app->window) {
          assert(!renderer);
          renderer = newRenderer(app);
          controller = newGameController(renderer, this->gameState);
          renderer->setDragHandler(this->controller);
          controller->render();
          controller->render();
        }
        break;
      case APP_CMD_TERM_WINDOW:
        delete renderer;
        renderer = nullptr;
        delete this->controller;
        this->controller = nullptr;
        active = false;
        break;
      case APP_CMD_GAINED_FOCUS:
        active = true;
        break;
      case APP_CMD_LOST_FOCUS:
        active = false;
        break;
      case APP_CMD_SAVE_STATE:
        app->savedStateSize = sizeof(struct State);
        app->savedState = malloc(app->savedStateSize);
        *((struct State *) app->savedState) = state;
        break;
      case APP_CMD_DESTROY:
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

      default:
        break;
    }
  }

  void mainLoop() override {
    while (true) {
      while (true) {
        struct android_poll_source *source = nullptr;
        int events = 0;
        int id = ALooper_pollAll(active ? 0 : -1,
                                 nullptr, &events, (void **) &source);
        if (id < 0 && !source) {
          break;
        }
        source->process(app, source);
        if (app->destroyRequested) {
          delete renderer;
          renderer = nullptr;
          return;
        }
      }
      if (active && controller) {
        controller->animate();
        renderer->drawFrame();
      }
    }
  }
};

struct Engine *NewEngine(android_app *app) {
  return new LocalEngine(app);
}
