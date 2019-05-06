#include <android_native_app_glue.h>

class Engine {
public:
  virtual int32_t handleInput(AInputEvent *pEvent) = 0;
  virtual void handleCmd(int32_t cmd) = 0;
  virtual void mainLoop() = 0;
};

extern struct Engine *NewEngine(android_app *app);
