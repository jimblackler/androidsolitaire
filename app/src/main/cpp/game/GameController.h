#pragma once // TODO ... review

#include "DragHandler.h"

class GameState;

class Renderer;

class GameController : public DragHandler {
public:
  virtual void render() = 0;
  virtual void placeHolder(const int x, const int y, void (*onClick)()) = 0;
};

extern GameController *newGameController(Renderer *renderer,
                                         GameState *gameState);