#include "GameState.h"
#include "Action.h"

class LocalGameState : public GameState {
  CardList deck;
  CardList stock;
  CardList waste;
  std::vector<CardList> foundations;
  std::vector<CardList> tableausFaceDown;
  std::vector<CardList> tableausFaceUp;

  void newGame() override {  // todo ... add from settings
    // Add cards to deck
    for (int idx = 0; idx != NUMBER_CARDS; idx++) {
      deck.add(idx);
    }

    deck.shuffle();

    // Tableaus.
    tableausFaceDown.resize(NUMBER_TABLEAUS);
    tableausFaceUp.resize(NUMBER_TABLEAUS);
    for (int tableau = 0; tableau != NUMBER_TABLEAUS; tableau++) {
      for (int position = 0; position <= tableau - 1; position++) {
        tableausFaceDown[tableau].add(deck.pop());
      }
      tableausFaceUp[tableau].add(deck.pop());
    }

    // Stock.
    while (deck.length() > 0) {
      stock.add(deck.pop());
    }

    // Foundations
    foundations.resize(NUMBER_FOUNDATIONS);
  }

  void _draw() {
    if (stock.length() == 0) {
      while (waste.length()) {
        int drawn = waste.pop();
        stock.add(drawn);
      }
    } else {
      // X cards from stock to waste.
      for (int idx = 0; idx != CARDS_TO_DRAW && stock.length(); idx++) {
        int drawn = stock.pop();
        waste.add(drawn);
      }
    }
  }

  bool remove(int cardNumber) {
    // In tableau cards?
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS;
         tableauIdx++) {
      auto tableau = tableausFaceUp[tableauIdx];
      if (tableau.remove(cardNumber)) {
        // Reveal undercard if needed.
        if (tableau.length() == 0) {
          auto tableauFaceDown = tableausFaceDown[tableauIdx];
          if (tableauFaceDown.length() > 0) {
            tableau.pushFront(tableauFaceDown.pop());
          }
        }
        return true;
      }
    }
    // In stock cards?
    if (stock.remove(cardNumber)) {
      return true;
    }

    // In waste cards?
    if (waste.remove(cardNumber)) {
      return true;
    }

    // Foundations
    for (int idx = 0; idx != NUMBER_FOUNDATIONS; idx++) {
      if (foundations[idx].remove(cardNumber)) {
        return true;
      }
    }

