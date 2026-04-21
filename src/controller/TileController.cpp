#include "controller/TileController.hpp"

using namespace std;

TileController::TileController(Game& game, UIManager& uiManager)
    : game(game), uiManager(uiManager) {}

TileController::TileController(const TileController& other)
    : game(other.game), uiManager(other.uiManager) {}

TileController::~TileController() {}

TileController& TileController::operator=(const TileController& other) {
    return *this;
}

void TileController::resolveLanding(Tile& tile, Player& player) {}

void TileController::handleStreetPurchase(StreetTile& tile) {}

void TileController::handleAuction(StreetTile& tile, Player* triggerPlayer) {}

void TileController::handleIncomeTax(IncomeTaxTile& tile) {}

void TileController::handleLuxuryTax(LuxuryTaxTile& tile) {}

void TileController::handleFestival(FestivalTile& tile) {}

void TileController::handleForceDrop(Player& player) {}

void TileController::handleJailTurn(Player& player) {}

void TileController::handleBankruptcy(Player& player) {}