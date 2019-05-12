#include "GameState.h"
#include "MathUtils.h"
#include "Rules.h"
#include "DragHandler.h"
#include "GameController.h"
#include "../Renderer.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <sys/time.h>

static const auto STOCK_X = 42;
static const auto STOCK_Y = 42;
static const auto TABLEAU_X = STOCK_X;
static const auto TABLEAU_Y = 210;
static const auto TABLEAU_X_SPACING = 115;
static const auto TABLEAU_Y_SPACING = 25;
static const auto FOUNDATION_X = 386;
static const auto FOUNDATION_X_SPACING = 115;
static const auto FOUNDATION_Y = STOCK_Y;
static const auto WASTE_X = 196;
static const auto WASTE_X_SPACING = 22;
static const auto WASTE_Y = STOCK_Y;
static const auto RAISE_DURATION = 80;
static const auto RAISE_HEIGHT = 8;
static const auto ANIMATION_TIME = 400;
static const auto ANIMATION_DISTANCE_MAX = 800;
static const auto ANIMATION_TIME_SUPPLEMENT = 125;
static const auto WASTE_DRAW_STAGGER = 20;
static const auto ANIMATION_TEST_SLOWDOWN = 1;
static const auto FLY_HEIGHT = 30;
static const auto FLY_DISTANCE_MAX = 800;

struct Curve {
  float startTime;
  float endTime;

  std::vector<float> start;

  float flyHeight;
  float endX;
  float endY;

  bool draggable;
};

static float getTimeNow() {
  struct timeval timeVal;
  gettimeofday(&timeVal, nullptr);
  return timeVal.tv_sec * 1000000 + timeVal.tv_usec;
}


class LocalGameController : public GameController {
public:
  Renderer *renderer;
  std::map<int, Curve> curves;
  std::map<int, int> cardHistory;
  float riseStarted;
  std::list<int> raisingCards;
  int lastCardMoved = -1;
  GameState *gameState;

  LocalGameController(Renderer *renderer, GameState *gameState) {
    this->renderer = renderer;
    this->gameState = gameState;

    for (int idx = 0; idx != NUMBER_CARDS; idx++) {
      renderer->faceDown(idx);
    }

    // Placeholder; stock
    renderer->placeHolder(STOCK_X, STOCK_Y, []() {});

    // Placeholder; tableau
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      renderer->placeHolder(TABLEAU_X + TABLEAU_X_SPACING * tableauIdx, TABLEAU_Y, []() {});
    }

