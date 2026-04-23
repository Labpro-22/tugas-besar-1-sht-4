#include "controller/CommandController.hpp"

#include "controller/TileController.hpp"
#include "model/Board.hpp"
#include "model/Dice.hpp"
#include "model/Game.hpp"
#include "model/NimonException.hpp"
#include "model/Player.hpp"
#include "model/cards/HandCard.hpp"
#include "model/managers/LogManager.hpp"
#include "model/tiles/GoTile.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/Tile.hpp"
#include "model/tiles/UtilityTile.hpp"
#include "view/UIManager.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

using namespace std;

string CommandController::toUpperCopy(string value) const {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return value;
}

string CommandController::firstToken(const string& input) const {
    istringstream stream(input);
    string token;
    stream >> token;
    return toUpperCopy(token);
}

string CommandController::formatMoney(int amount) const {
    const bool negative = amount < 0;
    long long value = amount;
    if (negative) {
        value *= -1;
    }

    string digits = to_string(value);
    string formatted;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; i--) {
        if (count == 3) {
            formatted.push_back('.');
            count = 0;
        }
        formatted.push_back(digits[static_cast<size_t>(i)]);
        count++;
    }

    reverse(formatted.begin(), formatted.end());

    string prefix = "M";
    if (negative) prefix += "-";
    return prefix + formatted;
}

int CommandController::playerNumberByPointer(const vector<Player>& players, const Player* player) const {
    if (player == nullptr) {
        return 0;
    }

    for (size_t i = 0; i < players.size(); i++) {
        if (&players[i] == player) {
            return static_cast<int>(i) + 1;
        }
    }
    return 0;
}

string CommandController::ownerLabel(const OwnableTile& tile, const vector<Player>& players) const {
    Player* owner = tile.getOwner();
    if (owner == nullptr) {
        return "BANK";
    }

    const int playerNumber = playerNumberByPointer(players, owner);
    if (playerNumber > 0) {
        return "P" + to_string(playerNumber);
    }

    return owner->getUsername();
}

string CommandController::ownershipStatusText(OwnershipStatus status) const {
    if (status == OwnershipStatus::BANK) return "BANK";
    if (status == OwnershipStatus::OWNED) return "OWNED";
    return "MORTGAGED";
}

