#include <GLES/gl.h>
#include <GLES3/gl32.h>
#include "Sprite.h"

static const GLfloat gVertexBufferData[] = {
    0, 0, 0,
    1.0F, 0, 0,
    0, 1.0F, 0,
    1.0F, 1.0F, 0,
};

class LocalSprite : public Sprite {
public:
  void setUVs(float left, float right, float top, float bottom) override {

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData,
                 GL_STATIC_DRAW);

    GLfloat gUvBufferData[] = {
        left, top,
        right, top,
        left, bottom,
        right, bottom
    };
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gUvBufferData), gUvBufferData,
                 GL_STATIC_DRAW);
  }

private:
  GLuint vertexBuffer;
  GLuint uvBuffer;

  void draw() override {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }
};

Sprite *newSprite() {
  return new LocalSprite();
}