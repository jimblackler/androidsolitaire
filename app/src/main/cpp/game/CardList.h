#include <vector>

class CardList {
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

  std::vector<int> asArray() {  //TODO rename in C++ version
    return cards;
  }

  int get(int idx) {
    return cards[idx];
  }

  unsigned int length() {
    return (int) cards.size();
  }

  int pop() {
    int result = cards.back();
    cards.pop_back();
    return result;
  }

  int indexOf(int cardNumber) {
    auto entry = find(cards.begin(), cards.end(), cardNumber);
    if (entry == cards.end()) {
      return -1;
    }
    return (int) (entry - cards.begin());
  }

  bool remove(int cardNumber) {
    auto entry = find(cards.begin(), cards.end(), cardNumber);
    if (entry == cards.end()) {
      return false;
    }
    cards.erase(entry);
    return true;
  }

  void shuffle() {
    std::random_shuffle(cards.begin(), cards.end());
  }
};
