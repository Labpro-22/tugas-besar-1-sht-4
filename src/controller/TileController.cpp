#include "controller/TileController.hpp"

#include "model/Game.hpp"
#include "model/NimonException.hpp"
#include "model/Player.hpp"
#include "model/RentContext.hpp"
#include "model/cards/ChanceCard.hpp"
#include "model/cards/CommunityChestCard.hpp"
#include "model/cards/HandCard.hpp"
#include "model/tiles/ChanceTile.hpp"
#include "model/tiles/CommunityChestTile.hpp"
#include "model/tiles/FestivalTile.hpp"
#include "model/tiles/GoToJailTile.hpp"
#include "model/tiles/IncomeTaxTile.hpp"
#include "model/tiles/LuxuryTaxTile.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/Tile.hpp"
#include "model/tiles/UtilityTile.hpp"
#include "view/UIManager.hpp"

#include <algorithm>
#include <cctype>
#include <memory>
#include <sstream>

using namespace std;

string TileController::toUpperCopy(string value) const {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return value;
}

string TileController::ownershipStatusText(OwnershipStatus status) const {
    if (status == OwnershipStatus::BANK) return "BANK";
    if (status == OwnershipStatus::OWNED) return "OWNED";
    return "MORTGAGED";
}

string TileController::normalizeColorKey(string value) const {
    string normalized;
    for (char ch : value) {
        if (isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

string TileController::colorDisplayName(const string& colorGroup) const {
    const string key = normalizeColorKey(colorGroup);
    if (key == "COKLAT" || key == "BROWN" || key == "CK") return "COKLAT";
    if (key == "BIRUMUDA" || key == "LIGHTBLUE" || key == "BM") return "BIRU MUDA";
    if (key == "PINK" || key == "PK") return "PINK";
    if (key == "ORANGE" || key == "ORANYE" || key == "OR") return "ORANGE";
    if (key == "MERAH" || key == "RED" || key == "MR") return "MERAH";
    if (key == "KUNING" || key == "YELLOW" || key == "KN") return "KUNING";
    if (key == "HIJAU" || key == "GREEN" || key == "HJ") return "HIJAU";
    if (key == "BIRUTUA" || key == "DARKBLUE" || key == "NAVY" || key == "BT") return "BIRU TUA";
    if (key == "STASIUN" || key == "RAILROAD") return "STASIUN";
    if (key == "UTILITAS" || key == "UTILITY" || key == "AB") return "UTILITAS";
    return "AKSI";
}

string TileController::buildingText(const StreetTile& tile) const {
    if (tile.hasHotel()) {
        return "Hotel";
    }
    if (tile.getBuildingLevel() <= 0) {
        return "Tanpa bangunan";
    }
    return to_string(tile.getBuildingLevel()) + " rumah";
}

string TileController::festivalStatus(const StreetTile& tile) const {
    if (tile.getFestivalDuration() <= 0 || tile.getFestivalMultiplier() <= 1) {
        return "";
    }
    return "aktif x" + to_string(tile.getFestivalMultiplier()) +
           " (" + to_string(tile.getFestivalDuration()) + " giliran)";
}

int TileController::rentPreview(const StreetTile& tile) const {
    const vector<int>& rentLevels = tile.getRentLevels();
    const int level = min(tile.getBuildingLevel(), static_cast<int>(rentLevels.size()) - 1);
    if (level < 0 || rentLevels.empty()) {
        return 0;
    }
    return rentLevels[static_cast<size_t>(level)] * tile.getFestivalMultiplier();
}

void TileController::buildDeedData(
    const OwnableTile& tile,
    string& title,
    int& purchasePrice,
    int& mortgageValue,
    vector<string>& moneyRowLabels,
    vector<int>& moneyRowValues,
    vector<string>& detailRowLabels,
    vector<string>& detailRowValues,
    string& ownershipStatus,
    string& ownerName
) const {
    const StreetTile* street = dynamic_cast<const StreetTile*>(&tile);
    const RailroadTile* railroad = dynamic_cast<const RailroadTile*>(&tile);
    const UtilityTile* utility = dynamic_cast<const UtilityTile*>(&tile);

    if (street != nullptr) {
        title = "[" + colorDisplayName(street->getColorGroup()) + "] " +
                toUpperCopy(tile.getName()) + " (" + tile.getCode() + ")";
        const vector<int>& rents = street->getRentLevels();
        const vector<string> labels = {
            "Sewa dasar",
            "Sewa (1 rumah)",
            "Sewa (2 rumah)",
            "Sewa (3 rumah)",
            "Sewa (4 rumah)",
            "Sewa (hotel)"
        };
        for (size_t i = 0; i < labels.size() && i < rents.size(); i++) {
            moneyRowLabels.push_back(labels[i]);
            moneyRowValues.push_back(rents[i]);
        }
        moneyRowLabels.push_back("Harga Rumah");
        moneyRowValues.push_back(street->getHouseBuildCost());
        moneyRowLabels.push_back("Harga Hotel");
        moneyRowValues.push_back(street->getHotelBuildCost());
        detailRowLabels.push_back("Bangunan");
        detailRowValues.push_back(buildingText(*street));
    } else if (railroad != nullptr) {
        title = "[STASIUN] " + toUpperCopy(tile.getName()) + " (" + tile.getCode() + ")";
        detailRowLabels.push_back("Sewa");
        detailRowValues.push_back("Jumlah stasiun");
    } else if (utility != nullptr) {
        title = "[UTILITAS] " + toUpperCopy(tile.getName()) + " (" + tile.getCode() + ")";
        detailRowLabels.push_back("Sewa");
        detailRowValues.push_back("Dadu x util");
    } else {
        title = "[PROPERTI] " + toUpperCopy(tile.getName()) + " (" + tile.getCode() + ")";
    }

    purchasePrice = tile.getPurchasePrice();
    mortgageValue = tile.getMortgageValue();
    ownershipStatus = ownershipStatusText(tile.getOwnershipStatus());
    if (tile.getOwner() != nullptr) ownerName = tile.getOwner()->getUsername();
    else ownerName = "BANK";
}

void TileController::returnPropertyToBank(OwnableTile& property) const {
    StreetTile* street = dynamic_cast<StreetTile*>(&property);
    if (street != nullptr) {
        street->sellBuildings();
    }
    property.setOwner(nullptr);
    property.setOwnershipStatus(OwnershipStatus::BANK);
}

vector<OwnableTile*> TileController::ownedProperties(Game& game, Player& player) const {
    return game.getPropertyManager().getOwnedProperties(game.getBoard(), player);
}

int TileController::liquidationValue(Game& game, Player& player) const {
    int total = 0;
    for (OwnableTile* property : ownedProperties(game, player)) {
        if (property != nullptr) {
            if (property->isOwned()) {
                total += max(property->getSelltoBankValue(), property->getMortgageValue());
            } else if (property->isMortgaged()) {
                total += 0;
            }
        }
    }
    return total;
}

int TileController::discountedPurchasePrice(const Player& player, const OwnableTile& property) const {
    int price = property.getPurchasePrice();
    if (player.getDiscountDuration() > 0 && player.getDiscountPercent() > 0) {
        price = price * (100 - player.getDiscountPercent()) / 100;
    }
    return max(0, price);
}

void TileController::acquireProperty(Player& player, OwnableTile& property, int price) const {
    player -= price;
    property.setOwner(&player);
    property.setOwnershipStatus(OwnershipStatus::OWNED);
}

size_t TileController::activePlayerCount(Game& game) const {
    size_t count = 0;
    for (const Player& player : game.getPlayers()) {
        if (!player.isBankrupt()) count++;
    }
    return count;
}

TileController::TileController(Game& game, UIManager& uiManager)
    : game(game),
      uiManager(uiManager),
      pendingDebtAmount(0),
      pendingCreditor(nullptr),
      pendingDebtToBank(true) {}

TileController::TileController(const TileController& other)
    : game(other.game),
      uiManager(other.uiManager),
      pendingDebtAmount(other.pendingDebtAmount),
      pendingCreditor(other.pendingCreditor),
      pendingDebtToBank(other.pendingDebtToBank) {}

TileController::~TileController() {}

TileController& TileController::operator=(const TileController& other) {
    if (this != &other) {
        pendingDebtAmount = other.pendingDebtAmount;
        pendingCreditor = other.pendingCreditor;
        pendingDebtToBank = other.pendingDebtToBank;
    }
    return *this;
}

void TileController::resolveLanding(Tile& tile, Player& player) {
    switch (tile.onLand()) {
        case Tile::TileType::Street:
            handleStreetPurchase(static_cast<StreetTile&>(tile));
            break;
        case Tile::TileType::Railroad:
        case Tile::TileType::Utility: {
            OwnableTile& property = static_cast<OwnableTile&>(tile);
            Player* owner = property.getOwner();

            if (owner == nullptr || property.getOwnershipStatus() == OwnershipStatus::BANK) {
                const int price = discountedPurchasePrice(player, property);
                if (player.getMoney() < price) {
                    uiManager.printError("Uang kamu tidak cukup untuk membeli " + tile.getName() + ".");
                    return;
                }
                acquireProperty(player, property, price);
                if (tile.onLand() == Tile::TileType::Railroad) {
                    uiManager.printRailroadAcquired(player.getUsername(), tile.getName(), tile.getCode());
                    game.getLogManager().addLog(
                        game.getCurrentTurn(),
                        player.getUsername(),
                        "RAILROAD",
                        "Membeli otomatis " + tile.getName() + " (" + tile.getCode() + ") seharga M" + to_string(price)
                    );
                } else {
                    uiManager.printUtilityAcquired(player.getUsername(), tile.getName(), tile.getCode());
                    game.getLogManager().addLog(
                        game.getCurrentTurn(),
                        player.getUsername(),
                        "UTILITY",
                        "Membeli otomatis " + tile.getName() + " (" + tile.getCode() + ") seharga M" + to_string(price)
                    );
                }
                return;
            }

            if (owner == &player) {
                uiManager.printMessage("Kamu mendarat di properti milik sendiri.");
                return;
            }

            if (property.isMortgaged()) {
                uiManager.printMortgagedNoRent(tile.getName(), tile.getCode(), owner->getUsername());
                return;
            }

            RentContext rentContext = game.getPropertyManager().createRentContext(
                game.getBoard(),
                game.getConfigManager(),
                game.getDice(),
                property
            );
            const int rent = property.calculateRent(rentContext);
            if (player.isShieldActive()) {
                uiManager.printMessage("[SHIELD ACTIVE]: Tagihan sewa M" + to_string(rent) + " dibatalkan.");
                return;
            }
            if (player.getMoney() < rent) {
                uiManager.printError("Kamu tidak mampu membayar sewa penuh! (M" + to_string(rent) + ")");
                pendingDebtAmount = rent;
                pendingCreditor = owner;
                pendingDebtToBank = false;
                handleBankruptcy(player);
                return;
            }

            string tileCondition = "Utilitas";
            if (tile.onLand() == Tile::TileType::Railroad) tileCondition = "Stasiun";

            uiManager.printRentPayment(
                player.getUsername(),
                player.getMoney(),
                owner->getUsername(),
                owner->getMoney(),
                tile.getName(),
                tile.getCode(),
                tileCondition,
                "",
                rent
            );
            player -= rent;
            *owner += rent;
            game.getLogManager().addLog(
                game.getCurrentTurn(),
                player.getUsername(),
                "SEWA",
                "Membayar sewa M" + to_string(rent) + " untuk " + tile.getName() +
                " (" + tile.getCode() + ") kepada " + owner->getUsername()
            );
            break;
        }
        case Tile::TileType::IncomeTax:
            handleIncomeTax(static_cast<IncomeTaxTile&>(tile));
            break;
        case Tile::TileType::LuxuryTax:
            handleLuxuryTax(static_cast<LuxuryTaxTile&>(tile));
            break;
        case Tile::TileType::Festival:
            handleFestival(static_cast<FestivalTile&>(tile));
            break;
        case Tile::TileType::Chance: {
            uiManager.printMessage("Kamu mendarat di Petak Kesempatan!");
            uiManager.printMessage("Mengambil kartu...");
            shared_ptr<ChanceCard> card = game.getCardManager().drawChanceCard();
            if (card != nullptr) {
                game.getLogManager().addLog(
                    game.getCurrentTurn(),
                    player.getUsername(),
                    "KESEMPATAN",
                    "Mengambil kartu " + card->getName() + ": " + card->getDescription()
                );
                card->apply(game, player);
            }
            break;
        }
        case Tile::TileType::CommunityChest: {
            uiManager.printMessage("Kamu mendarat di Petak Dana Umum!");
            uiManager.printMessage("Mengambil kartu...");
            shared_ptr<CommunityChestCard> card = game.getCardManager().drawCommunityChestCard();
            if (card != nullptr) {
                game.getLogManager().addLog(
                    game.getCurrentTurn(),
                    player.getUsername(),
                    "DANA_UMUM",
                    "Mengambil kartu " + card->getName() + ": " + card->getDescription()
                );
                card->apply(game, player);
            }
            break;
        }
        case Tile::TileType::GoToJail:
            uiManager.printMessage("Kamu mendarat di Petak Pergi ke Penjara!");
            game.getJailManager().sendToJail(player);
            player.moveTo(11);
            game.getLogManager().addLog(game.getCurrentTurn(), player.getUsername(), "JAIL", "Masuk penjara");
            break;
        case Tile::TileType::Go:
            uiManager.printMessage("Kamu berada di Petak Mulai.");
            break;
        case Tile::TileType::Jail:
            if (player.isJailed()) uiManager.printMessage("Kamu sedang berada di penjara.");
            else uiManager.printMessage("Kamu hanya mampir di penjara.");
            break;
        case Tile::TileType::FreeParking:
            uiManager.printMessage("Kamu mendarat di Bebas Parkir.");
            break;
    }
}

void TileController::handleStreetPurchase(StreetTile& tile) {
    Player& player = game.getCurrentPlayer();
    Player* owner = tile.getOwner();

    if (owner == nullptr || tile.getOwnershipStatus() == OwnershipStatus::BANK) {
        string title;
        int purchasePrice = 0;
        int mortgageValue = 0;
        vector<string> moneyRowLabels;
        vector<int> moneyRowValues;
        vector<string> detailRowLabels;
        vector<string> detailRowValues;
        string ownershipStatus;
        string ownerName;

        buildDeedData(
            tile,
            title,
            purchasePrice,
            mortgageValue,
            moneyRowLabels,
            moneyRowValues,
            detailRowLabels,
            detailRowValues,
            ownershipStatus,
            ownerName
        );

        uiManager.printStreetPurchasePrompt(
            player.getUsername(),
            player.getMoney(),
            title,
            purchasePrice,
            mortgageValue,
            moneyRowLabels,
            moneyRowValues,
            detailRowLabels,
            detailRowValues,
            ownershipStatus,
            ownerName
        );

        const bool wantsToBuy = uiManager.readYesNo();
        const int price = discountedPurchasePrice(player, tile);
        if (wantsToBuy && player.getMoney() >= price) {
            acquireProperty(player, tile, price);
            uiManager.printMessage(tile.getName() + " kini menjadi milikmu!");
            uiManager.printMessage("Uang tersisa: M" + to_string(player.getMoney()));
            game.getLogManager().addLog(
                game.getCurrentTurn(),
                player.getUsername(),
                "BELI",
                "Membeli " + tile.getName() + " (" + tile.getCode() + ") seharga M" + to_string(price)
            );
            return;
        }

        uiManager.printMessage("Properti ini akan masuk ke sistem lelang...");
        handleAuction(tile, &player);
        return;
    }

    if (owner == &player) {
        uiManager.printMessage("Kamu mendarat di properti milik sendiri.");
        return;
    }

    if (tile.isMortgaged()) {
        uiManager.printMortgagedNoRent(tile.getName(), tile.getCode(), owner->getUsername());
        return;
    }

    RentContext rentContext = game.getPropertyManager().createRentContext(
        game.getBoard(),
        game.getConfigManager(),
        game.getDice(),
        tile
    );
    const int rent = tile.calculateRent(rentContext);
    if (player.isShieldActive()) {
        uiManager.printMessage("[SHIELD ACTIVE]: Tagihan sewa M" + to_string(rent) + " dibatalkan.");
        return;
    }
    if (player.getMoney() < rent) {
        uiManager.printError("Kamu tidak mampu membayar sewa penuh! (M" + to_string(rent) + ")");
        pendingDebtAmount = rent;
        pendingCreditor = owner;
        pendingDebtToBank = false;
        handleBankruptcy(player);
        return;
    }

    uiManager.printRentPayment(
        player.getUsername(),
        player.getMoney(),
        owner->getUsername(),
        owner->getMoney(),
        tile.getName(),
        tile.getCode(),
        buildingText(tile),
        festivalStatus(tile),
        rent
    );
    player -= rent;
    *owner += rent;
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "SEWA",
        "Membayar sewa M" + to_string(rent) + " untuk " + tile.getName() +
        " (" + tile.getCode() + ") kepada " + owner->getUsername()
    );
}

void TileController::handleAuction(OwnableTile& tile, Player* triggerPlayer) {
    vector<Player*> participants;
    vector<Player>& players = game.getPlayers();
    vector<int> auctionOrder = game.getTurnManager().getTurnOrder();
    if (auctionOrder.empty()) {
        for (size_t i = 0; i < players.size(); i++) {
            auctionOrder.push_back(static_cast<int>(i));
        }
    }

    int triggerPlayerIndex = -1;
    if (triggerPlayer != nullptr) {
        for (size_t i = 0; i < players.size(); i++) {
            if (&players[i] == triggerPlayer) {
                triggerPlayerIndex = static_cast<int>(i);
                break;
            }
        }
    }

    int triggerOrderIndex = game.getTurnManager().getCurrentPlayerIndex();
    if (triggerPlayerIndex >= 0) {
        for (size_t i = 0; i < auctionOrder.size(); i++) {
            if (auctionOrder[i] == triggerPlayerIndex) {
                triggerOrderIndex = static_cast<int>(i);
                break;
            }
        }
    }
    if (triggerOrderIndex < 0 || triggerOrderIndex >= static_cast<int>(auctionOrder.size())) {
        triggerOrderIndex = -1;
    }

    for (size_t offset = 1; offset <= auctionOrder.size(); offset++) {
        size_t orderIndex = offset - 1;
        if (triggerOrderIndex >= 0) {
            orderIndex = (static_cast<size_t>(triggerOrderIndex) + offset) % auctionOrder.size();
        }

        const int playerIndex = auctionOrder[orderIndex];
        if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size()) && !players[playerIndex].isBankrupt()) {
            participants.push_back(&players[playerIndex]);
        }
    }

    if (participants.empty()) {
        return;
    }

    int currentIndex = 0;
    int currentBid = 0;
    Player* highestBidder = nullptr;
    int passesAfterBid = 0;
    int totalPassesWithoutBid = 0;

    uiManager.printMessage("Properti " + tile.getName() + " (" + tile.getCode() + ") akan dilelang!");

    while (true) {
        Player* currentPlayer = participants[static_cast<size_t>(currentIndex)];
        string highestBidderName = "";
        if (highestBidder != nullptr) highestBidderName = highestBidder->getUsername();

        uiManager.printAuctionState(
            tile.getName(),
            tile.getCode(),
            currentBid,
            highestBidderName,
            currentPlayer->getUsername()
        );

        string action = uiManager.readAuctionAction();
        if (action.rfind("BID", 0) == 0) {
            int amount = 0;
            istringstream stream(action);
            string ignored;
            if (!(stream >> ignored >> amount)) {
                uiManager.printError("Format bid: BID <jumlah>.");
                continue;
            }

            const int minimumBid = highestBidder == nullptr ? 0 : currentBid + 1;
            if (amount < minimumBid) {
                uiManager.printError("Bid minimal saat ini adalah M" + to_string(minimumBid) + ".");
                continue;
            }
            if (currentPlayer->getMoney() < amount) {
                uiManager.printError("Bid tidak boleh melebihi uang pemain.");
                continue;
            }

            currentBid = amount;
            highestBidder = currentPlayer;
            passesAfterBid = 0;
            totalPassesWithoutBid = 0;
            uiManager.printMessage("Penawaran tertinggi: M" + to_string(currentBid) + " (" + currentPlayer->getUsername() + ")");
            game.getLogManager().addLog(
                game.getCurrentTurn(),
                currentPlayer->getUsername(),
                "BID",
                "Menawar M" + to_string(currentBid) + " untuk " + tile.getName() + " (" + tile.getCode() + ")"
            );
        } else {
            if (highestBidder == nullptr && totalPassesWithoutBid >= static_cast<int>(participants.size()) - 1) {
                uiManager.printError("Minimal harus ada satu bid. Pemain terakhir yang belum pass wajib melakukan BID.");
                continue;
            }
            if (highestBidder == nullptr) {
                totalPassesWithoutBid++;
            } else if (currentPlayer != highestBidder) {
                passesAfterBid++;
            }
        }

        if (highestBidder != nullptr && passesAfterBid >= static_cast<int>(participants.size()) - 1) {
            break;
        }

        currentIndex = (currentIndex + 1) % static_cast<int>(participants.size());
    }

    if (highestBidder == nullptr) {
        return;
    }

    *highestBidder -= currentBid;
    tile.setOwner(highestBidder);
    tile.setOwnershipStatus(OwnershipStatus::OWNED);

    uiManager.printAuctionWinner(tile.getName(), tile.getCode(), highestBidder->getUsername(), currentBid);
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        highestBidder->getUsername(),
        "LELANG",
        "Menang " + tile.getName() + " (" + tile.getCode() + ") seharga M" + to_string(currentBid)
    );
}

