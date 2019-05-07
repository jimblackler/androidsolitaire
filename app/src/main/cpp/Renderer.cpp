#include "Renderer.h"

#include "game/GameState.h"

class LocalRenderer : public Renderer {
  void drawFrame() override {};
};

Renderer *NewRenderer() {
  return new LocalRenderer();
}