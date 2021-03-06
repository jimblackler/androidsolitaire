#pragma once

#include <list>

class DragHandler {

public:
  virtual std::list<int> startDrag(int cardNumber) = 0;
  virtual void cardClickedOrDropped(int card, bool click) = 0;
};
