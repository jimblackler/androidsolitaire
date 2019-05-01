#include "GlUtils.h"

#include <jni.h>
#include <ctime>
#include <EGL/egl.h>
#include <string.h>

#include "Controller.h"
#include "Renderer.h"

static Controller *gController = nullptr;
static Renderer *gRenderer = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_jimblackler_net_solitaire_MainLib_init(JNIEnv *env, jclass type, jobject glService) {
  if (gRenderer) {
    delete gRenderer;
    gRenderer = nullptr;
  }

  auto versionStr = (const char *) glGetString(GL_VERSION);
  if (strstr(versionStr, "OpenGL ES 3.")) {
    gRenderer = newRenderer();
    gRenderer->init(env, glService);
  } else {
    ALOGE("Unsupported OpenGL ES version");
  }

  if (!gController) {
    gController = newController();
  }

}

extern "C"
JNIEXPORT void JNICALL
Java_jimblackler_net_solitaire_MainLib_resize(JNIEnv *env, jclass type, jint width, jint height) {
  if (gRenderer) {
    gRenderer->resize(width, height);
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_jimblackler_net_solitaire_MainLib_step(JNIEnv *env, jclass typej) {
  if (gRenderer && gController) {
    gController->process(gRenderer);
  }
}
