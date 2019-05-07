#include "Rules.h"
#include "CardList.h"

enum MOVE_TYPE {
    DRAW,
    TO_TABLEU,
    TO_FOUNDATION,
};

class GameState {
private:
  CardList deck;
  CardList stock;
  CardList waste;

public:
  void newGame(const Rules& rules) {
      //tableausFaceDown = [];
      //tableausFaceUp = [];
      //this.foundations = [];
      //this.rules = rules;

      // Add cards to deck
      for (int idx = 0; idx != Rules::NUMBER_CARDS; idx++) {
        deck.add(idx);
      }

      deck.shuffle();

//      // Tableaus.
//      for (let tableau = 0; tableau !== Rules.NUMBER_TABLEAUS; tableau++) {
//        this.tableausFaceDown[tableau] = new CardList();
//        for (let position = 0; position <= tableau - 1; position++) {
//          this.tableausFaceDown[tableau].add(this.deck.pop());
//        }
//        this.tableausFaceUp[tableau] = new CardList();
//        this.tableausFaceUp[tableau].add(this.deck.pop());
//      }
//
//      // Stock.
//      while (this.deck.length() > 0) {
//        this.stock.add(this.deck.pop());
//      }
//
//      // Foundations
//      for (let idx = 0; idx !== Rules.NUMBER_FOUNDATIONS; idx++) {
//        this.foundations[idx] = new CardList();
//      }

  }
//
//  _draw() {
//    if (this.stock.length() === 0) {
//      while (this.waste.length()) {
//        const drawn = this.waste.pop();
//        this.stock.add(drawn);
//      }
//    } else {
//      // X cards from stock to waste.
//      for (let idx = 0; idx !== this.rules.cardsToDraw && this.stock.length(); idx++) {
//        const drawn = this.stock.pop();
//        this.waste.add(drawn);
//      }
//    }
//  }
//
//  remove(cardNumber) {
//      // In tableau cards?
//      for (let tableauIdx = 0; tableauIdx !== Rules.NUMBER_TABLEAUS; tableauIdx++) {
//        const tableau = this.tableausFaceUp[tableauIdx];
//        if (tableau.remove(cardNumber)) {
//          // Reveal undercard if needed.
//          if (tableau.length() === 0) {
//            const tableauFaceDown = this.tableausFaceDown[tableauIdx];
//            if (tableauFaceDown.length() > 0) {
//              tableau.pushFront(tableauFaceDown.pop());
//            }
//          }
//          return true;
//        }
//      }
//      // In stock cards?
//      if (this.stock.remove(cardNumber)) {
//        return true;
//      }
//
//      // In waste cards?
//      if (this.waste.remove(cardNumber)) {
//        return true;
//      }
//
//      // Foundations
//      for (let idx = 0; idx !== Rules.NUMBER_FOUNDATIONS; idx++) {
//        if (this.foundations[idx].remove(cardNumber)) {
//          return true;
//        }
//      }
//
//      return false;
//  }
//
//  stackedOn(cardNumber) {
//      // In tableau cards?
//      for (let tableauIdx = 0; tableauIdx !== Rules.NUMBER_TABLEAUS; tableauIdx++) {
//        const tableau = this.tableausFaceUp[tableauIdx];
//        const idx = tableau.indexOf(cardNumber);
//        if (idx !== -1 && idx < tableau.length() - 1) {
//          return tableau.get(idx + 1);
//        }
//      }
//      return null;
//  }
//
//  _moveToTableau(cardNumber, tableauIdx) {
//    let movingCard = cardNumber;
//
//    do {
//      const stackedOn = this.stackedOn(movingCard);
//      if (this.remove(movingCard)) {
//        this.tableausFaceUp[tableauIdx].add(movingCard);
//      }
//      movingCard = stackedOn;
//    } while (movingCard !== null);
//  }
//
//  _moveToFoundation(cardNumber, foundationIdx) {
//    if (this.remove(cardNumber)) {
//      this.foundations[foundationIdx].add(cardNumber);
//    }
//  }
//
//  execute(action) {
//      if (action.moveType === MOVE_TYPE.DRAW) {
//        this._draw();
//      } else if (action.moveType === MOVE_TYPE.TO_TABLEU) {
//        this._moveToTableau(action.card, action.destinationIdx);
//      } else if (action.moveType === MOVE_TYPE.TO_FOUNDATION) {
//        this._moveToFoundation(action.card, action.destinationIdx);
//      }
//  }
//
//  getMoveableCards() {
//    const movableCards = new Set();
//
//    const wasteLength = this.waste.length();
//    if (wasteLength !== 0) {
//      const cardNumber = this.waste.get(wasteLength - 1);
//      movableCards.add(cardNumber);
//    }
//
//    for (let foundationIdx = 0; foundationIdx !== Rules.NUMBER_FOUNDATIONS; foundationIdx++) {
//      const foundation = this.foundations[foundationIdx];
//      const foundationLength = foundation.length();
//      if (foundationLength !== 0) {
//        const cardNumber = foundation.get(foundationLength - 1);
//        movableCards.add(cardNumber);
//      }
//    }
//
//    for (let tableauIdx = 0; tableauIdx !== Rules.NUMBER_TABLEAUS; tableauIdx++) {
//      const tableau = this.tableausFaceUp[tableauIdx];
//      const tableauLength = tableau.length();
//      for (let position = 0; position < tableauLength; position++) {
//        const cardNumber = tableau.get(position);
//        movableCards.add(cardNumber);
//      }
//    }
//    return movableCards;
//  }
//
//  isComplete() {
//    for (let foundationIdx = 0; foundationIdx !== Rules.NUMBER_FOUNDATIONS; foundationIdx++) {
//      if (this.foundations[foundationIdx].length() !== Rules.NUMBER_CARDS_IN_SUIT) {
//        return false;
//      }
//    }
//    return true;
//  }
//
//  getActions() {
//    const actionsFor = new Map();
//    const movableCards = this.getMoveableCards();
//
//    function addAction(action) {
//
//      const card = action.card;
//      if (!movableCards.has(card)) {
//        return;
//      }
//      let actions;
//      if (actionsFor.has(card)) {
//        actions = actionsFor.get(card)
//      } else {
//        actions = new Set();
//        actionsFor.set(card, actions);
//      }
//      actions.add(action);
//    }
//
//    for (let foundationIdx = 0; foundationIdx !== Rules.NUMBER_FOUNDATIONS; foundationIdx++) {
//      const foundation = this.foundations[foundationIdx];
//      const foundationLength = foundation.length();
//      let canPlaceOn;
//      if (foundationLength === 0) {
//        // Empty foundation ... will take Aces
//        canPlaceOn = [Rules.getCard(0, Rules.ACE_TYPE), Rules.getCard(1, Rules.ACE_TYPE),
//            Rules.getCard(2, Rules.ACE_TYPE), Rules.getCard(3, Rules.ACE_TYPE)];
//      } else {
//        const cardNumber = foundation.get(foundationLength - 1);
//        canPlaceOn = Rules.canPlaceOnInFoundation(cardNumber);
//      }
//      for (const other of canPlaceOn) {
//        addAction({
//                      card: other,
//                      moveType: MOVE_TYPE.TO_FOUNDATION,
//                      destinationIdx: foundationIdx
//                  });
//      }
//    }
//
//    // Position tableau cards.
//    for (let tableauIdx = 0; tableauIdx !== Rules.NUMBER_TABLEAUS; tableauIdx++) {
//      const tableau = this.tableausFaceUp[tableauIdx];
//      const tableauLength = tableau.length();
//      let canPlaceOn;
//      if (tableauLength === 0) {
//        // Empty tableau ... will take Kings
//        canPlaceOn = [Rules.getCard(0, Rules.KING_TYPE), Rules.getCard(1, Rules.KING_TYPE),
//            Rules.getCard(2, Rules.KING_TYPE), Rules.getCard(3, Rules.KING_TYPE)];
//      } else {
//        const cardNumber = tableau.get(tableauLength - 1);
//        canPlaceOn = Rules.canPlaceOnInTableau(cardNumber);
//      }
//      for (const other of canPlaceOn) {
//        addAction({
//                      card: other,
//                      moveType: MOVE_TYPE.TO_TABLEU,
//                      destinationIdx: tableauIdx,
//                  });
//      }
//    }
//    return actionsFor;
//  }
//
//  getAllActions() {
//    const actionsFor = this.getActions();
//    const actions = new Set();
//    actions.add({
//                    moveType: MOVE_TYPE.DRAW,
//                });
//    for (const entries of actionsFor.values()) {
//      for (const action of entries) {
//        actions.add(action);
//      }
//    }
//    return actions;
//  }
//
//
//  normalKey() {
//    const tableauStrings = [];
//    for (let tableauIdx = 0; tableauIdx !== Rules.NUMBER_TABLEAUS; tableauIdx++) {
//      tableauStrings.push(JSON.stringify(this.tableausFaceDown[tableauIdx].cards) +
//                          JSON.stringify(this.tableausFaceUp[tableauIdx].cards));
//    }
//    tableauStrings.sort();
//    return JSON.stringify(tableauStrings) + JSON.stringify(this.stock.cards) + JSON.stringify(this.waste.cards);
//  }

};
