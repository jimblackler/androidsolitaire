#pragma once

#include "glm/detail/type_mat.hpp"
#include "glm/vec3.hpp"

class Sprite {
public:
  virtual ~Sprite() = default;
  virtual void setUVs(float left, float right, float top, float bottom) = 0;
  virtual void setPosition(const glm::vec3 &position) = 0;
  virtual const glm::vec3 &getPosition() const = 0;
  virtual void draw(const glm::mat4 &mvp, int matrixId) const = 0;
  virtual bool hits(float x, float y, float margin=0) const = 0;
};

extern Sprite *newSprite(float width, float height);