string CommandController::normalizeColorKey(string value) const {
    string normalized;
    for (char ch : value) {
        if (isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

string CommandController::shortColorCode(const string& colorGroup) const {
    const string key = normalizeColorKey(colorGroup);
    if (key == "COKLAT" || key == "BROWN" || key == "CK") return "CK";
    if (key == "BIRUMUDA" || key == "LIGHTBLUE" || key == "BM") return "BM";
    if (key == "PINK" || key == "PK") return "PK";
    if (key == "ORANGE" || key == "ORANYE" || key == "OR") return "OR";
    if (key == "MERAH" || key == "RED" || key == "MR") return "MR";
    if (key == "KUNING" || key == "YELLOW" || key == "KN") return "KN";
    if (key == "HIJAU" || key == "GREEN" || key == "HJ") return "HJ";
    if (key == "BIRUTUA" || key == "DARKBLUE" || key == "NAVY" || key == "BT") return "BT";
    if (colorGroup.empty()) return "DF";
    return colorGroup;
}

string CommandController::colorDisplayName(const string& colorGroup) const {
    const string code = shortColorCode(colorGroup);
    if (code == "CK") return "COKLAT";
    if (code == "BM") return "BIRU MUDA";
    if (code == "PK") return "PINK";
    if (code == "OR") return "ORANGE";
    if (code == "MR") return "MERAH";
    if (code == "KN") return "KUNING";
    if (code == "HJ") return "HIJAU";
    if (code == "BT") return "BIRU TUA";
    if (code == "AB") return "UTILITAS";
    if (normalizeColorKey(colorGroup) == "STASIUN") return "STASIUN";
    return "AKSI";
}

string CommandController::categoryCode(const Tile& tile) const {
    const StreetTile* street = dynamic_cast<const StreetTile*>(&tile);
    if (street != nullptr) return shortColorCode(street->getColorGroup());
    if (dynamic_cast<const UtilityTile*>(&tile) != nullptr) return "AB";
    return "DF";
}

string CommandController::buildingText(const StreetTile& tile) const {
    if (tile.hasHotel()) {
        return "Hotel";
    }
    if (tile.getBuildingLevel() <= 0) {
        return "-";
    }
    return to_string(tile.getBuildingLevel()) + " rumah";
}

string CommandController::buildingMarker(const StreetTile& tile) const {
    if (tile.hasHotel()) {
        return "*";
    }
    return string(static_cast<size_t>(max(0, tile.getBuildingLevel())), '^');
}

string CommandController::propertyGroupLabel(const OwnableTile& property) const {
    const StreetTile* street = dynamic_cast<const StreetTile*>(&property);
    if (street != nullptr) {
        return street->getColorGroup();
    }
    if (dynamic_cast<const RailroadTile*>(&property) != nullptr) {
        return "STASIUN";
    }
    if (dynamic_cast<const UtilityTile*>(&property) != nullptr) {
        return "AB";
    }
    return "DF";
}

int CommandController::normalizedPosition(int rawPosition, int boardSize) const {
    if (boardSize <= 0) {
        return rawPosition;
    }
    if (rawPosition >= 1 && rawPosition <= boardSize) {
        return rawPosition;
    }
    int zeroBased = (rawPosition - 1) % boardSize;
    if (zeroBased < 0) {
        zeroBased += boardSize;
    }
    return zeroBased + 1;
}

string CommandController::playerTokensForTile(const Game& game, int tileIndex) const {
    const vector<Player>& players = game.getPlayers();
    const int boardSize = game.getBoard().getBoardSize();
    vector<string> jailed;
    vector<string> visitors;
    vector<string> tokens;

    for (size_t i = 0; i < players.size(); i++) {
        const Player& player = players[i];
        if (player.isBankrupt()) {
            continue;
        }

        const int position = normalizedPosition(player.getPosition(), boardSize);
        if (position != tileIndex) {
            continue;
        }

        const string number = to_string(static_cast<int>(i) + 1);
        if (tileIndex == 11) {
            if (player.isJailed()) {
                jailed.push_back(number);
            } else {
                visitors.push_back(number);
            }
        } else {
            tokens.push_back("(" + number + ")");
        }
    }

    if (tileIndex == 11 && (!jailed.empty() || !visitors.empty())) {
        string text;
        if (!jailed.empty()) {
            text += "IN:";
            for (size_t i = 0; i < jailed.size(); i++) {
                if (i > 0) text += ",";
                text += jailed[i];
            }
        }
        if (!visitors.empty()) {
            if (!text.empty()) text += " ";
            text += "V:";
            for (size_t i = 0; i < visitors.size(); i++) {
                if (i > 0) text += ",";
                text += visitors[i];
            }
        }
        return text;
    }

    string text;
    for (const string& token : tokens) {
        text += token;
    }
    return text;
}

void CommandController::buildDeedData(
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
            "Sewa (unimproved)",
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
        if (street->getFestivalDuration() > 0 && street->getFestivalMultiplier() > 1) {
            detailRowLabels.push_back("Festival");
            detailRowValues.push_back(
                "x" + to_string(street->getFestivalMultiplier()) +
                ", " + to_string(street->getFestivalDuration()) + " giliran"
            );
        }
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

void CommandController::printDeedFromTile(UIManager& uiManager, const OwnableTile& tile) const {
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

    uiManager.printDeed(
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
}

int CommandController::wrappedMove(Game& game, Player& player, int steps) const {
    Board& board = game.getBoard();
    const int boardSize = board.getBoardSize();
    if (boardSize <= 0) {
        player.moveTo(player.getPosition() + steps);
        return player.getPosition();
    }

    const int oldPosition = normalizedPosition(player.getPosition(), boardSize);
    const int rawPosition = oldPosition + steps;
    const int newPosition = normalizedPosition(rawPosition, boardSize);

    if (rawPosition > boardSize) {
        shared_ptr<Tile> goTile = board.getTile(1);
        GoTile* go = dynamic_cast<GoTile*>(goTile.get());
        int salary = game.getConfigManager().getGoSalary();
        if (go != nullptr) salary = go->getSalary();
        player += salary;
        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "GO",
            "Melewati GO dan menerima M" + to_string(salary)
        );
    }

    player.moveTo(newPosition);
    return newPosition;
}

void CommandController::fillPropertyOptionVectors(
    const vector<OwnableTile*>& properties,
    const string& valueLabel,
    vector<string>& groups,
    vector<string>& names,
    vector<string>& codes,
    vector<string>& valueLabels,
    vector<int>& values,
    vector<string>& statuses,
    bool useMortgageValue
) const {
    for (OwnableTile* property : properties) {
        if (property == nullptr) {
            continue;
        }
        groups.push_back(propertyGroupLabel(*property));
        names.push_back(property->getName());
        codes.push_back(property->getCode());
        valueLabels.push_back(valueLabel);
        int value = property->getPurchasePrice();
        if (useMortgageValue) value = property->getMortgageValue();
        values.push_back(value);
        statuses.push_back(ownershipStatusText(property->getOwnershipStatus()));
    }
}

bool CommandController::colorGroupHasBuildings(Game& game, const StreetTile& tile) const {
    vector<shared_ptr<StreetTile>> streets = game.getBoard().getStreetTileByColorGroup(tile.getColorGroup());
    for (const shared_ptr<StreetTile>& street : streets) {
        if (street != nullptr && street->getBuildingLevel() > 0) return true;
    }
    return false;
}

int CommandController::sellColorGroupBuildings(Game& game, Player& player, const StreetTile& tile) const {
    int total = 0;
    vector<shared_ptr<StreetTile>> streets = game.getBoard().getStreetTileByColorGroup(tile.getColorGroup());
    for (const shared_ptr<StreetTile>& street : streets) {
        if (street == nullptr || street->getOwner() != &player || street->getBuildingLevel() <= 0) {
            continue;
        }
        const int sellValue = street->getBuildingValue() / 2;
        total += sellValue;
        player += sellValue;
        street->sellBuildings();
    }
    return total;
}

CommandController::CommandController(Game& game, UIManager& uiManager)
    : game(game), uiManager(uiManager) {}

CommandController::CommandController(const CommandController& other)
    : game(other.game), uiManager(other.uiManager) {}

CommandController::~CommandController() {}

CommandController& CommandController::operator=(const CommandController& other) {
    (void) other;
    return *this;
}

bool CommandController::processCommand(const string& input) {
    const string command = firstToken(input);

    try {
        if (command == "LEMPAR_DADU") {
            handleRollDice();
        } else if (command == "ATUR_DADU") {
            istringstream stream(input);
            string ignored;
            int x = 0;
            int y = 0;
            stream >> ignored >> x >> y;
            if (x < 1 || x > 6 || y < 1 || y > 6) {
                uiManager.printError("Format: ATUR_DADU X Y dengan X dan Y antara 1-6.");
                return false;
            }
            handleSetDice(x, y);
        } else if (command == "CETAK_PAPAN") {
            handlePrintBoard();
        } else if (command == "CETAK_AKTA") {
            handlePrintDeed();
        } else if (command == "CETAK_PROPERTI") {
            handlePrintProperties();
        } else if (command == "GADAI") {
            handleMortgage();
        } else if (command == "TEBUS") {
            handleRedeem();
        } else if (command == "BANGUN") {
            handleBuild();
        } else if (command == "SIMPAN") {
            istringstream stream(input);
            string ignored;
            string filename;
            stream >> ignored >> filename;
            if (filename.empty()) {
                uiManager.printError("Format: SIMPAN <nama_file>");
                return false;
            }
            handleSave(filename);
        } else if (command == "CETAK_LOG") {
            handlePrintLog(parseRecentLogCount(input));
        } else if (command == "GUNAKAN_KEMAMPUAN") {
            handleUseCard();
        } else if (command == "HELP") {
            handleHelp();
        } else {
            uiManager.printError("Perintah tidak dikenal.");
            return false;
        }
    } catch (const exception& exception) {
        uiManager.printError(exception.what());
        return false;
    }

    return true;
}

void CommandController::handleHelp() {
    uiManager.printHelp();
}

bool CommandController::handleTripleDoubleJail(Player& player) const {
    if (game.getTurnManager().getConsecutiveDoubles() < 3 || player.isBankrupt()) {
        return false;
    }

    uiManager.printMessage("Dadu double tiga kali berturut-turut! Kamu masuk penjara.");
    game.getJailManager().sendToJail(player);
    player.moveTo(11);
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "JAIL",
        "Masuk penjara karena double tiga kali berturut-turut"
    );
    return true;
}

void CommandController::handleRollDice() {
    Player& player = game.getCurrentPlayer();
    game.getDice().roll();
    const int die1 = game.getDice().getDie1();
    const int die2 = game.getDice().getDie2();
    const int total = game.getDice().getTotal();

    uiManager.printDiceRoll(die1, die2, total);
    game.getTurnManager().registerDiceResult(game.getDice().isDouble());
    if (handleTripleDoubleJail(player)) {
        return;
    }

    const int destinationIndex = wrappedMove(game, player, total);
    shared_ptr<Tile> destination = game.getBoard().getTile(destinationIndex);
    if (destination == nullptr) {
        uiManager.printError("Petak tujuan tidak ditemukan.");
        return;
    }

    uiManager.printPlayerMovement(
        player.getUsername(),
        total,
        destination->getName(),
        destination->getCode()
    );

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "DADU",
        "Lempar: " + to_string(die1) + "+" + to_string(die2) + "=" +
        to_string(total) + " -> mendarat di " + destination->getName() +
        " (" + destination->getCode() + ")"
    );

    TileController tileController(game, uiManager);
    tileController.resolveLanding(*destination, player);
}

void CommandController::handleSetDice(int x, int y) {
    game.getDice() = Dice(x, y, true);
    uiManager.printMessage("Dadu diatur secara manual.");

    Player& player = game.getCurrentPlayer();
    const int total = game.getDice().getTotal();
    uiManager.printDiceRoll(x, y, total);
    game.getTurnManager().registerDiceResult(game.getDice().isDouble());
    if (handleTripleDoubleJail(player)) {
        return;
    }

    const int destinationIndex = wrappedMove(game, player, total);
    shared_ptr<Tile> destination = game.getBoard().getTile(destinationIndex);
    if (destination == nullptr) {
        uiManager.printError("Petak tujuan tidak ditemukan.");
        return;
    }

    uiManager.printPlayerMovement(
        player.getUsername(),
        total,
        destination->getName(),
        destination->getCode()
    );

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "DADU",
        "Atur dadu: " + to_string(x) + "+" + to_string(y) + "=" +
        to_string(total) + " -> mendarat di " + destination->getName() +
        " (" + destination->getCode() + ")"
    );

    TileController tileController(game, uiManager);
    tileController.resolveLanding(*destination, player);
}

void CommandController::handlePrintBoard() {
    vector<int> indices;
    vector<string> colorCodes;
    vector<vector<string>> lines;

    for (int index = 1; index <= game.getBoard().getBoardSize(); index++) {
        shared_ptr<Tile> tile = game.getBoard().getTile(index);
        if (tile == nullptr) {
            continue;
        }

        indices.push_back(tile->getIndex());
        colorCodes.push_back(categoryCode(*tile));

        vector<string> cellLines(3, "");
        cellLines[0] = "[" + categoryCode(*tile) + "] " + tile->getCode();

        const string tokens = playerTokensForTile(game, tile->getIndex());
        OwnableTile* ownable = dynamic_cast<OwnableTile*>(tile.get());
        if (ownable != nullptr) {
            string detail = ownerLabel(*ownable, game.getPlayers());
            StreetTile* street = dynamic_cast<StreetTile*>(tile.get());
            if (street != nullptr) {
                const string marker = buildingMarker(*street);
                if (!marker.empty()) {
                    detail += " " + marker;
                }
                if (street->getFestivalDuration() > 0 && street->getFestivalMultiplier() > 1) {
                    detail += " F" + to_string(street->getFestivalMultiplier()) +
                              ":" + to_string(street->getFestivalDuration());
                }
            }
            if (ownable->isMortgaged()) {
                detail += " [M]";
            }
            if (!tokens.empty()) {
                detail += " " + tokens;
            }
            cellLines[1] = detail;
        } else {
            cellLines[1] = tokens;
        }
        lines.push_back(cellLines);
    }

    string currentPlayerLabel;
    try {
        const Player& currentPlayer = game.getCurrentPlayer();
        const int number = playerNumberByPointer(game.getPlayers(), &currentPlayer);
        currentPlayerLabel = "P" + to_string(number) + " - " + currentPlayer.getUsername();
    } catch (...) {
        currentPlayerLabel = "-";
    }

    uiManager.printBoard(
        indices,
        colorCodes,
        lines,
        game.getCurrentTurn(),
        game.getMaxTurn(),
        currentPlayerLabel
    );
}

void CommandController::handlePrintDeed() {
    const string code = uiManager.readPropertyCode();
    shared_ptr<Tile> tile = game.getBoard().getTileByCode(toUpperCopy(code));
    OwnableTile* property = dynamic_cast<OwnableTile*>(tile.get());

    if (property == nullptr) {
        uiManager.printError("Petak \"" + code + "\" tidak ditemukan atau bukan properti.");
        return;
    }

    printDeedFromTile(uiManager, *property);
}

void CommandController::handlePrintProperties() {
    Player& player = game.getCurrentPlayer();
    vector<OwnableTile*> properties = game.getPropertyManager().getOwnedProperties(game.getBoard(), player);

    vector<string> groups;
    vector<string> names;
    vector<string> codes;
    vector<string> buildingStatuses;
    vector<string> statuses;
    vector<int> values;
    int totalPropertyWealth = 0;

    for (OwnableTile* property : properties) {
        if (property == nullptr) {
            continue;
        }

        StreetTile* street = dynamic_cast<StreetTile*>(property);
        groups.push_back(propertyGroupLabel(*property));
        names.push_back(property->getName());
        codes.push_back(property->getCode());
        string buildingStatus = "-";
        if (street != nullptr) buildingStatus = buildingText(*street);
        buildingStatuses.push_back(buildingStatus);
        statuses.push_back(ownershipStatusText(property->getOwnershipStatus()));
        values.push_back(property->getPurchasePrice());
        totalPropertyWealth += property->getPurchasePrice() + property->getBuildingValue();
    }

    uiManager.printPlayerProperties(
        player.getUsername(),
        player.getMoney(),
        player.countCards(),
        groups,
        names,
        codes,
        buildingStatuses,
        statuses,
        values,
        totalPropertyWealth
    );
}

void CommandController::handleMortgage() {
    Player& player = game.getCurrentPlayer();
    vector<OwnableTile*> properties = game.getPropertyManager().getMortgageableProperties(game, player);

    vector<string> groups;
    vector<string> names;
    vector<string> codes;
    vector<string> valueLabels;
    vector<int> values;
    vector<string> statuses;
    fillPropertyOptionVectors(properties, "Nilai Gadai", groups, names, codes, valueLabels, values, statuses, true);

    uiManager.printMortgageOptions(player.getMoney(), groups, names, codes, valueLabels, values, statuses);
    if (properties.empty()) {
        return;
    }

    const int choice = uiManager.readMortgageChoice(static_cast<int>(properties.size()));
    if (choice == 0) {
        uiManager.printMessage("Gadai dibatalkan.");
        return;
    }
    if (choice < 1 || choice > static_cast<int>(properties.size())) {
        uiManager.printError("Pilihan properti tidak valid.");
        return;
    }

    OwnableTile* property = properties[static_cast<size_t>(choice - 1)];
    StreetTile* street = dynamic_cast<StreetTile*>(property);
    if (street != nullptr && colorGroupHasBuildings(game, *street)) {
        uiManager.printMessage(property->getName() + " tidak dapat digadaikan!");
        uiManager.printMessage("Masih terdapat bangunan di color group [" + colorDisplayName(street->getColorGroup()) + "].");
        uiManager.printMessage("Bangunan harus dijual terlebih dahulu.");
        uiManager.printMessage("Jual semua bangunan color group [" + colorDisplayName(street->getColorGroup()) + "]? (y/n): ");
        if (!uiManager.readYesNo()) {
            uiManager.printMessage("Gadai dibatalkan.");
            return;
        }

        vector<shared_ptr<StreetTile>> groupStreets = game.getBoard().getStreetTileByColorGroup(street->getColorGroup());
        vector<string> soldBuildingLogs;
        for (const shared_ptr<StreetTile>& groupStreet : groupStreets) {
            if (groupStreet != nullptr && groupStreet->getOwner() == &player && groupStreet->getBuildingLevel() > 0) {
                soldBuildingLogs.push_back(
                    groupStreet->getName() + " (" + groupStreet->getCode() + ") seharga " +
                    formatMoney(groupStreet->getBuildingValue() / 2)
                );
            }
        }

        const int buildingValue = sellColorGroupBuildings(game, player, *street);
        uiManager.printMessage("Bangunan color group terjual. Kamu menerima " + formatMoney(buildingValue) + ".");
        uiManager.printMessage("Uang kamu sekarang: " + formatMoney(player.getMoney()));
        for (const string& detail : soldBuildingLogs) {
            game.getLogManager().addLog(
                game.getCurrentTurn(),
                player.getUsername(),
                "JUAL_BGN",
                "Menjual bangunan di " + detail
            );
        }
        uiManager.printMessage("Lanjut menggadaikan " + property->getName() + "? (y/n): ");
        if (!uiManager.readYesNo()) {
            uiManager.printMessage("Gadai dibatalkan.");
            return;
        }
    }

    const int mortgageValue = property->getMortgageValue();
    game.getPropertyManager().mortgageProperty(player, *property);
    uiManager.printMessage(property->getName() + " berhasil digadaikan.");
    uiManager.printMessage("Kamu menerima " + formatMoney(mortgageValue) + " dari Bank.");
    uiManager.printMessage("Uang kamu sekarang: " + formatMoney(player.getMoney()));
    uiManager.printMessage("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "GADAI",
        "Menggadaikan " + property->getName() + " (" + property->getCode() + ") senilai " + formatMoney(mortgageValue)
    );
}

void CommandController::handleRedeem() {
    Player& player = game.getCurrentPlayer();
    vector<OwnableTile*> properties = game.getPropertyManager().getRedeemableProperties(game, player);

    vector<string> groups;
    vector<string> names;
    vector<string> codes;
    vector<string> valueLabels;
    vector<int> values;
    vector<string> statuses;
    for (OwnableTile* property : properties) {
        if (property == nullptr) {
            continue;
        }
        groups.push_back(propertyGroupLabel(*property));
        names.push_back(property->getName());
        codes.push_back(property->getCode());
        valueLabels.push_back("Harga Tebus");
        values.push_back(game.getPropertyManager().getRedeemCost(*property));
        statuses.push_back(ownershipStatusText(property->getOwnershipStatus()));
    }

    uiManager.printRedeemOptions(player.getMoney(), groups, names, codes, valueLabels, values, statuses);
    if (properties.empty()) {
        return;
    }

    const int choice = uiManager.readRedeemChoice(static_cast<int>(properties.size()));
    if (choice == 0) {
        uiManager.printMessage("Tebus dibatalkan.");
        return;
    }
    if (choice < 1 || choice > static_cast<int>(properties.size())) {
        uiManager.printError("Pilihan properti tidak valid.");
        return;
    }

    OwnableTile* property = properties[static_cast<size_t>(choice - 1)];
    const int redeemCost = game.getPropertyManager().getRedeemCost(*property);
    if (player.getMoney() < redeemCost) {
        uiManager.printError("Uang kamu tidak cukup untuk menebus " + property->getName() + ".");
        uiManager.printError(
            "Harga tebus: " + formatMoney(redeemCost) +
            " | Uang kamu: " + formatMoney(player.getMoney())
        );
        return;
    }

    game.getPropertyManager().redeemProperty(player, *property);
    uiManager.printMessage(property->getName() + " berhasil ditebus!");
    uiManager.printMessage("Kamu membayar " + formatMoney(redeemCost) + " ke Bank.");
    uiManager.printMessage("Uang kamu sekarang: " + formatMoney(player.getMoney()));
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "TEBUS",
        "Menebus " + property->getName() + " (" + property->getCode() + ") seharga " + formatMoney(redeemCost)
    );
}

