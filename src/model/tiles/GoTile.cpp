#include "model/tiles/GoTile.hpp"

using namespace std;

GoTile::GoTile() {}

GoTile::GoTile(int index, const string& code, const string& name, int salary)
    : SpecialTile(index, code, name), salary(salary)
{}

GoTile::GoTile(const GoTile& other)
    : SpecialTile(other), salary(other.salary)
{}

GoTile::~GoTile() {}

GoTile& GoTile::operator=(const GoTile& other) {
    if (this != &other) {
        SpecialTile::operator=(other);
        this->salary = other.salary;
    }
    return *this;
}

Tile::TileType GoTile::onLand() const {
    return TileType::Go;
}

int GoTile::getSalary() const {
    return salary;
}