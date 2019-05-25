#include "GameController.h"

#include "Action.h"
#include "GameState.h"
#include "MathUtils.h"
#include "Rules.h"
#include "DragHandler.h"

#include "../Renderer.h"

#include <list>
#include <map>
#include <cmath>
#include <sys/time.h>
#include <thread>

static const auto STOCK_X = 26;
static const auto STOCK_Y = 18;
static const auto TABLEAU_X = STOCK_X;
static const auto TABLEAU_Y = 174;
static const auto TABLEAU_X_SPACING = 115;
static const auto TABLEAU_Y_MIN_SPACING_FACE_DOWN = 6;
static const auto TABLEAU_Y_MAX_SPACING_FACE_DOWN = 14;
static const auto TABLEAU_Y_MAX_SPACING_FACE_UP = 24;
static const auto TABLEAU_BOTTOM_MARGIN = 251;
static const auto FOUNDATION_X = 338;
static const auto FOUNDATION_X_SPACING = 115;
static const auto FOUNDATION_Y = STOCK_Y;
static const auto WASTE_X = 160;
static const auto WASTE_X_SPACING = 22;
static const auto WASTE_Y = STOCK_Y;
static const auto RAISE_DURATION = 80;
static const auto RAISE_HEIGHT = 14;
static const auto ANIMATION_TIME = 400;
static const auto ANIMATION_DISTANCE_MAX = 800;
static const auto ANIMATION_TIME_SUPPLEMENT = 125;
static const auto WASTE_DRAW_STAGGER = 20;
static const auto ANIMATION_TEST_SLOWDOWN = 1;
static const auto FLY_HEIGHT = 45;
static const auto FLY_DISTANCE_MAX = 800;

struct Curve {
  long long startTime;
  long long endTime;
  glm::vec3 start;
  float flyHeight;
  float endX;
  float endY;
  bool draggable;
};

static long long getTimeNow() {
  timeval timeVal{0, 0};
  gettimeofday(&timeVal, nullptr);
  return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
}


class LocalGameController : public GameController {
public:
  Renderer *renderer;
  std::map<int, Curve> curves;
  std::map<Action, long long> cardHistory;
  long long riseStarted;
  std::list<int> raisingCards;
  int lastCardMoved = -1;
  GameState *gameState;
  long long int timeLastRenderered;

  LocalGameController(Renderer *renderer, GameState *gameState) {
    this->renderer = renderer;
    this->gameState = gameState;

    for (int idx = 0; idx != NUMBER_CARDS; idx++) {
      renderer->faceDown(idx);
    }

    // Placeholder; stock
    renderer->placeHolder(STOCK_X, STOCK_Y, [this]() { this->draw(); });

    // Placeholder; tableau
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      renderer->placeHolder(TABLEAU_X + TABLEAU_X_SPACING * tableauIdx,
                            TABLEAU_Y, nullptr);
    }

