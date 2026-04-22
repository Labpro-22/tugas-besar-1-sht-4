#pragma once

#include <memory>

using namespace std;

class Game;
class UIManager;
class Player;

class CardController {
private:
    Game& game;
    UIManager& uiManager;

public:
    CardController(Game& game, UIManager& uiManager);
    CardController(const CardController& other);
    ~CardController();
    CardController& operator=(const CardController& other);

    void drawAndApplyChanceCard(Player& player);
    void drawAndApplyCommunityChestCard(Player& player);
    void useHandCard(Player& player, int cardIndex);
};
