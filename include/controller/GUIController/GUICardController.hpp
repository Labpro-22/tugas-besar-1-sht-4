#pragma once

#include "view/raylib/GuiTypes.hpp"

#include <vector>

class GUIGameController;

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
    GUIGameController& controller_;
};