    // Placeholder; foundation
    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS; foundationIdx++) {
      renderer->placeHolder(FOUNDATION_X + FOUNDATION_X_SPACING * foundationIdx, FOUNDATION_Y,
                            []() {});
    }
  }

  void _animate() {
    auto timeNow = getTimeNow();
    for (auto pair : curves) {
      auto k = pair.first;
      auto curve = pair.second;
      if (timeNow < curve.startTime) {
        continue;
      }
      auto t = MathUtils::toT(curve.startTime, curve.endTime, timeNow);
      if (t > 1) {
        renderer->positionCard(k, curve.endX, curve.endY, 0);
        curves.erase(k);
      } else {
        float multiplier1 = sinf(t * (float) M_PI / 2);
        float v;

        if (curve.start[2] < curve.flyHeight) {
          float start = (float) M_PI - asinf(curve.start[2] / curve.flyHeight);
          float a = MathUtils::tInRange(start, 0, t);
          v = sinf(a) * curve.flyHeight;
        } else {
          v = curve.start[2] * (1 - t);
        }

        renderer->positionCard(k, MathUtils::tInRange(curve.start[0], curve.endX, multiplier1),
                               MathUtils::tInRange(curve.start[1], curve.endY, multiplier1), v);
      }
    }
    if (!raisingCards.empty()) {
      float t = (timeNow - riseStarted) / RAISE_DURATION;
      if (t > 1) {
        t = 1;
      }
      for (int cardNumber : raisingCards) {
        std::vector<float> position = renderer->getCardPosition(cardNumber);
        renderer->positionCard(cardNumber, position[0], position[1], RAISE_HEIGHT * t);
      }
      if (t == 1) {
        raisingCards.clear();
        raisingCards.clear();
      }
    }
  };

  void placeHolder(const int x, const int y, void (*onClick)()) override {

  }

  void render() override {
    // Stop all animations immediately (old onArrive functions are invalid)
    for (const auto &pair : curves) {
      auto k = pair.first;
      auto curve = pair.second;
      renderer->positionCard(k, curve.endX, curve.endY, 0);
      curves.erase(k);
    }

    // Position stock cards.
    auto stock = gameState->getStock();
    int stockLength = stock.length();

    for (int idx = 0; idx != stockLength; idx++) {
      int cardNumber = stock.get(idx);
      renderer->faceDown(cardNumber);
      _placeCard(cardNumber, STOCK_X, STOCK_Y, false, 0);
    }

    // Position waste cards.
    auto waste = gameState->getWaste();
    int wasteLength = waste.length();
    for (int idx = 0; idx != wasteLength; idx++) {
      int cardNumber = waste.get(idx);

      renderer->faceUp(cardNumber);
      int staggerOrder = std::max(idx - wasteLength + CARDS_TO_DRAW, 0);
      float delay = staggerOrder * WASTE_DRAW_STAGGER * ANIMATION_TEST_SLOWDOWN;

      int position = idx - (wasteLength - std::min(CARDS_TO_DRAW, wasteLength));
      if (position < 0) {
        position = 0;
      }
      _placeCard(cardNumber, WASTE_X + WASTE_X_SPACING * position, WASTE_Y, idx == wasteLength - 1,
                 delay);
    }

    // Position foundation cards.
    auto foundations = gameState->getFoundations();
    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS; foundationIdx++) {
      CardList &foundation = foundations[foundationIdx];
      int foundationLength = foundation.length();

      for (int position = 0; position < foundationLength; position++) {
        int cardNumber = foundation.get(position);
        renderer->faceUp(cardNumber);
        void (*onArrive)();
        if (position == foundationLength - 1) {
          std::list<int> cards{cardNumber};
        }
        _placeCard(cardNumber, FOUNDATION_X + FOUNDATION_X_SPACING * foundationIdx,
                   FOUNDATION_Y, true, 0);
      }
    }

    // Position tableau cards.
    auto tableausFaceDown = gameState->getTableausFaceDown();
    auto tableausFaceUp = gameState->getTableausFaceUp();
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      auto tableau = tableausFaceDown[tableauIdx];
      int faceDownLength = tableau.length();
      for (int position = 0; position < faceDownLength; position++) {
        int cardNumber = tableau.get(position);
        _placeCard(cardNumber, TABLEAU_X + TABLEAU_X_SPACING * tableauIdx,
                   TABLEAU_Y + TABLEAU_Y_SPACING * position, false, 0);
        renderer->faceDown(cardNumber);
      }
      tableau = tableausFaceUp[tableauIdx];
      int tableauLength = tableau.length();

      for (int position = 0; position < tableauLength; position++) {
        int cardNumber = tableau.get(position);
        renderer->faceUp(cardNumber);
        _placeCard(cardNumber, TABLEAU_X + TABLEAU_X_SPACING * tableauIdx,
                   TABLEAU_Y + TABLEAU_Y_SPACING * (position + faceDownLength), true, 0);
      }
    }

    // Auto play
//    if (gameState->stock.length() == = 0 && gameState->waste.length() == = 0) {
//      const actionsFor = gameState->getActions();
//      let anyFaceDown = false;
//      for (let tableauIdx = 0; tableauIdx != = Rules.NUMBER_TABLEAUS; tableauIdx++) {
//        const tableau = gameState->tableausFaceDown[tableauIdx];
//        if (tableau.length() > 0) {
//          anyFaceDown = true;
//          break;
//        }
//      }
//      if (!anyFaceDown) {
//        window.setTimeout(() = > {
//            for (let tableauIdx = 0; tableauIdx !== Rules.NUMBER_TABLEAUS; tableauIdx++) {
//              const tableau = gameState->tableausFaceUp[tableauIdx];
//              if (tableau.length() <= 0) {
//                continue;
//              }
//              const position = tableau.length() - 1;
//              const cardNumber = tableau.get(position);
//              const actions = actionsFor.get(cardNumber);
//              if (!actions) {
//                continue;
//              }
//              for (const action of actions) {
//                if (action.moveType == = MOVE_TYPE.TO_TABLEU) {
//                  continue;
//                }
//                gameState->execute(action);
//                GameStore.store(gameState);
//                this.render(gameState);
//                return;
//              }
//            }
//            // All complete. If the user hits refresh, start a new game.
//            GameStore.erase();
//        }, 400);
//      }
//    }
  }

  void _placeCard(int cardNumber, float x, float y, bool draggable, float delay) {
    float timeNow = getTimeNow();
    renderer->raiseCard(cardNumber);

    std::vector<float> position = renderer->getCardPosition(cardNumber);

    float deltaX = position[0] - x;
    float deltaY = position[1] - y;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    float flyDistance = std::min(distance, (float) FLY_DISTANCE_MAX);
    float flyHeight = FLY_HEIGHT * flyDistance / FLY_DISTANCE_MAX;
    float animationDistance = std::min(distance, (float) ANIMATION_DISTANCE_MAX);

    float animationTime = ANIMATION_TEST_SLOWDOWN *
                          (ANIMATION_TIME * animationDistance / ANIMATION_DISTANCE_MAX +
                           ANIMATION_TIME_SUPPLEMENT);

    Curve curve;
    curve.startTime = timeNow + delay;
    curve.endTime = timeNow + animationTime + delay;
    curve.start = position;
    curve.endX = x;
    curve.endY = y;
    curve.flyHeight = flyHeight;
    curve.draggable = draggable;
    curves[cardNumber] = curve;
  }

  std::list<int> startDrag(int card) {
    auto cards = gameState->getStack(card);
    riseStarted = getTimeNow();
    raisingCards = cards;
    return cards;
  }

  void cardClickedOrDropped(int card, bool click) {
//    const gameState = this.gameState;
//    const cards = gameState.getStack(card);
//    const cardNumber = cards[0];
//    if (this.lastCardMoved !== cardNumber) {
//      this.cardHistory = new Map();
//      this.lastCardMoved = cardNumber;
//    }
//    const actionsFor = gameState.getActions();
//    let actions = actionsFor.get(cardNumber);
//
//    if (actions) {
//      // if click ... priority is (age-> usefulness -> proximity)
//      // otherwise it is proximity
//      if (click) {
//        // Filter actions to oldest actions.
//        let oldest = Number.MAX_VALUE;
//        let oldestActions = [];
//        for (const action of actions) {
//          const actionKey = JSON.stringify(action);
//          const time = this.cardHistory.has(actionKey) ? this.cardHistory.get(actionKey) : Number.MIN_VALUE;
//          if (time === oldest) {
//            oldestActions.push(action);
//          } else if (time < oldest) {
//            oldest = time;
//            oldestActions = [action];
//          }
//        }
//        if (oldestActions) {
//          actions = oldestActions;
//        }
//
//        // Filter actions to most useful actions.
//        let mostUseful = Number.MIN_VALUE;
//        let mostUsefulActions = [];
//        for (const action of actions) {
//          const useful = action.moveType;
//          if (useful === mostUseful) {
//            mostUsefulActions.push(action);
//          } else if (useful > mostUseful) {
//            mostUseful = useful;
//            mostUsefulActions = [action];
//          }
//        }
//        if (mostUsefulActions) {
//          actions = mostUsefulActions;
//        }
//      }
//
//      // Find closet action.
//      const position = this.renderer.getCardPosition(cardNumber);
//      let closest = Number.MAX_VALUE;
//      let closestAction;
//      for (const action of actions) {
//        if (cards.length === 1 || action.moveType === MOVE_TYPE.TO_TABLEAU) {
//          let x;
//          let y;
//          if (action.moveType === MOVE_TYPE.TO_TABLEAU) {
//            x = TABLEAU_X + TABLEAU_X_SPACING * action.destinationIdx;
//            y = TABLEAU_Y + (gameState.tableausFaceUp[action.destinationIdx].length() +
//                             gameState.tableausFaceDown[action.destinationIdx].length()) * TABLEAU_Y_SPACING;
//          } else if (action.moveType === MOVE_TYPE.TO_FOUNDATION) {
//            x = FOUNDATION_X + FOUNDATION_X_SPACING * action.destinationIdx;
//            y = FOUNDATION_Y;
//          }
//
//          const distance = Math.pow(position[0] - x, 2) + Math.pow(position[1] - y, 2);
//          if (distance < closest) {
//            closest = distance;
//            closestAction = action;
//          }
//        }
//      }
//      if (closestAction) {
//        this.cardHistory.set(JSON.stringify(closestAction), new Date().getTime());
//        gameState.execute(closestAction);
//        GameStore.store(gameState);
//      }
//    }
//
//    this.render();
  }




};

GameController *newGameController(Renderer *renderer, GameState *gameState) {
  return new LocalGameController(renderer, gameState);
}