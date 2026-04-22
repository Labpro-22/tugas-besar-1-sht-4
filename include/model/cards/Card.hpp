#pragma once

#include <string>

using namespace std;

class Game;
class Player;

class Card {
protected:
    string name;
    string description;

public:
    Card();
    Card(const string& name, const string& description);
    Card(const Card& other);
    virtual ~Card();
    Card& operator=(const Card& other);

    virtual void apply(Game& game, Player& player) = 0;
    string getName() const;
    string getDescription() const;
};