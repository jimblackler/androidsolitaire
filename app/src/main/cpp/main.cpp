#include <memory>
#include <EGL/egl.h>
#include "Engine.h"

static int32_t handleInput(struct android_app *app, AInputEvent *event) {
  auto engine = (struct Engine *) app->userData;
  return engine->handleInput(event);
}

static void handleCmd(struct android_app *app, int32_t cmd) {
  auto engine = (struct Engine *) app->userData;
  engine->handleCmd(cmd);
}

void android_main(struct android_app *app) {
  auto engine = NewEngine(app);
  app->userData = engine;
  app->onAppCmd = handleCmd;
  app->onInputEvent = handleInput;

  engine->mainLoop();
}

