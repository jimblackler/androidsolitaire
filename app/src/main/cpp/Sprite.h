#pragma once

class Sprite {
public:
  virtual ~Sprite() = default;
  virtual void setUVs(float left, float right, float top, float bottom) = 0;
  virtual void draw() = 0;
};

extern Sprite *newSprite();