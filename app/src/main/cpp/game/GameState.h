#include "Rules.h"
#include "CardList.h"
#include "Action.h"

#include <map>
#include <vector>
#include <set>

class GameState {
public:

  virtual void newGame() = 0;

  virtual const CardList &getStock() = 0;
  virtual const CardList &getWaste() = 0;
  virtual const std::vector<CardList> &getFoundations() = 0;
  virtual const std::vector<CardList> &getTableausFaceDown() = 0;
  virtual const std::vector<CardList> &getTableausFaceUp() = 0;
  virtual void execute(const Action &action) = 0;
  virtual std::map<int, std::set<Action>> getActions() = 0;
  virtual std::list<int> getStack(int cardNumber) = 0;
};

extern GameState *newGameState();