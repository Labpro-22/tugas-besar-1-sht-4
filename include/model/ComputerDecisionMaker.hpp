#pragma once

#include <string>

using namespace std;

class ComputerDecisionMaker {
public:
    static int randomInt(int min, int max);
    static bool rollThreshold(int threshold);

    static string decideCommand();
    static bool decideToBuy();
    static int decideIncomeTax();
    static int decideFestival(int maxIndex);
    static string decideAuctionAction(int minimumBid, int playerMoney, bool forcedBid);
    static int decideLiquidation(int maxIndex);
    static int decideForceDropChoice(int maxIndex);
    static int decideAbilityCard(int maxIndex);
    static int decideBuildGroup(int maxIndex);
    static int decideBuildTile(int maxIndex);
    static int decideRedeemChoice(int maxIndex);
    static int decideMortgageChoice(int maxIndex);
    static int decideJailChoice();
    static int decideLassoTarget(int maxIndex);
    static int decideDemolitionTarget(int maxIndex);
};
