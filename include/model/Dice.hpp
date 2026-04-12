#pragma once

using namespace std;

class Dice {
private:
    int die1;
    int die2;
    bool manualMode;

public:
    Dice();
    Dice(int die1, int die2, bool manualMode);
    Dice(const Dice& other);
    ~Dice();
    Dice& operator=(const Dice& other);

    void roll();
    void setManual(int x, int y);
    int getDie1() const;
    int getDie2() const;
    int getTotal() const;
    bool isDouble() const;
};