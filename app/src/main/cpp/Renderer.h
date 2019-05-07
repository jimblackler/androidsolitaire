#include <android_native_app_glue.h>

class Renderer {
public:
  virtual void initDisplay() = 0;
  virtual void drawFrame() = 0;
  virtual void closeDisplay() = 0;
};

extern Renderer * NewRenderer(android_app *app);