void TileController::handleIncomeTax(IncomeTaxTile& tile) {
    Player& player = game.getCurrentPlayer();
    uiManager.printIncomeTaxState(
        player.getUsername(),
        player.getMoney(),
        tile.getFlatTax(),
        tile.getPercentageTax()
    );

    const int choice = uiManager.readIncomeTaxChoice();
    vector<OwnableTile*> properties = ownedProperties(game, player);
    int propertyValue = 0;
    int buildingValue = 0;
    for (OwnableTile* property : properties) {
        if (property != nullptr) {
            propertyValue += property->getPurchasePrice();
            buildingValue += property->getBuildingValue();
        }
    }

    const int totalWealth = player.getMoney() + propertyValue + buildingValue;
    int taxAmount = tile.calculatePercentageTax(totalWealth);
    if (choice == 1) taxAmount = tile.calculateFlatTax();

    if (choice == 2) {
        uiManager.printIncomeTaxBreakdown(
            player.getMoney(),
            player.getMoney(),
            propertyValue,
            buildingValue,
            totalWealth,
            tile.getPercentageTax(),
            taxAmount
        );
    }

    if (player.isShieldActive()) {
        uiManager.printMessage("[SHIELD ACTIVE]: Pajak M" + to_string(taxAmount) + " dibatalkan.");
        return;
    }

    if (player.getMoney() < taxAmount) {
        uiManager.printError("Kamu tidak mampu membayar pajak! Uang kamu saat ini: M" + to_string(player.getMoney()));
        pendingDebtAmount = taxAmount;
        pendingCreditor = nullptr;
        pendingDebtToBank = true;
        handleBankruptcy(player);
        return;
    }

    player -= taxAmount;
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "PAJAK",
        "Membayar pajak M" + to_string(taxAmount) + " di " + tile.getName() + " (" + tile.getCode() + ")"
    );
}

