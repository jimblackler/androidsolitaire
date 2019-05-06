class Sprite {
public:
  virtual void draw() = 0;
};

extern Sprite *NewSprite(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom);