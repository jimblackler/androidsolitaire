#include <memory>
#include <EGL/egl.h>
#include "Engine.h"
#include <jni.h>
#include <string>

Engine* gEngine;

static int32_t handleInput(struct android_app *app, AInputEvent *event) {
  auto engine = (struct Engine *) app->userData;
  return engine->handleInput(event);
}

static void handleCmd(struct android_app *app, int32_t cmd) {
  auto engine = (struct Engine *) app->userData;
  engine->handleCmd(cmd);
}

void android_main(struct android_app *app) {
  gEngine = newEngine(app);
  app->userData = gEngine;
  app->onAppCmd = handleCmd;
  app->onInputEvent = handleInput;

  gEngine->mainLoop();

  delete gEngine;
}

std::string fromJstring(JNIEnv *env, jstring jStr){
  auto chars = env->GetStringUTFChars(jStr, nullptr);
  std::string str(chars);
  env->ReleaseStringUTFChars(jStr, chars);
  return str;
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_solitaire_MainActivity_action(JNIEnv *env,
                                                    jobject instance,
                                                    jstring action) {

  gEngine->action(fromJstring(env, action));
}