void TileController::handleLuxuryTax(LuxuryTaxTile& tile) {
    Player& player = game.getCurrentPlayer();
    const int taxAmount = tile.calculateLuxuryTax();
    uiManager.printLuxuryTaxState(player.getMoney(), taxAmount);

    if (player.isShieldActive()) {
        uiManager.printMessage("[SHIELD ACTIVE]: Pajak M" + to_string(taxAmount) + " dibatalkan.");
        return;
    }

    if (player.getMoney() < taxAmount) {
        uiManager.printError("Kamu tidak mampu membayar pajak! Uang kamu saat ini: M" + to_string(player.getMoney()));
        pendingDebtAmount = taxAmount;
        pendingCreditor = nullptr;
        pendingDebtToBank = true;
        handleBankruptcy(player);
        return;
    }

    player -= taxAmount;
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "PAJAK",
        "Membayar pajak M" + to_string(taxAmount) + " di " + tile.getName() + " (" + tile.getCode() + ")"
    );
}

void TileController::handleFestival(FestivalTile& tile) {
    (void) tile;
    Player& player = game.getCurrentPlayer();
    vector<OwnableTile*> properties = ownedProperties(game, player);

    vector<StreetTile*> streets;
    vector<string> names;
    vector<string> codes;
    vector<string> statuses;
    for (OwnableTile* property : properties) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr) {
            streets.push_back(street);
            names.push_back(street->getName());
            codes.push_back(street->getCode());
            statuses.push_back(festivalStatus(*street));
        }
    }

    uiManager.printFestivalState(player.getUsername(), names, codes, statuses);
    if (streets.empty()) {
        return;
    }

    const int choice = uiManager.readFestivalPropertyChoice(static_cast<int>(streets.size()));
    if (choice == 0) {
        uiManager.printMessage("Festival dibatalkan.");
        return;
    }
    if (choice < 1 || choice > static_cast<int>(streets.size())) {
        uiManager.printMessage("Festival dibatalkan.");
        return;
    }

    StreetTile* selected = streets[static_cast<size_t>(choice - 1)];
    const int oldRent = rentPreview(*selected);
    const int oldMultiplier = selected->getFestivalMultiplier();
    const bool alreadyMaxed = selected->getFestivalMultiplier() >= 8;
    game.getFestivalManager().activateFestival(*selected);
    const int newRent = rentPreview(*selected);
    const int newMultiplier = selected->getFestivalMultiplier();

    if (alreadyMaxed) {
        uiManager.printFestivalMaxed(selected->getName(), selected->getCode(), selected->getFestivalDuration());
    } else {
        uiManager.printFestivalActivated(
            selected->getName(),
            selected->getCode(),
            oldRent,
            newRent,
            selected->getFestivalDuration()
        );
    }

    string festivalAction = "Aktivasi";
    if (oldMultiplier > 1) festivalAction = "Penguatan";
    if (alreadyMaxed) festivalAction = "Perpanjangan";
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "FESTIVAL",
        festivalAction + " festival di " + selected->getName() + " (" + selected->getCode() +
        "), x" + to_string(oldMultiplier) + " -> x" + to_string(newMultiplier) +
        ", sewa M" + to_string(oldRent) + " -> M" + to_string(newRent) +
        ", durasi " + to_string(selected->getFestivalDuration()) + " giliran"
    );
}

