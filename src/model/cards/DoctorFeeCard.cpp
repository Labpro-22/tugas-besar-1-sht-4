#include "model/cards/DoctorFeeCard.hpp"

using namespace std;

DoctorFeeCard::DoctorFeeCard() {}

DoctorFeeCard::DoctorFeeCard(const string& name, const string& description, int fee) {}

DoctorFeeCard::DoctorFeeCard(const DoctorFeeCard& other) {}

DoctorFeeCard::~DoctorFeeCard() {}

DoctorFeeCard& DoctorFeeCard::operator=(const DoctorFeeCard& other) {
    return *this;
}

void DoctorFeeCard::apply(Game& game, Player& player) {}