class Renderer {
public:
  virtual void drawFrame() = 0;
};

extern Renderer * NewRenderer();