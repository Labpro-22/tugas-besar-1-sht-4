#include "model/cards/DiscountCard.hpp"

using namespace std;

DiscountCard::DiscountCard() {}

DiscountCard::DiscountCard(const string& name, const string& description, bool used, int discountPercent, int duration) {}

DiscountCard::DiscountCard(const DiscountCard& other) {}

DiscountCard::~DiscountCard() {}

DiscountCard& DiscountCard::operator=(const DiscountCard& other) {
    return *this;
}

void DiscountCard::apply(Game& game, Player& player) {}