#include "model/cards/DiscountCard.hpp"

using namespace std;

DiscountCard::DiscountCard() : HandCard(), discountPercent(0), duration(0) {}

DiscountCard::DiscountCard(const string& name, const string& description, bool used, int discountPercent, int duration)
    : HandCard(name, description, used), discountPercent(discountPercent), duration(duration) {}

DiscountCard::DiscountCard(const DiscountCard& other)
    : HandCard(other), discountPercent(other.discountPercent), duration(other.duration) {}

DiscountCard::~DiscountCard() {}

DiscountCard& DiscountCard::operator=(const DiscountCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        discountPercent = other.discountPercent;
        duration = other.duration;
    }
    return *this;
}

int DiscountCard::getDiscountPercent() const { return discountPercent; }
int DiscountCard::getDuration() const { return duration; }

void DiscountCard::apply(Game& game, Player& player) {}
