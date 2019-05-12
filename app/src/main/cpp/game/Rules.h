#pragma once

#include <list>
const auto NUMBER_CARDS = 52;
const auto NUMBER_TABLEAUS = 7;
const auto ACE_TYPE = 0;
const auto KING_TYPE = 12;
const auto NUMBER_CARDS_IN_SUIT = 13;
const auto NUMBER_FOUNDATIONS = 4;
const auto CARDS_TO_DRAW = 3; // TODO: make part of GameSettings

class Rules {
public:


  static int getSuit(int cardNumber) {
    return cardNumber / NUMBER_CARDS_IN_SUIT;
  };

  static int getType(int cardNumber) {
    return cardNumber % NUMBER_CARDS_IN_SUIT;
  };

  static int getCard(int suit, int type) {
    return suit * NUMBER_CARDS_IN_SUIT + type;
  };

  static std::list<int> canPlaceOnInTableau(int cardNumber) {
    const int suit = getSuit(cardNumber);
    const int type = getType(cardNumber);
    if (type == ACE_TYPE) {
      return std::list<int>(); // Nothing goes on aces.
    }
    if (suit < 2) {
      return std::list<int>{getCard(2, type - 1), getCard(3, type - 1)};
    } else {
      return std::list<int>{getCard(0, type - 1), getCard(1, type - 1)};
    }
  }

  static std::list<int> canPlaceOnInFoundation(int cardNumber) {
    const int suit = getSuit(cardNumber);
    const int type = getType(cardNumber);
    if (type == KING_TYPE) {
      return std::list<int>(); // Nothing goes on kings.
    }
    return std::list<int>{getCard(suit, type + 1)};
  }
};