    // Placeholder; foundation
    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS;
         foundationIdx++) {
      renderer->placeHolder(FOUNDATION_X + FOUNDATION_X_SPACING * foundationIdx,
                            FOUNDATION_Y, nullptr);
    }
  }

  void draw() override {
    gameState->execute({MOVE_TYPE::DRAW, 0, 0});
    // TODO. store
    render();
  }

  void animate() override {
    long long timeNow = getTimeNow();
    if (!curves.empty()) {
      auto it = curves.cbegin();
      while (it != curves.cend()) {
        auto pair = *it;
        auto cardNumber = pair.first;
        auto curve = pair.second;
        if (timeNow < curve.startTime) {
          it++;
          continue;
        }
        float t = MathUtils::toT(curve.startTime, curve.endTime, timeNow);
        if (t > 1) {
          renderer->positionCard(cardNumber, curve.endX, curve.endY, 0);
          renderer->setDraggable(cardNumber, curve.draggable);
          curves.erase(it++);
        } else {
          float multiplier1 = sinf(t * (float) M_PI / 2);
          float v;

          if (curve.start[2] < curve.flyHeight) {
            float start = (float) M_PI - asinf(curve.start[2] / curve.flyHeight);
            float a = MathUtils::tInRange(start, 0.0F, t);
            v = sinf(a) * curve.flyHeight;
          } else {
            v = curve.start[2] * (1 - t);
          }
          float x = MathUtils::tInRange(curve.start[0], curve.endX, multiplier1);
          float y = MathUtils::tInRange(curve.start[1], curve.endY, multiplier1);
          renderer->positionCard(cardNumber, x, y, v);
          it++;
        }
      }
      if (curves.empty()) {
        timeLastRenderered = timeNow;
      }
    }
    if (!raisingCards.empty()) {
      float t = (float) (timeNow - riseStarted) / RAISE_DURATION;
      if (t > 1) {
        t = 1;
      }
      for (int cardNumber : raisingCards) {
        glm::vec3 position = renderer->getCardPosition(cardNumber);
        renderer->positionCard(cardNumber, position[0], position[1],
                               RAISE_HEIGHT * t);
      }
      if (t == 1) {
        raisingCards.clear();
      }
    }
    if (curves.empty()) {
      _tryAutoPlay();
    }
  };

  void render() override {
    const GameState *gameState = this->gameState;
    // Stop all animations immediately (old onArrive functions are invalid)
    for (const auto &pair : curves) {
      int cardNumber = pair.first;
      const Curve &curve = pair.second;
      renderer->positionCard(cardNumber, curve.endX, curve.endY, 0);
    }
    curves.clear();

    // Position stock cards.
    auto &stock = gameState->getStock();
    int stockLength = stock.size();

    for (int idx = 0; idx != stockLength; idx++) {
      int cardNumber = stock[idx];
      renderer->faceDown(cardNumber);
      _placeCard(cardNumber, STOCK_X, STOCK_Y, false, 0);
    }

    // Position waste cards.
    auto &waste = gameState->getWaste();
    int wasteLength = waste.size();
    assert (wasteLength >= 0); // temp till bug is found
    assert (wasteLength < NUMBER_CARDS);  // temp till bug is found
    for (int idx = 0; idx != wasteLength; idx++) {
      int cardNumber = waste[idx];

      renderer->faceUp(cardNumber);
      int staggerOrder = std::max(idx - wasteLength + CARDS_TO_DRAW, 0);
      int delay = staggerOrder * WASTE_DRAW_STAGGER * ANIMATION_TEST_SLOWDOWN;

      int position = idx - (wasteLength - std::min(CARDS_TO_DRAW, wasteLength));
      if (position < 0) {
        position = 0;
      }
      _placeCard(cardNumber, WASTE_X + WASTE_X_SPACING * position, WASTE_Y,
                 idx == wasteLength - 1, delay);
    }

    // Position foundation cards.
    auto &foundations = gameState->getFoundations();
    for (int foundationIdx = 0; foundationIdx != NUMBER_FOUNDATIONS;
         foundationIdx++) {
      auto &foundation = foundations[foundationIdx];
      int foundationLength = foundation.size();

      for (int position = 0; position < foundationLength; position++) {
        int cardNumber = foundation[position];
        renderer->faceUp(cardNumber);
        if (position == foundationLength - 1) {
          std::list<int> cards{cardNumber};
        }
        _placeCard(cardNumber,
                   FOUNDATION_X + FOUNDATION_X_SPACING * foundationIdx,
                   FOUNDATION_Y, true, 0);
      }
    }

    // Position tableau cards.
    auto &tableausFaceDown = gameState->getTableausFaceDown();
    auto &tableausFaceUp = gameState->getTableausFaceUp();
    for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
      float position = TABLEAU_Y;
      auto &tableauFaceDown = tableausFaceDown[tableauIdx];
      int faceDownLength = tableauFaceDown.size();
      auto &tableauFaceUp = tableausFaceUp[tableauIdx];
      int faceUpLength = tableauFaceUp.size();
      float tableauYSpacingFaceDown;
      float tableauYSpacingFaceUp;
      float tableauHeight = renderer->getTargetHeight() - TABLEAU_BOTTOM_MARGIN;
      float availableForFaceUp =
          tableauHeight - faceDownLength * TABLEAU_Y_MIN_SPACING_FACE_DOWN;

      if (availableForFaceUp / faceUpLength > TABLEAU_Y_MAX_SPACING_FACE_UP) {
        tableauYSpacingFaceUp = TABLEAU_Y_MAX_SPACING_FACE_UP;
      } else {
        tableauYSpacingFaceUp = availableForFaceUp / faceUpLength;
      }
      float availableForFaceDown =
          tableauHeight - faceUpLength * tableauYSpacingFaceUp;
      if (availableForFaceDown / faceDownLength >
          TABLEAU_Y_MAX_SPACING_FACE_DOWN) {
        tableauYSpacingFaceDown = TABLEAU_Y_MAX_SPACING_FACE_DOWN;
      } else {
        tableauYSpacingFaceDown = availableForFaceDown / faceDownLength;
      }

      for (int cardNumber: tableauFaceDown) {
        _placeCard(cardNumber, TABLEAU_X + TABLEAU_X_SPACING * tableauIdx,
                   position, false, 0);
        renderer->faceDown(cardNumber);
        position += tableauYSpacingFaceDown;
      }

      for (int cardNumber: tableauFaceUp) {
        _placeCard(cardNumber, TABLEAU_X + TABLEAU_X_SPACING * tableauIdx,
                   position, true, 0);
        renderer->faceUp(cardNumber);
        position += tableauYSpacingFaceUp;
      }
    }
  }

  void _tryAutoPlay() {
    if (timeLastRenderered > getTimeNow() - 150) {
      return;
    }

    long long timeNow = getTimeNow();
    auto &tableausFaceDown = gameState->getTableausFaceDown();
    auto &tableausFaceUp = gameState->getTableausFaceUp();
    auto &stock = gameState->getStock();
    auto &waste = gameState->getWaste();
    // Auto play
    if (stock.empty() && waste.empty()) {
      auto actionsFor = gameState->getActions();
      bool anyFaceDown = false;
      for (int tableauIdx = 0; tableauIdx != NUMBER_TABLEAUS; tableauIdx++) {
        if (!tableausFaceDown[tableauIdx].empty()) {
          anyFaceDown = true;
          break;
        }
      }
      if (!anyFaceDown) {
        for (auto &tableau : tableausFaceUp) {
          if (tableau.empty()) {
            continue;
          }
          for (auto &action : actionsFor[tableau.back()]) {
            if (action.moveType == MOVE_TYPE::TO_TABLEAU) {
              continue;
            }
            gameState->execute(action);
            //GameStore.store(gameState);
            timeLastRenderered = LONG_LONG_MAX;
            render();
            return;
          }
        }
      }
    }
  }

  void _placeCard(int cardNumber, float x, float y, bool draggable, int delay) {
    long long timeNow = getTimeNow();
    renderer->raiseCard(cardNumber);
    renderer->setDraggable(cardNumber, false);

    glm::vec3 position = renderer->getCardPosition(cardNumber);

    float deltaX = position[0] - x;
    float deltaY = position[1] - y;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    float flyDistance = std::min(distance, (float) FLY_DISTANCE_MAX);
    float flyHeight = FLY_HEIGHT * flyDistance / FLY_DISTANCE_MAX;
    float animationDistance = std::min(distance, (float) ANIMATION_DISTANCE_MAX);

    long animationTime = ANIMATION_TEST_SLOWDOWN *
                         (int) (ANIMATION_TIME * animationDistance
                                / ANIMATION_DISTANCE_MAX +
                                ANIMATION_TIME_SUPPLEMENT);

    Curve curve;
    curve.startTime = timeNow + delay;
    curve.endTime = timeNow + animationTime + delay;
    curve.start = position;
    curve.flyHeight = flyHeight;
    curve.endX = x;
    curve.endY = y;
    curve.draggable = draggable;
    curves[cardNumber] = curve;
  }

  std::list<int> startDrag(int card) override {
    auto cards = gameState->getStack(card);
    riseStarted = getTimeNow();
    raisingCards = cards;
    return cards;
  }

  void cardClickedOrDropped(int card, bool click) override {
    std::list<int> cards = gameState->getStack(card);
    int cardNumber = cards.front();
    if (lastCardMoved != cardNumber) {
      cardHistory.clear();
      lastCardMoved = cardNumber;
    }
    std::map<int, std::set<Action>> actionsFor = gameState->getActions();
    std::set<Action> actions = actionsFor[cardNumber];

    if (!actions.empty()) {
      // if click ... priority is (age-> usefulness -> proximity)
      // otherwise it is proximity
      if (click) {
        // Filter actions to oldest actions.
        long long oldest = LLONG_MAX;
        std::set<Action> oldestActions;
        for (const Action &action : actions) {
          long long time =
              cardHistory.count(action) ? cardHistory[action] : LLONG_MIN;
          if (time == oldest) {
            oldestActions.insert(action);
          } else if (time < oldest) {
            oldest = time;
            oldestActions = {action};
          }
        }
        if (!oldestActions.empty()) {
          actions = oldestActions;
        }

        // Filter actions to most useful actions.
        int mostUseful = INT_MIN;
        std::set<Action> mostUsefulActions;
        for (const Action &action : actions) {
          auto useful = action.moveType;
          if (useful == mostUseful) {
            mostUsefulActions.insert(action);
          } else if (useful > mostUseful) {
            mostUseful = useful;
            mostUsefulActions = {action};
          }
        }
        if (!mostUsefulActions.empty()) {
          actions = mostUsefulActions;
        }
      }

      // Find closet action.
      auto &tableausFaceUp = gameState->getTableausFaceUp();
      auto &tableausFaceDown = gameState->getTableausFaceDown();
      auto &position = renderer->getCardPosition(cardNumber);
      double closest = DBL_MAX;
      const Action *closestAction = nullptr;
      for (const Action &action : actions) {
        if (cards.size() == 1 || action.moveType == MOVE_TYPE::TO_TABLEAU) {
          float x;
          float y;
          if (action.moveType == MOVE_TYPE::TO_TABLEAU) {
            x = TABLEAU_X + TABLEAU_X_SPACING * action.destinationIdx;
            y = TABLEAU_Y + tableausFaceUp[action.destinationIdx].size()
                            * TABLEAU_Y_MAX_SPACING_FACE_DOWN +
                tableausFaceDown[action.destinationIdx].size()
                * TABLEAU_Y_MAX_SPACING_FACE_UP;
          } else {
            assert (action.moveType == MOVE_TYPE::TO_FOUNDATION);
            x = FOUNDATION_X + FOUNDATION_X_SPACING * action.destinationIdx;
            y = FOUNDATION_Y;
          }

          auto distance = pow(position[0] - x, 2) + pow(position[1] - y, 2);
          if (distance < closest) {
            closest = distance;
            closestAction = &action;
          }
        }
      }
      if (closestAction) {
        cardHistory[*closestAction] = getTimeNow();
        gameState->execute(*closestAction);
        //GameStore.store(gameState);
      }
    }
    render();
  }
};

GameController *newGameController(Renderer *renderer, GameState *gameState) {
  return new LocalGameController(renderer, gameState);
}