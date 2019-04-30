#include <jni.h>

class Renderer;

class Controller {
public:
  virtual ~Controller() = default;
  virtual void process(Renderer *pRenderer) = 0;
};

Controller *newController();