void TileController::handleForceDrop(Player& player) {
    vector<shared_ptr<HandCard>> hand = game.getCardManager().getHandCards(player);
    vector<string> cardNames;
    for (const shared_ptr<HandCard>& card : hand) {
        if (card != nullptr) cardNames.push_back(card->getName());
        else cardNames.push_back("Kartu tidak dikenal");
    }

    uiManager.printForceDropState(player.getUsername(), cardNames);
    if (cardNames.empty()) {
        return;
    }

    const int choice = uiManager.readForceDropChoice(static_cast<int>(cardNames.size()));
    game.getCardManager().dropHandCard(player, choice - 1);
    uiManager.printMessage(cardNames[static_cast<size_t>(choice - 1)] + " telah dibuang.");
}

void TileController::handleJailTurn(Player& player) {
    const int jailFine = game.getConfigManager().getJailFine();
    uiManager.printJailOptions(player.getUsername(), player.getMoney(), jailFine, 0);
    const int choice = uiManager.readJailChoice();

    if (choice == 1) {
        if (player.getMoney() < jailFine) {
            pendingDebtAmount = jailFine;
            pendingCreditor = nullptr;
            pendingDebtToBank = true;
            handleBankruptcy(player);
            return;
        }
        game.getJailManager().payJailFine(player, jailFine);
        game.getJailManager().releaseFromJail(player);
        uiManager.printMessage("Kamu membayar denda dan keluar dari penjara.");
    } else if (choice == 2) {
        const bool released = game.getJailManager().tryRollForRelease(game.getDice(), player);
        uiManager.printDiceRoll(game.getDice().getDie1(), game.getDice().getDie2(), game.getDice().getTotal());
        if (released) uiManager.printMessage("Double! Kamu keluar dari penjara.");
        else uiManager.printMessage("Belum double. Kamu tetap di penjara.");
    } else if (choice == 3) {
        uiManager.printMessage("Belum ada kartu bebas penjara yang terhubung. Pilih opsi lain pada giliran berikutnya.");
    }
}

