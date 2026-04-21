#pragma once

class RentContext {
private:
    bool ownerHasColorGroup;
    int railroadRent;
    int utilityMultiplier;
    int diceTotal;

public:
    RentContext();
    RentContext(
        bool ownerHasColorGroup,
        int railroadRent,
        int utilityMultiplier,
        int diceTotal
    );
    RentContext(const RentContext& other);
    ~RentContext();
    RentContext& operator=(const RentContext& other);

    bool getOwnerHasColorGroup() const;
    int getRailroadRent() const;
    int getUtilityMultiplier() const;
    int getDiceTotal() const;

    void setOwnerHasColorGroup(bool ownerHasColorGroup);
    void setRailroadRent(int railroadRent);
    void setUtilityMultiplier(int utilityMultiplier);
    void setDiceTotal(int diceTotal);
};