void CommandController::handleBuild() {
    Player& player = game.getCurrentPlayer();
    vector<string> groups = game.getPropertyManager().getBuildableColorGroups(game, player);
    uiManager.printBuildOptions(player.getMoney(), groups);
    if (groups.empty()) {
        return;
    }

    const int groupChoice = uiManager.readBuildGroupChoice(static_cast<int>(groups.size()));
    if (groupChoice == 0) {
        uiManager.printMessage("Bangun dibatalkan.");
        return;
    }
    if (groupChoice < 1 || groupChoice > static_cast<int>(groups.size())) {
        uiManager.printError("Pilihan color group tidak valid.");
        return;
    }

    const string selectedGroup = groups[static_cast<size_t>(groupChoice - 1)];
    vector<StreetTile*> streets = game.getPropertyManager().getBuildableStreets(game, player, selectedGroup);
    vector<string> names;
    vector<string> codes;
    vector<string> buildingStatuses;
    vector<string> buildStatuses;

    for (StreetTile* street : streets) {
        if (street == nullptr) {
            continue;
        }
        names.push_back(street->getName());
        codes.push_back(street->getCode());
        buildingStatuses.push_back(buildingText(*street));
        string buildStatus = "dapat dibangun";
        if (street->getBuildingLevel() >= 4) buildStatus = "siap upgrade ke hotel";
        buildStatuses.push_back(buildStatus);
    }

    uiManager.printBuildableTiles(selectedGroup, names, codes, buildingStatuses, buildStatuses);
    if (streets.empty()) {
        return;
    }

    const int tileChoice = uiManager.readBuildTileChoice(static_cast<int>(streets.size()));
    if (tileChoice == 0) {
        uiManager.printMessage("Bangun dibatalkan.");
        return;
    }
    if (tileChoice < 1 || tileChoice > static_cast<int>(streets.size())) {
        uiManager.printError("Pilihan petak tidak valid.");
        return;
    }

    StreetTile* street = streets[static_cast<size_t>(tileChoice - 1)];
    const int oldLevel = street->getBuildingLevel();
    int buildCost = street->getHouseBuildCost();
    if (oldLevel >= 4) buildCost = street->getHotelBuildCost();
    if (oldLevel >= 4) {
        uiManager.printMessage("Upgrade ke hotel? Biaya: " + formatMoney(buildCost) + " (y/n): ");
        if (!uiManager.readYesNo()) {
            uiManager.printMessage("Bangun dibatalkan.");
            return;
        }
    }
    game.getPropertyManager().buildOnStreet(game.getBoard(), player, *street);
    if (oldLevel >= 4) {
        uiManager.printMessage(street->getName() + " di-upgrade ke Hotel!");
    } else {
        uiManager.printMessage(
            "Kamu membangun 1 rumah di " + street->getName() +
            ". Biaya: " + formatMoney(buildCost)
        );
    }
    uiManager.printMessage("Uang tersisa: " + formatMoney(player.getMoney()));
    string buildingDetail = "Membangun 1 rumah";
    if (oldLevel >= 4) buildingDetail = "Membangun hotel";
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "BANGUN",
        buildingDetail + " di " + street->getName() + " (" + street->getCode() + ") seharga " + formatMoney(buildCost)
    );
}

