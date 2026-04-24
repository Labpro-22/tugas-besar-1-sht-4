#pragma once

#include "view/raylib/GuiTypes.hpp"

#include <vector>

class GUIGameController;
class Player;

class GUICardController {
public:
    explicit GUICardController(GUIGameController& controller);

    void openCards();
    void openRandomCardDraw(int deckKey);
    void useSelectedHandCard();
    void storeDrawnCard();
    void applyDrawnCard();
    void dropSelectedHandCard();
    void useJailCard();

    bool currentPlayerNeedsForceDrop() const;
    void maybeOpenForceDrop();

    view::raylibgui::CardInfo pickRandomCard(int deckKey);
    std::vector<view::raylibgui::CardInfo> chanceDeck() const;
    std::vector<view::raylibgui::CardInfo> communityDeck() const;

private:
    friend class GUIGameController;
    friend class GUICommandController;
    friend class GUITileController;

    GUIGameController& controller_;

    void closeCardDrawOverlay(bool discardPendingCard);
    void clearPendingDrawnCard(bool discardPendingCard);
    void discardAllCards(Player& player);
    void configureSelectedHandCard(Player& player, int cardIndex);
};
