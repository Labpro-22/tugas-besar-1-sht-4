#include "controller/GUIController/GUITileController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

using namespace view::raylibgui;

GUITileController::GUITileController(GUIGameController& controller)
    : controller_(controller) {}

int GUITileController::getMortgageValue(const TileInfo& tile) const {
    return controller_.getMortgageValue(tile);
}

int GUITileController::getRedeemCost(const TileInfo& tile) const {
    return controller_.getRedeemCost(tile);
}

int GUITileController::findJailIndex() const {
    return controller_.findJailIndex();
}

int GUITileController::computeRent(const TileInfo& tile) const {
    return controller_.computeRent(tile);
}

int GUITileController::computeTileAssetValue(const TileInfo& tile) const {
    return controller_.computeTileAssetValue(tile);
}

Rectangle GUITileController::boardTileRect(Rectangle square, int index) const {
    return controller_.boardTileRect(square, index);
}

void GUITileController::setSelectedTile(int tileIndex) {
    controller_.setSelectedTile(tileIndex);
}

void GUITileController::openTileDetail(int tileIndex) {
    controller_.openTileDetail(tileIndex);
}

void GUITileController::openPurchase(int tileIndex) {
    controller_.openPurchase(tileIndex);
}

void GUITileController::openAuctionForTile(int tileIndex) {
    controller_.openAuctionForTile(tileIndex);
}

void GUITileController::openIncomeTax() {
    controller_.openIncomeTax();
}

void GUITileController::openLuxuryTax() {
    controller_.openLuxuryTax();
}

void GUITileController::openFestival() {
    controller_.openFestival();
}

void GUITileController::openJail() {
    controller_.openJail();
}

void GUITileController::openBuild() {
    controller_.openBuild();
}

void GUITileController::openMortgage() {
    controller_.openMortgage();
}

void GUITileController::openRedeem() {
    controller_.openRedeem();
}

void GUITileController::openLiquidation() {
    controller_.openLiquidation();
}

void GUITileController::triggerTileEvent(int tileIndex, bool fromMovement) {
    controller_.triggerTileEvent(tileIndex, fromMovement);
}

bool GUITileController::canCurrentPlayerAffordSelectedPurchase() const {
    return controller_.canCurrentPlayerAffordSelectedPurchase();
}

int GUITileController::currentPurchasePrice() const {
    return controller_.currentPurchasePrice();
}

void GUITileController::buySelectedProperty() {
    controller_.buySelectedProperty();
}

void GUITileController::skipSelectedPurchase() {
    controller_.skipSelectedPurchase();
}

void GUITileController::auctionRaiseBid(int amount) {
    controller_.auctionRaiseBid(amount);
}

void GUITileController::auctionPass() {
    controller_.auctionPass();
}

void GUITileController::finalizeAuction() {
    controller_.finalizeAuction();
}

int GUITileController::flatIncomeTax() const {
    return controller_.flatIncomeTax();
}

int GUITileController::percentageIncomeTax() const {
    return controller_.percentageIncomeTax();
}

void GUITileController::payIncomeTax(bool useFlatTax) {
    controller_.payIncomeTax(useFlatTax);
}

int GUITileController::luxuryTaxAmount() const {
    return controller_.luxuryTaxAmount();
}

void GUITileController::payLuxuryTax() {
    controller_.payLuxuryTax();
}

void GUITileController::activateFestivalOnSelectedTile() {
    controller_.activateFestivalOnSelectedTile();
}

void GUITileController::payJailFine() {
    controller_.payJailFine();
}

void GUITileController::buildOnSelectedTile() {
    controller_.buildOnSelectedTile();
}

void GUITileController::mortgageSelectedTile() {
    controller_.mortgageSelectedTile();
}

void GUITileController::redeemSelectedTile() {
    controller_.redeemSelectedTile();
}

void GUITileController::liquidateSelectedTile() {
    controller_.liquidateSelectedTile();
}

void GUITileController::declareBankrupt() {
    controller_.declareBankrupt();
}

std::vector<int> GUITileController::currentPlayerStreetOptions() const {
    return controller_.currentPlayerStreetOptions();
}

std::vector<int> GUITileController::currentPlayerBuildOptions() const {
    return controller_.currentPlayerBuildOptions();
}

std::vector<int> GUITileController::currentPlayerMortgageOptions() const {
    return controller_.currentPlayerMortgageOptions();
}

std::vector<int> GUITileController::currentPlayerRedeemOptions() const {
    return controller_.currentPlayerRedeemOptions();
}