void CommandController::handleSave(const string& filename) {
    ifstream existingFile(filename.c_str());
    if (existingFile.good() && !uiManager.confirmOverwrite(filename)) {
        uiManager.printMessage("Simpan dibatalkan.");
        return;
    }

    uiManager.printMessage("Menyimpan permainan...");
    game.getSaveLoadManager().saveGame(game, filename);
    uiManager.printMessage("Permainan berhasil disimpan ke: " + filename);
}

void CommandController::handlePrintLog(int recentCount) {
    vector<string> formattedLogs;

    if (recentCount < 0) {
        const vector<LogManager::LogEntry>& logs = game.getLogManager().getLogs();
        for (const LogManager::LogEntry& log : logs) {
            formattedLogs.push_back(log.format());
        }
        uiManager.printLogs(formattedLogs);
        return;
    }

    vector<LogManager::LogEntry> logs = game.getLogManager().getRecentLogs(recentCount);
    for (const LogManager::LogEntry& log : logs) {
        formattedLogs.push_back(log.format());
    }
    uiManager.printRecentLogs(formattedLogs, recentCount);
}

void CommandController::handleUseCard() {
    Player& player = game.getCurrentPlayer();
    if (player.hasUsedHandCardThisTurn()) {
        uiManager.printError("Kamu sudah menggunakan kartu kemampuan pada giliran ini!");
        uiManager.printError("Penggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.");
        return;
    }

    vector<shared_ptr<HandCard>> cards = game.getCardManager().getHandCards(player);
    if (cards.empty()) {
        uiManager.printMessage("Kamu tidak memiliki kartu kemampuan.");
        return;
    }

    vector<string> cardNames;
    vector<string> cardDescriptions;
    for (const shared_ptr<HandCard>& card : cards) {
        if (card != nullptr) {
            cardNames.push_back(card->getName());
            cardDescriptions.push_back(card->getDescription());
        } else {
            cardNames.push_back("Kartu tidak dikenal");
            cardDescriptions.push_back("");
        }
    }

    uiManager.printAbilityCardOptions(cardNames, cardDescriptions);
    const int choice = uiManager.readAbilityCardChoice(static_cast<int>(cards.size()));
    if (choice == 0) {
        uiManager.printMessage("Penggunaan kartu dibatalkan.");
        return;
    }
    if (choice < 1 || choice > static_cast<int>(cards.size())) {
        uiManager.printMessage("Penggunaan kartu dibatalkan.");
        return;
    }
    game.getCardManager().useHandCard(game, player, choice - 1);
    player.setUsedHandCardThisTurn(true);
    game.getLogManager().addLog(game.getCurrentTurn(), player.getUsername(), "KARTU", cardNames[choice - 1]);
}

int CommandController::parseRecentLogCount(const string& input) const {
    istringstream stream(input);
    string command;
    int count = -1;
    stream >> command;
    if (stream >> count) {
        return max(0, count);
    }
    return -1;
}