    return false;
  }

  int stackedUnder(int cardNumber) {
    // In tableau cards?
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS;
         tableauIdx++) {
      auto tableau = tableausFaceUp[tableauIdx];
      int idx = tableau.indexOf(cardNumber);
      if (idx != -1 && idx < tableau.length() - 1) {
        return tableau.get(idx + 1);
      }
    }
    return -1;
  }

  std::list<int> getStack(int cardNumber) override {
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
        tableausFaceUp[tableauIdx].add(movingCard);
      }
      movingCard = stackedOn;
    } while (movingCard != -1);
  }

  void _moveToFoundation(int cardNumber, int foundationIdx) {
    if (remove(cardNumber)) {
      foundations[foundationIdx].add(cardNumber);
    }
  }

  void execute(const Action &action) {
    if (action.moveType == MOVE_TYPE::DRAW) {
      _draw();
    } else if (action.moveType == MOVE_TYPE::TO_TABLEU) {
      _moveToTableau(action.card, action.destinationIdx);
    } else if (action.moveType == MOVE_TYPE::TO_FOUNDATION) {
      _moveToFoundation(action.card, action.destinationIdx);
    }
  }

  std::set<int> getMovableCards() {
    auto movableCards = std::set<int>();

    int wasteLength = waste.length();
    if (wasteLength != 0) {
      int cardNumber = waste.get(wasteLength - 1);
      movableCards.insert(cardNumber);
    }

    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS;
         foundationIdx++) {
      auto foundation = foundations[foundationIdx];
      int foundationLength = foundation.length();
      if (foundationLength != 0) {
        int cardNumber = foundation.get(foundationLength - 1);
        movableCards.insert(cardNumber);
      }
    }

    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS;
         tableauIdx++) {
      auto tableau = tableausFaceUp[tableauIdx];
      int tableauLength = tableau.length();
      for (int position = 0; position < tableauLength; position++) {
        int cardNumber = tableau.get(position);
        movableCards.insert(cardNumber);
      }
    }
    return movableCards;
  }

  bool isComplete() {
    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS;
         foundationIdx++) {
      if (foundations[foundationIdx].length() != NUMBER_CARDS_IN_SUIT) {
        return false;
      }
    }
    return true;
  }

  std::map<int, std::set<Action>> getActions() {
    std::map<int, std::set<Action>> actionsFor;
    std::set<int> movableCards = getMovableCards();


    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS;
         foundationIdx++) {
      auto foundation = foundations[foundationIdx];
      int foundationLength = foundation.length();
      std::list<int> canPlaceOn;
      if (foundationLength == 0) {
        // Empty foundation ... will take Aces
        canPlaceOn = std::list<int>{Rules::getCard(0, ACE_TYPE),
                                    Rules::getCard(1, ACE_TYPE),
                                    Rules::getCard(2, ACE_TYPE),
                                    Rules::getCard(3, ACE_TYPE)};
      } else {
        int cardNumber = foundation.get(foundationLength - 1);
        canPlaceOn = Rules::canPlaceOnInFoundation(cardNumber);
      }

      for (auto other : canPlaceOn) {
        Action action;
        action.card = other;
        action.moveType = MOVE_TYPE::TO_FOUNDATION;
        action.destinationIdx = foundationIdx;
        int card = action.card;
        if (movableCards.find(card) == movableCards.end()) {
          continue;
        }
        if (actionsFor.find(card) == actionsFor.end()) {
          actionsFor[card] = std::set<Action>();
        }
        actionsFor[card].insert(action);
      }
    }

    // Position tableau cards.
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS;
         tableauIdx++) {
      auto tableau = tableausFaceUp[tableauIdx];
      int tableauLength = tableau.length();
      std::list<int> canPlaceOn;
      if (tableauLength == 0) {
        // Empty tableau ... will take Kings
        canPlaceOn = std::list<int>{Rules::getCard(0, KING_TYPE),
                                    Rules::getCard(1, KING_TYPE),
                                    Rules::getCard(2, KING_TYPE),
                                    Rules::getCard(3, KING_TYPE)};
      } else {
        int cardNumber = tableau.get(tableauLength - 1);
        canPlaceOn = Rules::canPlaceOnInTableau(cardNumber);
      }
      for (auto other : canPlaceOn) {
        Action action;
        action.card = other;
        action.moveType = MOVE_TYPE::TO_TABLEU;
        action.destinationIdx = tableauIdx;
        int card = action.card;
        if (movableCards.find(card) == movableCards.end()) {
          continue;
        }
        if (actionsFor.find(card) == actionsFor.end()) {
          actionsFor[card] = std::set<Action>();
        }
        actionsFor[card].insert(action);
      }
    }
    return actionsFor;
  }

  std::set<Action> getAllActions() {
    auto actionsFor = getActions();
    auto actions = std::set<Action>();
    {
      Action action;
      action.moveType = MOVE_TYPE::DRAW;
      action.card = 0;
      action.destinationIdx = 0;
      actions.insert(action);
    }
    for (const auto &pair : actionsFor) {
      for (auto action : pair.second) {
        actions.insert(action);
      }
    }
    return actions;
  }

  const CardList &getStock() override {
    return stock;
  }

  const CardList &getWaste() override {
    return waste;
  }

  const std::vector<CardList> &getFoundations() override {
    return foundations;
  }

  const std::vector<CardList> &getTableausFaceDown() override {
    return tableausFaceDown;
  }

  const std::vector<CardList> &getTableausFaceUp() override {
    return tableausFaceUp;
  }
};

GameState *newGameState() {
  return new LocalGameState();
}