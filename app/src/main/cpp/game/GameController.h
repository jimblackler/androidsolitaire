#include "DragHandler.h"

class GameState;

class Renderer;

class GameController : public DragHandler {
public:
  virtual ~GameController() = default;
  virtual void render() = 0;
  virtual void animate() = 0;
};

extern GameController *newGameController(Renderer *renderer,
                                         GameState *gameState);