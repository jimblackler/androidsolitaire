#include <list>

class Rules {
public:

  static const auto NUMBER_CARDS = 52;
  static const auto NUMBER_TABLEAUS = 7;
  static const auto ACE_TYPE = 0;
  static const auto KING_TYPE = 12;
  static const auto NUMBER_CARDS_IN_SUIT = 13;
  static const auto NUMBER_FOUNDATIONS = 4;

  int getSuit(int cardNumber) {
    return cardNumber / NUMBER_CARDS_IN_SUIT;
  };

  int getType(int cardNumber) {
    return cardNumber % NUMBER_CARDS_IN_SUIT;
  };

  int getCard(int suit, int type) {
    return suit * NUMBER_CARDS_IN_SUIT + type;
  };

  std::list<int> canPlaceOnInTableau(int cardNumber) {
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

  std::list<int> canPlaceOnInFoundation(int cardNumber) {
    const int suit = getSuit(cardNumber);
    const int type = getType(cardNumber);
    if (type == KING_TYPE) {
      return std::list<int>(); // Nothing goes on kings.
    }
    return std::list<int>{getCard(suit, type + 1)};
  }
};
