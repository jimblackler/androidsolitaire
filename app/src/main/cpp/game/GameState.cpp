#include "GameState.h"
#include "Action.h"

#include <random>

const int MAX_UNDO_MOVES = 5;

class LocalGameState : public GameState {

  std::vector<int> stock;
  std::vector<int> waste;
  std::vector<std::vector<int>> foundations;
  std::vector<std::vector<int>> tableausFaceDown;
  std::vector<std::vector<int>> tableausFaceUp;
  std::vector<void *> moves;
  unsigned long seed = 0;

  void newGame() override {  // todo ... add from settings
    seed = (unsigned long) time(0);
    restartGame();
  }

  void restartGame() override {
    // Add cards to deck
    std::vector<int> deck;
    for (int idx = 0; idx != NUMBER_CARDS; idx++) {
      deck.push_back(idx);
    }

    std::mt19937 generator(seed);
    std::shuffle(deck.begin(), deck.end(), generator);

    // Tableaus.
    tableausFaceDown.resize(NUMBER_TABLEAUS);
    tableausFaceUp.resize(NUMBER_TABLEAUS);
    for (int tableau = 0; tableau != NUMBER_TABLEAUS; tableau++) {
      std::vector<int> &tableauFaceDown = tableausFaceDown[tableau];
      tableauFaceDown.clear();
      for (int position = 0; position <= tableau - 1; position++) {
        tableauFaceDown.push_back(deck.back());
        deck.pop_back();
      }
      std::vector<int> &tableauFaceUp = tableausFaceUp[tableau];
      tableauFaceUp.clear();
      tableauFaceUp.push_back(deck.back());
      deck.pop_back();
    }

    // Stock.
    stock.clear();
    while (!deck.empty()) {
      stock.push_back(deck.back());
      deck.pop_back();
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
        stock.push_back(waste.back());
        waste.pop_back();
      }
    } else {
      // X cards from stock to waste.
      for (int idx = 0; idx != CARDS_TO_DRAW && !stock.empty(); idx++) {
        waste.push_back(stock.back());
        stock.pop_back();
      }
    }
  }

  bool remove(int cardNumber) {
    // In tableau cards?
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      std::vector<int> &tableauFaceUp = tableausFaceUp[tableauIdx];
      auto entry = std::find(tableauFaceUp.begin(), tableauFaceUp.end(),
                             cardNumber);
      if (entry == tableauFaceUp.end()) {
        continue;
      }
      tableauFaceUp.erase(entry);
      // Reveal undercard if needed.
      if (tableauFaceUp.empty()) {
        std::vector<int> &tableauFaceDown = tableausFaceDown[tableauIdx];
        if (!tableauFaceDown.empty()) {
          tableauFaceUp.insert(tableauFaceUp.begin(),
                               tableauFaceDown.back());
          tableauFaceDown.pop_back();
        }
      }
      return true;
    }

    // In waste cards?
    if (cardNumber == waste.back()) {
      waste.pop_back();
      return true;
    }

    // Foundations
    for (auto &foundation: foundations) {
      auto entry = std::find(foundation.begin(), foundation.end(), cardNumber);
      if (entry != foundation.end()) {
        foundation.erase(entry);
        return true;
      }
    }

    return false;
  }

  int stackedUnder(int cardNumber) const {
    // In tableau cards?
    for (auto &tableau : tableausFaceUp) {
      auto entry = std::find(tableau.begin(), tableau.end(), cardNumber);
      if (entry == tableau.end()) {
        continue;
      }
      entry++;
      if (entry != tableau.end()) {
        return *entry;
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
    size_t size = 0;
    moves.emplace_back(serialize(&size));
    if (moves.size() > MAX_UNDO_MOVES) {
      free(moves.front());
      moves.erase(moves.begin());
    }

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
  };

  void undo() override {
    if (moves.empty()) {
      return;
    }
    deserialize(moves.back());
    moves.pop_back();
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
        canPlaceOn = {Rules::getCard(0, ACE_TYPE),
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

  static void _serialize(int pass, unsigned long int value,
                         void **memory, size_t *size) {
    if (pass == 0) {
      *size += sizeof(unsigned long);
    } else {
      auto memoryi = (unsigned long **) memory;
      *(*memoryi)++ = value;
    }
  }

  static void _serialize(int pass, const std::vector<int> &vector,
                         void **memory, size_t *size) {
    if (pass == 0) {
      *size += sizeof(int) + sizeof(int) * vector.size();
    } else {
      auto memoryi = (int **) memory;
      *(*memoryi)++ = (int) vector.size();
      for (int value : vector) {
        *(*memoryi)++ = value;
      }
    }
  }

  static void _deserialize(std::vector<int> &vector, const void **memory) {
    auto memoryi = (int **) memory;
    auto size = (unsigned int) *(*memoryi)++;
    vector.resize(size);
    for (int idx = 0; idx < size; idx++) {
      vector[idx] = *(*memoryi)++;
    }
  }

  static unsigned long _deserialize(const void **memory) {
    auto memoryi = (unsigned long **) memory;
    return *(*memoryi)++;
  }

  void *serialize(size_t *size) const override {
    void *memory = nullptr;
    void *write;
    for (int pass = 0; pass < 2; pass++) {
      _serialize(pass, seed, &write, size);
      _serialize(pass, stock, &write, size);
      _serialize(pass, waste, &write, size);
      for (auto &foundation : foundations) {
        _serialize(pass, foundation, &write, size);
      }
      for (auto &tableauFaceDown : tableausFaceDown) {
        _serialize(pass, tableauFaceDown, &write, size);
      }
      for (auto &tableauFaceUp : tableausFaceUp) {
        _serialize(pass, tableauFaceUp, &write, size);
      }
      if (pass == 0) {
        memory = malloc(*size);
        write = memory;
      }
    }
    return memory;
  }

  void deserialize(const void *memory) override {
    seed = _deserialize(&memory);
    _deserialize(stock, &memory);
    _deserialize(waste, &memory);
    foundations.resize(NUMBER_FOUNDATIONS);
    for (auto &foundation : foundations) {
      _deserialize(foundation, &memory);
    }
    tableausFaceDown.resize(NUMBER_TABLEAUS);
    for (auto &tableauFaceDown : tableausFaceDown) {
      _deserialize(tableauFaceDown, &memory);
    }
    tableausFaceUp.resize(NUMBER_TABLEAUS);
    for (auto &tableauFaceUp : tableausFaceUp) {
      _deserialize(tableauFaceUp, &memory);
    }
  }

};

GameState *newGameState() {
  return new LocalGameState();
}
