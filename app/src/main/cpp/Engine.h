#include <android_native_app_glue.h>
#include <string>

class Engine {
public:
  virtual ~Engine() = default;
  virtual int32_t handleInput(AInputEvent *pEvent) = 0;
  virtual void handleCmd(int32_t cmd) = 0;
  virtual void mainLoop() = 0;
  virtual void action(const std::string &action) = 0;
};

extern struct Engine *newEngine(android_app *app);
