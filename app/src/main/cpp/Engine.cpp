#include "Engine.h"

#include "Renderer.h"
#include "game/GameState.h"
#include "game/GameController.h"
#include <android/sensor.h>
#include <deque>
#include <memory>

#define ALOGI(...) __android_log_print(ANDROID_LOG_ERROR, "Engine", __VA_ARGS__)

class LocalEngine : public Engine {

public:
  explicit LocalEngine(android_app *app) {
    this->app = app;
    gameState = newGameState();
    if (app->savedState) {
      gameState->deserialize(app->savedState);
    } else {
      gameState->newGame();
      // TODO: do on a delay to animate
      gameState->execute({MOVE_TYPE::DRAW, 0, 0});
    }

    active = false;
  }

private:
  struct android_app *app = nullptr;
  Renderer *renderer = nullptr;
  GameController *controller = nullptr;;
  GameState *gameState = nullptr;
  std::deque<std::string> actions;
  bool active;

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
          controller = newGameController(renderer, gameState);
          renderer->setDragHandler(controller);
          controller->render(); // Twice to stop animations.
          controller->render();
        }
        break;
      case APP_CMD_TERM_WINDOW:
        delete renderer;
        renderer = nullptr;
        delete controller;
        controller = nullptr;
        active = false;
        break;
      case APP_CMD_GAINED_FOCUS:
        active = true;
        break;
      case APP_CMD_LOST_FOCUS:
        active = false;
        break;
      case APP_CMD_SAVE_STATE:
        app->savedState = gameState->serialize(&app->savedStateSize);
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

  void processActions() {
    if (actions.empty()) {
      return;
    }
    for (const std::string &action : actions) {
      if (action == "newGame") {
        gameState->newGame();
        controller->render();
        controller->draw();
      }
    }
    actions.clear();
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
        processActions();
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

  void action(const std::string &action) override {
    actions.push_front(action);
  }
};

struct Engine *newEngine(android_app *app) {
  return new LocalEngine(app);
}
