#include "controller/CardController.hpp"

#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/Board.hpp"
#include "model/cards/ChanceCard.hpp"
#include "model/cards/CommunityChestCard.hpp"
#include "model/cards/HandCard.hpp"
#include "model/cards/TeleportCard.hpp"
#include "model/cards/LassoCard.hpp"
#include "model/cards/DemolitionCard.hpp"
#include "model/tiles/StreetTile.hpp"
#include "view/UIManager.hpp"

#include <vector>

using namespace std;

CardController::CardController(Game& game, UIManager& uiManager)
    : game(game), uiManager(uiManager) {}

CardController::CardController(const CardController& other)
    : game(other.game), uiManager(other.uiManager) {}

CardController::~CardController() {}

CardController& CardController::operator=(const CardController& other) {
    if (this != &other) {
    }
    return *this;
}

void CardController::drawAndApplyChanceCard(Player& player) {
    shared_ptr<ChanceCard> card = game.getCardManager().drawChanceCard();
    if (card != nullptr) {
        uiManager.printCardDrawn("Kartu Kesempatan", card->getName(), card->getDescription());
        card->apply(game, player);
    }
}

void CardController::drawAndApplyCommunityChestCard(Player& player) {
    shared_ptr<CommunityChestCard> card = game.getCardManager().drawCommunityChestCard();
    if (card != nullptr) {
        uiManager.printCardDrawn("Kartu Dana Umum", card->getName(), card->getDescription());
        card->apply(game, player);
    }
}

void CardController::useHandCard(Player& player, int cardIndex) {
    vector<shared_ptr<HandCard>> cards = game.getCardManager().getHandCards(player);
    if (cardIndex < 0 || cardIndex >= static_cast<int>(cards.size())) return;
    
    shared_ptr<HandCard> targetCard = cards[cardIndex];
    if (targetCard == nullptr) return;

    uiManager.printCardEffect("Menggunakan Kartu Kemampuan Spesial: " + targetCard->getName() + "...");

    if (auto teleport = dynamic_cast<TeleportCard*>(targetCard.get())) {
        Board& board = game.getBoard();
        int boardSize = board.getBoardSize();
        uiManager.printMessage("Daftar petak yang tersedia:");
        for (int i = 0; i < boardSize; i++) {
            auto tile = board.getTile(i);
            if (tile) {
                uiManager.printMessage("  [" + tile->getCode() + "] " + tile->getName());
            }
        }

        string targetCode;
        int targetIndex = -1;
        while (targetIndex < 0) {
            targetCode = uiManager.readPropertyCode();
            targetIndex = board.getTileIndex(targetCode);
            if (targetIndex < 0) {
                uiManager.printError("Kode petak tidak valid. Coba lagi.");
            }
        }
        uiManager.printMessage("Bidak dipindahkan ke " + board.getTile(targetIndex)->getName() + " (" + targetCode + ").");
        teleport->setTargetTileIndex(targetIndex);
    } 
    else if (auto lasso = dynamic_cast<LassoCard*>(targetCard.get())) {
        int myPos = player.getPosition();
        vector<Player*> targets;
        int boardSize = game.getBoard().getBoardSize();

        for (Player& other : game.getPlayers()) {
            if (other.getUsername() == player.getUsername()) continue;
            if (other.isBankrupt()) continue;

            int otherPos = other.getPosition();
            bool isAhead = (otherPos > myPos) ||
                           (otherPos < myPos && (myPos - otherPos) > boardSize / 2);
            if (isAhead) {
                targets.push_back(&other);
            }
        }

        if (targets.empty()) {
            uiManager.printMessage("Tidak ada lawan yang berada di depan posisi Anda. LassoCard tidak berpengaruh.");
            return;
        }

        vector<string> names;
        vector<int> positions;
        for (Player* target : targets) {
            names.push_back(target->getUsername());
            positions.push_back(target->getPosition());
        }

        int choice = uiManager.readLassoTarget(names, positions);
        if (choice == 0) {
            uiManager.printMessage("LassoCard dibatalkan.");
            return;
        }
        Player* targetPlayer = targets[choice - 1];
        uiManager.printMessage(targetPlayer->getUsername() + " ditarik ke posisi " + player.getUsername() + ".");
        lasso->setTargetPlayer(targetPlayer);
    }
    else if (auto demo = dynamic_cast<DemolitionCard*>(targetCard.get())) {
        vector<StreetTile*> demolishableTargets;
        vector<string> names;
        vector<string> codes;
        vector<string> owners;
        vector<string> buildings;

        for (Player& other : game.getPlayers()) {
            if (other.getUsername() == player.getUsername()) continue;
            if (other.isBankrupt()) continue;

            for (OwnableTile* tile : game.getPropertyManager().getOwnedProperties(game.getBoard(), other)) {
                StreetTile* street = dynamic_cast<StreetTile*>(tile);
                if (street && street->getBuildingLevel() > 0) {
                    demolishableTargets.push_back(street);
                    names.push_back(street->getName());
                    codes.push_back(street->getCode());
                    owners.push_back(other.getUsername());
                    string buildingDesc = street->hasHotel() ? "Hotel" : to_string(street->getBuildingLevel()) + " Rumah";
                    buildings.push_back(buildingDesc);
                }
            }
        }

        if (demolishableTargets.empty()) {
            uiManager.printMessage("Tidak ada properti lawan yang memiliki bangunan. DemolitionCard tidak berpengaruh.");
            return;
        }

        int choice = uiManager.readDemolitionTarget(names, codes, owners, buildings);
        if (choice == 0) {
            uiManager.printMessage("DemolitionCard dibatalkan.");
            return;
        }
        
        StreetTile* targetStreet = demolishableTargets[choice - 1];
        uiManager.printMessage("Bangunan di " + targetStreet->getName() + " (" + targetStreet->getCode() + ") dihancurkan!");
        demo->setTargetStreet(targetStreet);
    }

    game.getCardManager().useHandCard(game, player, cardIndex);
}
