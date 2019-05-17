#include "GameState.h"
#include "Action.h"

#include "vectorUtils.h"

#include <random>

class LocalGameState : public GameState {
  std::vector<int> deck;
  std::vector<int> stock;
  std::vector<int> waste;
  std::vector<std::vector<int>> foundations;
  std::vector<std::vector<int>> tableausFaceDown;
  std::vector<std::vector<int>> tableausFaceUp;

  void newGame() override {  // todo ... add from settings
    // Add cards to deck
    deck.clear();
    for (int idx = 0; idx != NUMBER_CARDS; idx++) {
      deck.push_back(idx);
    }

    std::mt19937 generator((unsigned long) time(0));
    std::shuffle(deck.begin(), deck.end(), generator);

    // Tableaus.
    tableausFaceDown.resize(NUMBER_TABLEAUS);
    tableausFaceUp.resize(NUMBER_TABLEAUS);
    for (int tableau = 0; tableau != NUMBER_TABLEAUS; tableau++) {
      std::vector<int> &tableauFaceDown = tableausFaceDown[tableau];
      tableauFaceDown.clear();
      for (int position = 0; position <= tableau - 1; position++) {
        tableauFaceDown.push_back(vector_utils::pop(deck));
      }
      std::vector<int> &tableauFaceUp = tableausFaceUp[tableau];
      tableauFaceUp.clear();
      tableauFaceUp.push_back(vector_utils::pop(deck));
    }

    // Stock.
    stock.clear();
    while (!deck.empty()) {
      stock.push_back(vector_utils::pop(deck));
    }

    // Foundations
    foundations.resize(NUMBER_FOUNDATIONS);
    for (auto &foundation : foundations) {
      foundation.clear();
    }

    waste.clear();
  }

  void _draw() {
    if (stock.empty()) {
      while (!waste.empty()) {
        stock.push_back(vector_utils::pop(waste));
      }
    } else {
      // X cards from stock to waste.
      for (int idx = 0; idx != CARDS_TO_DRAW && !stock.empty(); idx++) {
        waste.push_back(vector_utils::pop(stock));
      }
    }
  }

  bool remove(int cardNumber) {
    // In tableau cards?
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      std::vector<int> &tableauFaceUp = tableausFaceUp[tableauIdx];
      if (vector_utils::remove(tableauFaceUp, cardNumber)) {
        // Reveal undercard if needed.
        if (tableauFaceUp.empty()) {
          std::vector<int> &tableauFaceDown = tableausFaceDown[tableauIdx];
          if (!tableauFaceDown.empty()) {
            tableauFaceUp.insert(tableauFaceUp.begin(),
                                 vector_utils::pop(tableauFaceDown));
          }
        }
        return true;
      }
    }
    // In stock cards?
    if (vector_utils::remove(stock, cardNumber)) {
      return true;
    }

    // In waste cards?
    if (vector_utils::remove(waste, cardNumber)) {
      return true;
    }

    // Foundations
    for (auto &foundation: foundations) {
      if (vector_utils::remove(foundation, cardNumber)) {
        return true;
      }
    }

    return false;
  }

  int stackedUnder(int cardNumber) const {
    // In tableau cards?
    for (auto &tableau : tableausFaceUp) {
      int idx = vector_utils::indexOf(tableau, cardNumber);
      if (idx >= 0 && idx < tableau.size() - 1) {
        return tableau[idx + 1];
      }
    }
    return -1;
  }

  std::list<int> getStack(int cardNumber) const override {
    int card = cardNumber;
    auto cards = std::list<int>();
    while (card != -1) {
      cards.push_back(card);
      card = stackedUnder(card);
    }
    return cards;
  }

  void _moveToTableau(int cardNumber, int tableauIdx) {
    int movingCard = cardNumber;
    do {
      int stackedOn = this->stackedUnder(movingCard);
      if (remove(movingCard)) {
        tableausFaceUp[tableauIdx].push_back(movingCard);
      }
      movingCard = stackedOn;
    } while (movingCard != -1);
  }

  void _moveToFoundation(int cardNumber, int foundationIdx) {
    if (remove(cardNumber)) {
      foundations[foundationIdx].push_back(cardNumber);
    }
  }

  void execute(const Action &action) override {
    switch (action.moveType) {
      case MOVE_TYPE::DRAW:
        _draw();
        break;
      case MOVE_TYPE::TO_TABLEAU:
        _moveToTableau(action.card, action.destinationIdx);
        break;
      case MOVE_TYPE::TO_FOUNDATION:
        _moveToFoundation(action.card, action.destinationIdx);
        break;
    }
  }

  const std::set<int> getMovableCards() const {
    auto movableCards = std::set<int>();

    if (!waste.empty()) {
      movableCards.insert(waste.back());
    }

    for (auto &foundation : foundations) {
      if (!foundation.empty()) {
        movableCards.insert(foundation.back());
      }
    }

    for (auto &tableau : tableausFaceUp) {
      for (int cardNumber : tableau) {
        movableCards.insert(cardNumber);
      }
    }
    return movableCards;
  }

  const std::map<int, std::set<Action>> getActions() const override {
    std::map<int, std::set<Action>> actionsFor;
    const std::set<int> &movableCards = getMovableCards();

    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS;
         foundationIdx++) {
      auto &foundation = foundations[foundationIdx];
      std::list<int> canPlaceOn;
      if (foundation.empty()) {
        // Empty foundation ... will take Aces
        canPlaceOn = std::list<int>{Rules::getCard(0, ACE_TYPE),
                                    Rules::getCard(1, ACE_TYPE),
                                    Rules::getCard(2, ACE_TYPE),
                                    Rules::getCard(3, ACE_TYPE)};
      } else {
        canPlaceOn = Rules::canPlaceOnInFoundation(foundation.back());
      }

      for (int other : canPlaceOn) {
        if (!movableCards.count(other)) {
          continue;
        }
        if (!actionsFor.count(other)) {
          actionsFor[other] = std::set<Action>();
        }
        actionsFor[other].insert(
            {MOVE_TYPE::TO_FOUNDATION, other, foundationIdx});
      }
    }

    // Tableau cards.
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      auto &tableau = tableausFaceUp[tableauIdx];
      std::list<int> canPlaceOn;
      if (tableau.empty()) {
        // Empty tableau ... will take Kings
        canPlaceOn = {Rules::getCard(0, KING_TYPE),
                      Rules::getCard(1, KING_TYPE),
                      Rules::getCard(2, KING_TYPE),
                      Rules::getCard(3, KING_TYPE)};
      } else {
        canPlaceOn = Rules::canPlaceOnInTableau(tableau.back());
      }
      for (int other : canPlaceOn) {
        if (!movableCards.count(other)) {
          continue;
        }
        if (!actionsFor.count(other)) {
          actionsFor[other] = std::set<Action>();
        }
        actionsFor[other].insert({MOVE_TYPE::TO_TABLEAU, other, tableauIdx});
      }
    }
    return actionsFor;
  }

  const std::vector<int> &getStock() const override {
    return stock;
  }

  const std::vector<int> &getWaste() const override {
    return waste;
  }

  const std::vector<std::vector<int>> &getFoundations() const override {
    return foundations;
  }

  const std::vector<std::vector<int>> &getTableausFaceDown() const override {
    return tableausFaceDown;
  }

  const std::vector<std::vector<int>> &getTableausFaceUp() const override {
    return tableausFaceUp;
  }
};

GameState *newGameState() {
  return new LocalGameState();
}