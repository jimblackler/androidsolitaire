#include <random>
#include <vector>
#include "VectorUtils.h"

class CardList  {
  std::vector<int> cards;
public:

  CardList() {
  }

  void add(int cardNumber) {
    cards.push_back(cardNumber);
  }

  void pushFront(int cardNumber) {
    cards.insert(cards.begin(), cardNumber);
  }

  const std::vector<int> asArray() const {  //TODO rename in C++ version
    return cards;
  }

  int get(int idx) const {
    return cards[idx];
  }

  unsigned int length() const {
    return (int) cards.size();
  }

  int pop() {
    return vector_utils::pop(cards);
  }

  int indexOf(int cardNumber) const {
    return vector_utils::indexOf(cards, cardNumber);
  }

  bool remove(int cardNumber) {
    return vector_utils::remove(cards, cardNumber);
  }

  void clear() {
    cards.clear();
  }

  void shuffle() {
    std::mt19937 generator((unsigned long) time(0));
    std::shuffle(cards.begin(), cards.end(), generator);
  }
};
