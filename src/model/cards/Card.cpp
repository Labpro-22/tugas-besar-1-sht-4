#include "model/cards/Card.hpp"

using namespace std;

Card::Card() {}

Card::Card(const string& name, const string& description) : name(name), description(description) {}

Card::Card(const Card& other) : name(other.name), description(other.description){}

Card::~Card() {}

Card& Card::operator=(const Card& other) {
    if(this != &other){
        name = other.name;
        description = other.description;
    }
    return *this;
}

string Card::getName() const {
    return name;
}