#include <vector>

class CardList {
  std::vector<int> cards;
public:

  CardList() {


  }

  void add(int cardNumber) {
    cards.push_back(cardNumber);
  }
//
//  pushFront(cardNumber) {
//      return this.cards.splice(0, 0, cardNumber);
//  }
//
//  asArray() {
//    return this.cards;
//  }
//
//  get(idx) {
//      return this.cards[idx];
//  }
//
//  length() {
//    return this.cards.length;
//  }
//
//  pop() {
//    return this.cards.pop();
//  }
//
//  indexOf(cardNumber) {
//      return this.cards.indexOf(cardNumber);
//  }
//
//  remove(cardNumber) {
//      const idx = this.indexOf(cardNumber);
//      if (idx === -1) {
//        return false;
//      } else {
//        this.cards.splice(idx, 1);
//        return true;
//      }
//  }

  void shuffle() {
    std::random_shuffle(cards.begin(), cards.end());

  }
};
