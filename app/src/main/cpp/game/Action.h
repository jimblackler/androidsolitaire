#pragma once

enum MOVE_TYPE {
  DRAW,
  TO_TABLEAU,
  TO_FOUNDATION,
};

struct Action {
  enum MOVE_TYPE moveType;
  int card;
  int destinationIdx;

  bool operator<(const Action &o) const {
    if (moveType < o.moveType) {
      return true;
    }
    if (moveType > o.moveType) {
      return false;
    }
    if (card < o.card) {
      return true;
    }
    if (card > o.card) {
      return false;
    }
    return destinationIdx < o.destinationIdx;
  }
};