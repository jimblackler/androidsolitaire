#include <random>
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
    int result = cards.back();
    cards.pop_back();
    return result;
  }

  int indexOf(int cardNumber) const {
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

  void clear() {
    cards.clear();
  }

  void shuffle() {
    std::mt19937 generator((unsigned long) time(0));
    std::shuffle(cards.begin(), cards.end(), generator);
  }
};
