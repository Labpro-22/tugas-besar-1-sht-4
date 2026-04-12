#pragma once

#include "model/cards/CommunityChestCard.hpp"

using namespace std;

class Game;
class Player;

class DoctorFeeCard : public CommunityChestCard {
private:
    int fee;

public:
    DoctorFeeCard();
    DoctorFeeCard(const string& name, const string& description, int fee);
    DoctorFeeCard(const DoctorFeeCard& other);
    ~DoctorFeeCard();
    DoctorFeeCard& operator=(const DoctorFeeCard& other);

    void apply(Game& game, Player& player) override;
};