void TileController::handleBankruptcy(Player& player) {
    const int liquidation = liquidationValue(game, player);
    uiManager.printLiquidationState(
        player.getUsername(),
        player.getMoney(),
        pendingDebtAmount,
        liquidation
    );

    if (player.getMoney() + liquidation >= pendingDebtAmount) {
        while (player.getMoney() < pendingDebtAmount) {
            vector<OwnableTile*> properties = ownedProperties(game, player);
            vector<OwnableTile*> actions;
            vector<string> actionTypes;
            int option = 1;

            uiManager.printMessage("=== Opsi Likuidasi ===");
            for (OwnableTile* property : properties) {
                if (property == nullptr) {
                    continue;
                }
                if (property->isOwned()) {
                    uiManager.printMessage(
                        to_string(option) + ". Jual " + property->getName() +
                        " (" + property->getCode() + ") ke Bank: M" +
                        to_string(property->getSelltoBankValue())
                    );
                    actions.push_back(property);
                    actionTypes.push_back("SELL");
                    option++;

                    if (!property->isMortgaged()) {
                        uiManager.printMessage(
                            to_string(option) + ". Gadaikan " + property->getName() +
                            " (" + property->getCode() + "): M" +
                            to_string(property->getMortgageValue())
                        );
                        actions.push_back(property);
                        actionTypes.push_back("MORTGAGE");
                        option++;
                    }
                }
            }

            if (actions.empty()) {
                break;
            }

            const int choice = uiManager.readLiquidationChoice(static_cast<int>(actions.size()));
            if (choice == 0) {
                uiManager.printError("Kewajiban belum terpenuhi. Kamu wajib melikuidasi aset.");
                continue;
            }
            if (choice < 1 || choice > static_cast<int>(actions.size())) {
                uiManager.printError("Pilihan likuidasi tidak valid.");
                continue;
            }

            OwnableTile* selected = actions[static_cast<size_t>(choice - 1)];
            const string actionType = actionTypes[static_cast<size_t>(choice - 1)];
            if (actionType == "SELL") {
                const int sellValue = selected->getSelltoBankValue();
                player += sellValue;
                uiManager.printMessage(selected->getName() + " terjual ke Bank. Kamu menerima M" + to_string(sellValue) + ".");
                game.getLogManager().addLog(
                    game.getCurrentTurn(),
                    player.getUsername(),
                    "JUAL",
                    "Menjual " + selected->getName() + " (" + selected->getCode() + ") ke Bank seharga M" + to_string(sellValue)
                );
                returnPropertyToBank(*selected);
            } else {
                const int mortgageValue = selected->getMortgageValue();
                game.getPropertyManager().mortgageProperty(player, *selected);
                uiManager.printMessage(selected->getName() + " digadaikan. Kamu menerima M" + to_string(mortgageValue) + ".");
                game.getLogManager().addLog(
                    game.getCurrentTurn(),
                    player.getUsername(),
                    "GADAI",
                    "Menggadaikan " + selected->getName() + " (" + selected->getCode() + ") senilai M" + to_string(mortgageValue)
                );
            }
            uiManager.printMessage("Uang kamu sekarang: M" + to_string(player.getMoney()));
        }

        if (player.getMoney() >= pendingDebtAmount) {
            if (!pendingDebtToBank && pendingCreditor != nullptr) {
                player -= pendingDebtAmount;
                *pendingCreditor += pendingDebtAmount;
            } else {
                player -= pendingDebtAmount;
            }
            uiManager.printMessage("Kewajiban M" + to_string(pendingDebtAmount) + " terpenuhi.");
            return;
        }
    }

    uiManager.printMessage(player.getUsername() + " dinyatakan BANGKRUT!");

    vector<OwnableTile*> properties = ownedProperties(game, player);
    if (!pendingDebtToBank && pendingCreditor != nullptr) {
        *pendingCreditor += max(0, player.getMoney());
        player -= player.getMoney();

        for (OwnableTile* property : properties) {
            if (property != nullptr) {
                property->setOwner(pendingCreditor);
                OwnershipStatus status = OwnershipStatus::OWNED;
                if (property->isMortgaged()) status = OwnershipStatus::MORTGAGED;
                property->setOwnershipStatus(status);
                game.getLogManager().addLog(
                    game.getCurrentTurn(),
                    player.getUsername(),
                    "ASET",
                    property->getName() + " (" + property->getCode() + ") dialihkan ke " + pendingCreditor->getUsername()
                );
            }
        }
        uiManager.printMessage("Aset dialihkan ke " + pendingCreditor->getUsername() + ".");
    } else {
        player -= player.getMoney();
        player.setBankrupt(true);
        for (OwnableTile* property : properties) {
            if (property != nullptr) {
                returnPropertyToBank(*property);
                game.getLogManager().addLog(
                    game.getCurrentTurn(),
                    player.getUsername(),
                    "ASET",
                    property->getName() + " (" + property->getCode() + ") dikembalikan ke Bank"
                );
                if (activePlayerCount(game) > 0) {
                    handleAuction(*property, &player);
                }
            }
        }
        uiManager.printMessage("Seluruh properti dikembalikan ke Bank.");
    }

    player.setBankrupt(true);
    game.getLogManager().addLog(game.getCurrentTurn(), player.getUsername(), "BANGKRUT", "Keluar dari permainan");
}
