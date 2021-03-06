#pragma once

#include <android_native_app_glue.h>
#include <vector>
#include <list>
#include "glm/vec3.hpp"

class DragHandler;

class GameController;

class Renderer {
public:
  virtual ~Renderer() = default;
  virtual void drawFrame() = 0;

  virtual void faceDown(int card) = 0;
  virtual void faceUp(int card) = 0;
  virtual void positionCard(int cardNumber, float x, float y, float z) = 0;
  virtual void raiseCard(int cardNumber) = 0;
  virtual const glm::vec3 &getCardPosition(int cardNumber) const = 0;
  virtual void setDragHandler(DragHandler *dragHandler) = 0;
  virtual void placeHolder(const int x, const int y, std::function<void()>) = 0;
  virtual void motionEvent(int type, float x, float y) = 0;
  virtual void setDraggable(int cardNumber, bool draggable) = 0;
  virtual float getTargetHeight() const = 0;
};

extern Renderer *newRenderer(android_app *app);