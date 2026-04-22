#pragma once

#include "model/cards/HandCard.hpp"

using namespace std;

class Game;
class Player;

class DemolitionCard : public HandCard {
public:
    DemolitionCard();
    DemolitionCard(const string& name, const string& description, bool used);
    DemolitionCard(const DemolitionCard& other);
    ~DemolitionCard();
    DemolitionCard& operator=(const DemolitionCard& other);

    void setTargetStreet(class StreetTile* street);
    void apply(Game& game, Player& player) override;
private:
    class StreetTile* targetStreet;
};