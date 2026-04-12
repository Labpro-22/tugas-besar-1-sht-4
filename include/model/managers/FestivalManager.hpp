#pragma once

using namespace std;

class Game;
class Player;
class StreetTile;

class FestivalManager {
public:
    FestivalManager();
    FestivalManager(const FestivalManager& other);
    ~FestivalManager();
    FestivalManager& operator=(const FestivalManager& other);

    void activateFestival(Game& game, Player& player, StreetTile& tile);
    void strengthenFestival(StreetTile& tile);
    void decrementFestivalDurations(Player& owner);
    void clearExpiredFestival(StreetTile& tile);
};