#include "model/ComputerDecisionMaker.hpp"

#include <algorithm>
#include <random>

using namespace std;

static mt19937& rng() {
    static mt19937 gen(random_device{}());
    return gen;
}

int ComputerDecisionMaker::randomInt(int min, int max) {
    if (min > max) return min;
    uniform_int_distribution<int> dist(min, max);
    return dist(rng());
}

bool ComputerDecisionMaker::rollThreshold(int threshold) {
    return randomInt(1, 100) > threshold;
}

string ComputerDecisionMaker::decideCommand() {
    return "LEMPAR_DADU";
}

bool ComputerDecisionMaker::decideToBuy() {
    return rollThreshold(50);
}

int ComputerDecisionMaker::decideIncomeTax() {
    return randomInt(1, 2);
}

int ComputerDecisionMaker::decideFestival(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return randomInt(1, maxIndex);
}

string ComputerDecisionMaker::decideAuctionAction(int minimumBid, int playerMoney, bool forcedBid) {
    const int minBid = max(0, minimumBid);
    if (playerMoney < minBid) return "PASS";
    if (!forcedBid && randomInt(1, 2) == 1) return "PASS";
    return "BID " + to_string(randomInt(minBid, playerMoney));
}

int ComputerDecisionMaker::decideLiquidation(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return randomInt(1, maxIndex);
}

int ComputerDecisionMaker::decideForceDropChoice(int maxIndex) {
    if (maxIndex <= 0) return 1;
    return randomInt(1, maxIndex);
}

int ComputerDecisionMaker::decideAbilityCard(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return randomInt(1, maxIndex);
}

int ComputerDecisionMaker::decideBuildGroup(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return randomInt(1, maxIndex);
}

int ComputerDecisionMaker::decideBuildTile(int maxIndex) {
    if (maxIndex <= 0) return 1;
    return randomInt(1, maxIndex);
}

int ComputerDecisionMaker::decideRedeemChoice(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return rollThreshold(50) ? randomInt(1, maxIndex) : 0;
}

int ComputerDecisionMaker::decideMortgageChoice(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return rollThreshold(50) ? randomInt(1, maxIndex) : 0;
}

int ComputerDecisionMaker::decideJailChoice() {
    return randomInt(1, 2);
}

int ComputerDecisionMaker::decideLassoTarget(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return randomInt(1, maxIndex);
}

int ComputerDecisionMaker::decideDemolitionTarget(int maxIndex) {
    if (maxIndex <= 0) return 0;
    return randomInt(1, maxIndex);
}
