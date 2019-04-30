#include "Controller.h"
#include "Renderer.h"

class LocalController : public Controller {

  int timeCount;

  void process(Renderer *renderer) override {
    renderer->render(timeCount);
    timeCount++;
  }

public:

  LocalController() {
    timeCount = 0;
  }

  ~LocalController() override = default;
};

Controller *newController() {
  return new LocalController();
}
