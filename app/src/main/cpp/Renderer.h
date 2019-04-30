#include <jni.h>

class Renderer {
public:
  virtual ~Renderer() = default;

  virtual void resize(int w, int h) = 0;
  virtual void render(int count) = 0;
  virtual bool init(JNIEnv *env, jobject glService, const jint *data, jint width, jint height) = 0;
};

Renderer *newRenderer();
