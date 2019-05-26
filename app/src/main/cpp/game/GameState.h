#include "Rules.h"
#include "Action.h"

#include <map>
#include <vector>
#include <set>

class GameState {
public:

  virtual void newGame() = 0;
  virtual void restartGame() = 0;

  virtual const std::vector<int> &getStock() const = 0;
  virtual const std::vector<int> &getWaste() const = 0;
  virtual const std::vector<std::vector<int>> &getFoundations() const = 0;
  virtual const std::vector<std::vector<int>> &getTableausFaceDown() const = 0;
  virtual const std::vector<std::vector<int>> &getTableausFaceUp() const = 0;
  virtual void execute(const Action &action) = 0;
  virtual void undo() = 0;
  virtual const std::map<int, std::set<Action>> getActions() const = 0;
  virtual std::list<int> getStack(int cardNumber) const = 0;
  virtual void *serialize(size_t *savedStateSize) const = 0;
  virtual void deserialize(const void *memory) = 0;
};

extern GameState *newGameState();