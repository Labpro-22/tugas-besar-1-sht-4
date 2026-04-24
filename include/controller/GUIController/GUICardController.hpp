#pragma once

#include "controller/GUIController/GUIControllerContext.hpp"

#include <vector>

class Player;

class GUICardController {
public:
    explicit GUICardController(GUIControllerContext& context);

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
    GUIControllerContext& controller_;

    void closeCardDrawOverlay(bool discardPendingCard);
    void clearPendingDrawnCard(bool discardPendingCard);
    void discardAllCards(Player& player);
    void configureSelectedHandCard(Player& player, int cardIndex);
};
