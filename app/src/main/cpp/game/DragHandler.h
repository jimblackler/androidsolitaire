#pragma once

class DragHandler {

public:
  virtual std::list<int> startDrag(int cardNumber) = 0;
};
