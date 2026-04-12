#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class Game;
class Player;

class FreeParkingTile : public SpecialTile {
public:
    FreeParkingTile();
    FreeParkingTile(int index, const string& code, const string& name);
    FreeParkingTile(const FreeParkingTile& other);
    ~FreeParkingTile();
    FreeParkingTile& operator=(const FreeParkingTile& other);

    void onLand(Game& game, Player& player) override;
};