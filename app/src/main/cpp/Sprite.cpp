#include <GLES/gl.h>
#include <GLES3/gl32.h>
#include "Sprite.h"
#include "glm/detail/type_mat.hpp"
#include "glm/gtx/transform.hpp"

static const GLfloat gVertexBufferData[] = {
    0, 0, 0,
    1.0F, 0, 0,
    0, 1.0F, 0,
    1.0F, 1.0F, 0,
};

class LocalSprite : public Sprite {
public:
  LocalSprite(float width, float height) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData,
                 GL_STATIC_DRAW);
    glGenBuffers(1, &uvBuffer);
    this->width = width;
    this->height = height;
  }

private:
  glm::vec3 position;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  float width;
  float height;

  void setUVs(float left, float right, float top, float bottom) override {
    GLfloat gUvBufferData[] = {
        left, top,
        right, top,
        left, bottom,
        right, bottom
    };

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gUvBufferData), gUvBufferData,
                 GL_STATIC_DRAW);
  }

  void setPosition(const glm::vec3 &position) override {
    this->position = position;
  }

  const glm::vec3 &getPosition() const override {
    return position;
  }

  void draw(const glm::mat4 &mvp, int matrixId) const override {
    glm::mat4 mvp2 = glm::translate(mvp,
                                    {position.x, position.y - position.z, 0});
    mvp2 = glm::scale(mvp2, {width, height, 1});
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp2[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
  }

  bool hits(float x, float y, float margin) const override {
    if (x < position.x - margin) {
      return false;
    }
    if (y < position.y - margin) {
      return false;
    }
    if (x > position.x + width + margin) {
      return false;
    }
    if (y > position.y + height + margin) {
      return false;
    }
    return true;
  }
};

Sprite *newSprite(float width, float height) {
  return new LocalSprite(width, height);
}