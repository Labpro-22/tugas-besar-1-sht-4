#include "model/managers/SaveLoadManager.hpp"

#include "model/Game.hpp"
#include "model/NimonException.hpp"
#include "model/cards/DemolitionCard.hpp"
#include "model/cards/DiscountCard.hpp"
#include "model/cards/LassoCard.hpp"
#include "model/cards/MoveCard.hpp"
#include "model/cards/ShieldCard.hpp"
#include "model/cards/TeleportCard.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/UtilityTile.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

SaveLoadManager::SaveLoadManager() = default;
SaveLoadManager::SaveLoadManager(const SaveLoadManager&) {}
SaveLoadManager::~SaveLoadManager() {}
SaveLoadManager& SaveLoadManager::operator=(const SaveLoadManager&) { return *this; }

static string trim(const string& value) {
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

static bool readNonEmptyLine(istream& in, string& line) {
    while (getline(in, line)) {
        line = trim(line);
        if (!line.empty()) return true;
    }
    return false;
}

static string readRequiredLine(istream& in, const string& sectionName) {
    string line;
    if (!readNonEmptyLine(in, line)) {
        throw FileException("Format file save tidak valid pada section: " + sectionName);
    }
    return line;
}

static vector<string> splitWords(const string& line) {
    vector<string> tokens;
    istringstream ss(line);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

static bool tryParseInt(const string& text, int& value) {
    if (text.empty()) return false;

    char* endPtr = nullptr;
    long parsed = strtol(text.c_str(), &endPtr, 10);
    if (endPtr == text.c_str() || *endPtr != '\0') return false;

    value = static_cast<int>(parsed);
    return true;
}

static string toLowerCase(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(tolower(ch));
    });
    return s;
}

static int parseCountLine(istream& in, const string& sectionName) {
    string line = readRequiredLine(in, sectionName);
    vector<string> parts = splitWords(line);
    if (parts.empty()) {
        throw FileException("Jumlah data kosong pada section: " + sectionName);
    }

    int count = 0;
    if (!tryParseInt(parts[0], count) || count < 0) {
        throw FileException("Jumlah data tidak valid pada section: " + sectionName);
    }
    return count;
}

static bool hasTxtExtension(const string& filename) {
    if (filename.size() < 4) return false;
    string suffix = filename.substr(filename.size() - 4);
    return toLowerCase(suffix) == ".txt";
}

static string statusToStr(PlayerStatus s) {
    switch (s) {
        case PlayerStatus::JAILED:   return "JAILED";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
        default:                     return "ACTIVE";
    }
}

static PlayerStatus strToStatus(const string& s) {
    if (s == "ACTIVE")   return PlayerStatus::ACTIVE;
    if (s == "JAILED")   return PlayerStatus::JAILED;
    if (s.rfind("JAILED_", 0) == 0) return PlayerStatus::JAILED;
    if (s == "BANKRUPT") return PlayerStatus::BANKRUPT;
    throw FileException("Status pemain tidak valid: " + s);
}

static int jailAttemptsFromStatus(const string& s) {
    if (s.rfind("JAILED_", 0) != 0) {
        return 0;
    }

    int attempts = 0;
    if (!tryParseInt(s.substr(7), attempts)) {
        throw FileException("Status penjara tidak valid: " + s);
    }
    return max(0, min(3, attempts));
}

static string ownershipToStr(OwnershipStatus s) {
    switch (s) {
        case OwnershipStatus::OWNED:     return "OWNED";
        case OwnershipStatus::MORTGAGED: return "MORTGAGED";
        default:                         return "BANK";
    }
}

static OwnershipStatus strToOwnership(const string& s) {
    if (s == "BANK")      return OwnershipStatus::BANK;
    if (s == "OWNED")     return OwnershipStatus::OWNED;
    if (s == "MORTGAGED") return OwnershipStatus::MORTGAGED;
    throw FileException("Status kepemilikan properti tidak valid: " + s);
}

static Player* findPlayer(vector<Player>& players, const string& username) {
    for (Player& p : players) {
        if (p.getUsername() == username) return &p;
    }
    return nullptr;
}

static shared_ptr<HandCard> makeHandCard(const string& type, int value, int duration) {
    if (type == "MoveCard") {
        int steps = (value > 0) ? value : 1;
        return make_shared<MoveCard>("MoveCard", "Maju " + to_string(steps) + " Petak", false, steps);
    }
    if (type == "DiscountCard") {
        int percent = (value >= 0) ? value : 0;
        int remainingDuration = (duration > 0) ? duration : 1;
        return make_shared<DiscountCard>(
            "DiscountCard",
            "Diskon " + to_string(percent) + "%",
            false,
            percent,
            remainingDuration
        );
    }
    if (type == "ShieldCard")
        return make_shared<ShieldCard>("ShieldCard", "Kebal tagihan atau sanksi selama 1 giliran", false, 1);
    if (type == "TeleportCard")
        return make_shared<TeleportCard>("TeleportCard", "Pindah ke petak manapun", false);
    if (type == "LassoCard")
        return make_shared<LassoCard>("LassoCard", "Menarik satu lawan ke posisi Anda", false);
    if (type == "DemolitionCard")
        return make_shared<DemolitionCard>("DemolitionCard", "Menghancurkan satu properti milik lawan", false);
    return nullptr;
}

void SaveLoadManager::saveGame(Game& game, const string& filename) {
    if (!hasTxtExtension(filename)) {
        throw FileException("File save harus berekstensi .txt");
    }

    if (!game.isGameRunning()) {
        throw InvalidActionException("Save hanya dapat dilakukan saat permainan sedang berjalan.");
    }

    const TurnManager& tm = game.getTurnManager();
    if (tm.hasActionTakenThisTurn()) {
        throw InvalidActionException("Save hanya dapat dilakukan di awal giliran sebelum ada aksi.");
    }

    const vector<Player>& players = game.getPlayers();
    const vector<int>& order = tm.getTurnOrder();
    if (!players.empty() && !order.empty()) {
        int activeIdx = tm.getCurrentPlayerIndex();
        if (activeIdx >= 0 && activeIdx < static_cast<int>(order.size())) {
            int playerIdx = order[activeIdx];
            if (playerIdx >= 0 && playerIdx < static_cast<int>(players.size()) &&
                players[playerIdx].isUsedHandCardThisTurn()) {
                throw InvalidActionException("Save hanya boleh sebelum aksi pemain aktif dijalankan.");
            }
        }
    }

    ofstream out(filename);
    if (!out.is_open())
        throw FileException("Gagal membuka file untuk disimpan: " + filename);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        game.getCurrentPlayer().getUsername(),
        "SAVE",
        "Permainan disimpan ke " + filename
    );

    const GameContext& ctx = game.getGameContext();
    out << ctx.getCurrentTurn() << " " << ctx.getMaxTurn() << "\n";

    savePlayers(game, out);
    saveTurnState(game, out);
    saveProperties(game, out);
    saveDecks(game, out);
    saveLogs(game, out);
}

void SaveLoadManager::loadGame(Game& game, const string& filename) {
    if (!hasTxtExtension(filename)) {
        throw FileException("File load harus berekstensi .txt");
    }

    if (game.isGameRunning()) {
        throw InvalidActionException("Load hanya dapat dilakukan sebelum permainan dimulai.");
    }

    ifstream file(filename);
    if (!file.is_open())
        throw FileException("Gagal membuka file save: " + filename);

    game.getConfigManager().loadAllConfigs();
    game.getBoard().initializeBoard(game.getConfigManager());
    game.getCardManager().initializeDecks();

    {
        string line = readRequiredLine(file, "TURN_STATE");
        istringstream ss(line);
        int currentTurn = 0;
        int maxTurn = 0;
        if (!(ss >> currentTurn >> maxTurn)) {
            throw FileException("Header turn tidak valid.");
        }
        game.getGameContext().setCurrentTurn(currentTurn);
        game.getGameContext().setMaxTurn(maxTurn);
    }

    loadPlayers(game, file);
    loadTurnState(game, file);
    loadProperties(game, file);
    loadDecks(game, file);
    loadLogs(game, file);
}

void SaveLoadManager::savePlayers(const Game& game, ostream& out) {
    const vector<Player>& players = game.getPlayers();
    const Board& board = game.getBoard();

    out << players.size() << "\n";

    for (const Player& p : players) {
        string posCode = "GO";
        shared_ptr<Tile> tile = board.getTile(p.getPosition());
        if (tile) posCode = tile->getCode();

        string statusText = statusToStr(p.getStatus());
        if (p.getStatus() == PlayerStatus::JAILED) {
            statusText = "JAILED_" + to_string(max(0, min(3, p.getFailedJailRolls())));
        }

        out << p.getUsername() << " "
            << p.getMoney() << " "
            << posCode << " "
            << statusText << "\n";

        vector<shared_ptr<HandCard>> cards = p.getHandCards();
        int cardCount = 0;
        for (const shared_ptr<HandCard>& card : cards) {
            if (card != nullptr) cardCount++;
        }

        out << cardCount << "\n";
        for (const shared_ptr<HandCard>& card : cards) {
            if (!card) continue;
            const string& name = card->getName();
            if (name == "MoveCard") {
                auto mc = dynamic_pointer_cast<MoveCard>(card);
                out << "MoveCard " << (mc ? mc->getSteps() : 1) << "\n";
            } else if (name == "DiscountCard") {
                auto dc = dynamic_pointer_cast<DiscountCard>(card);
                out << "DiscountCard "
                    << (dc ? dc->getDiscountPercent() : 0) << " "
                    << (dc ? dc->getDuration() : 1) << "\n";
            } else {
                out << name << "\n";
            }
        }
    }
}

void SaveLoadManager::loadPlayers(Game& game, istream& in) {
    vector<Player>& players = game.getPlayers();
    players.clear();

    int count = parseCountLine(in, "STATE_PEMAIN");

    for (int i = 0; i < count; i++) {
        string line = readRequiredLine(in, "PLAYER_" + to_string(i + 1));
        vector<string> tokens = splitWords(line);
        if (tokens.size() < 4) {
            throw FileException("Format state pemain tidak valid pada pemain ke-" + to_string(i + 1));
        }

        string username = tokens[0];
        if (find_if(players.begin(), players.end(), [&](const Player& player) {
                return player.getUsername() == username;
            }) != players.end()) {
            throw FileException("Username pemain duplikat pada save file: " + username);
        }

        string tileCode = tokens[2];
        string statusStr = tokens[3];
        int money = 0;
        if (!tryParseInt(tokens[1], money)) {
            throw FileException("Uang pemain tidak valid pada pemain: " + username);
        }

        int position = game.getBoard().getTileIndex(tileCode);
        if (position <= 0) {
            throw FileException("Kode petak tidak valid untuk pemain: " + username);
        }

        PlayerStatus status = strToStatus(statusStr);
        int failedJailRolls = jailAttemptsFromStatus(statusStr);

        Player p(username, money, position, status, failedJailRolls, false, false, 0, 0);

        int numCards = parseCountLine(in, "PLAYER_CARD_COUNT_" + username);

        for (int j = 0; j < numCards; j++) {
            line = readRequiredLine(in, "PLAYER_CARD_" + username);
            vector<string> cardTokens = splitWords(line);
            if (cardTokens.empty()) {
                throw FileException("Format kartu pemain tidak valid untuk: " + username);
            }

            string type = cardTokens[0];
            int value = 0;
            int duration = 0;
            if (cardTokens.size() >= 2) {
                tryParseInt(cardTokens[1], value);
            }
            if (cardTokens.size() >= 3) {
                tryParseInt(cardTokens[2], duration);
            }

            auto card = makeHandCard(type, value, duration);
            if (!card) {
                throw FileException("Jenis kartu tidak dikenali: " + type);
            }
            p.addHandCard(card);
        }

        players.push_back(p);
    }
}

void SaveLoadManager::saveTurnState(const Game& game, ostream& out) {
    const TurnManager& tm = game.getTurnManager();
    const vector<Player>& players = game.getPlayers();
    vector<int> order = tm.getTurnOrder();

    vector<int> normalizedOrder;
    vector<bool> used(players.size(), false);
    for (int idx : order) {
        if (idx >= 0 && idx < static_cast<int>(players.size()) && !used[idx]) {
            normalizedOrder.push_back(idx);
            used[idx] = true;
        }
    }
    for (int i = 0; i < static_cast<int>(players.size()); i++) {
        if (!used[i]) normalizedOrder.push_back(i);
    }

    for (int i = 0; i < static_cast<int>(normalizedOrder.size()); i++) {
        if (i > 0) out << " ";
        out << players[normalizedOrder[i]].getUsername();
    }
    out << "\n";

    int activeOrderIndex = tm.getCurrentPlayerIndex();
    if (activeOrderIndex < 0 || activeOrderIndex >= static_cast<int>(normalizedOrder.size())) {
        activeOrderIndex = 0;
    }

    if (normalizedOrder.empty()) {
        out << "\n";
        return;
    }
    out << players[normalizedOrder[activeOrderIndex]].getUsername() << "\n";
}

void SaveLoadManager::loadTurnState(Game& game, istream& in) {
    TurnManager& tm = game.getTurnManager();
    vector<Player>& players = game.getPlayers();

    string line = readRequiredLine(in, "TURN_ORDER");
    vector<string> tokens = splitWords(line);

    vector<int> order;
    vector<bool> used(players.size(), false);
    for (const string& username : tokens) {
        for (int i = 0; i < static_cast<int>(players.size()); i++) {
            if (players[i].getUsername() == username && !used[i]) {
                order.push_back(i);
                used[i] = true;
                break;
            }
        }
    }

    for (int i = 0; i < static_cast<int>(players.size()); i++) {
        if (!used[i]) order.push_back(i);
    }

    tm.setTurnOrder(order);

    int activeIdx = 0;
    if (!order.empty()) {
        line = readRequiredLine(in, "ACTIVE_TURN");
        vector<string> activeTokens = splitWords(line);
        string activeUsername = activeTokens.empty() ? "" : activeTokens[0];

        for (int i = 0; i < static_cast<int>(order.size()); i++) {
            if (players[order[i]].getUsername() == activeUsername) {
                activeIdx = i;
                break;
            }
        }
    }

    tm.setCurrentPlayerIndex(activeIdx);
    tm.setRolledThisTurn(false);
    tm.setActionTakenThisTurn(false);
    tm.setConsecutiveDoubles(0);
    tm.setTurnCount(game.getGameContext().getCurrentTurn());
}

void SaveLoadManager::saveProperties(const Game& game, ostream& out) {
    const vector<shared_ptr<Tile>>& tiles = game.getBoard().getTiles();

    int count = 0;
    for (const shared_ptr<Tile>& tile : tiles) {
        if (dynamic_pointer_cast<StreetTile>(tile) ||
            dynamic_pointer_cast<RailroadTile>(tile) ||
            dynamic_pointer_cast<UtilityTile>(tile)) {
            count++;
        }
    }
    out << count << "\n";

    for (const shared_ptr<Tile>& tile : tiles) {
        shared_ptr<OwnableTile> ownable = dynamic_pointer_cast<OwnableTile>(tile);
        if (!ownable) continue;

        OwnershipStatus ownership = ownable->getOwnershipStatus();
        const Player* owner = ownable->getOwner();
        string ownerStr = (ownership == OwnershipStatus::BANK || owner == nullptr)
            ? "BANK"
            : owner->getUsername();

        shared_ptr<StreetTile> street = dynamic_pointer_cast<StreetTile>(tile);
        if (street) {
            int buildingLevel = street->getBuildingLevel();
            string nBuildings = (buildingLevel >= 5)
                ? "H"
                : to_string(max(0, buildingLevel));

            out << street->getCode() << " street "
                << ownerStr << " "
                << ownershipToStr(ownership) << " "
                << street->getFestivalMultiplier() << " "
                << street->getFestivalDuration() << " "
                << nBuildings << "\n";
            continue;
        }

        shared_ptr<RailroadTile> railroad = dynamic_pointer_cast<RailroadTile>(tile);
        if (railroad) {
            out << railroad->getCode() << " railroad "
                << ownerStr << " "
                << ownershipToStr(ownership) << " "
                << railroad->getFestivalMultiplier() << " "
                << railroad->getFestivalDuration() << " 0\n";
            continue;
        }

        shared_ptr<UtilityTile> utility = dynamic_pointer_cast<UtilityTile>(tile);
        if (utility) {
            out << utility->getCode() << " utility "
                << ownerStr << " "
                << ownershipToStr(ownership) << " "
                << utility->getFestivalMultiplier() << " "
                << utility->getFestivalDuration() << " 0\n";
            continue;
        }
    }
}

void SaveLoadManager::loadProperties(Game& game, istream& in) {
    vector<Player>& players = game.getPlayers();
    Board& board = game.getBoard();

    int count = parseCountLine(in, "STATE_PROPERTI");

    for (int i = 0; i < count; i++) {
        string line = readRequiredLine(in, "PROPERTY_" + to_string(i + 1));
        vector<string> fields = splitWords(line);
        if (fields.size() < 7) {
            throw FileException("Format state properti tidak valid.");
        }

        string code = fields[0];
        string type = toLowerCase(fields[1]);
        string ownerStr = fields[2];
        string ownershipStr = fields[3];
        string nBuildStr = fields[6];
        int fmult = 1;
        int fdur = 0;
        if (!tryParseInt(fields[4], fmult)) {
            throw FileException("FMULT tidak valid pada properti: " + code);
        }
        if (!tryParseInt(fields[5], fdur)) {
            throw FileException("FDUR tidak valid pada properti: " + code);
        }

        shared_ptr<Tile> tile = board.getTileByCode(code);
        if (!tile) {
            throw FileException("Kode petak properti tidak ditemukan: " + code);
        }

        OwnershipStatus os = strToOwnership(ownershipStr);
        Player* owner = nullptr;
        if (ownerStr != "BANK") {
            owner = findPlayer(players, ownerStr);
            if (!owner) {
                throw FileException("Pemilik properti tidak ditemukan: " + ownerStr);
            }
        }

        if (os == OwnershipStatus::BANK) {
            owner = nullptr;
        } else if (!owner) {
            throw FileException("Properti bukan milik BANK tetapi owner tidak valid: " + code);
        }

        if (type == "street") {
            shared_ptr<StreetTile> street = dynamic_pointer_cast<StreetTile>(tile);
            if (!street) {
                throw FileException("Tile " + code + " bukan street.");
            }

            street->setOwnershipStatus(os);
            street->setOwner(owner);
            int buildLevel = 0;
            if (nBuildStr == "H" || nBuildStr == "h") {
                buildLevel = 5;
            } else if (!tryParseInt(nBuildStr, buildLevel)) {
                throw FileException("Jumlah bangunan tidak valid pada properti: " + code);
            }

            street->setBuildingLevel(buildLevel);
            street->setFestivalState(fmult, fdur);
        } else if (type == "railroad") {
            shared_ptr<RailroadTile> railroad = dynamic_pointer_cast<RailroadTile>(tile);
            if (!railroad) {
                throw FileException("Tile " + code + " bukan railroad.");
            }

            railroad->setOwnershipStatus(os);
            railroad->setOwner(owner);
            railroad->setFestivalState(fmult, fdur);
        } else if (type == "utility") {
            shared_ptr<UtilityTile> utility = dynamic_pointer_cast<UtilityTile>(tile);
            if (!utility) {
                throw FileException("Tile " + code + " bukan utility.");
            }

            utility->setOwnershipStatus(os);
            utility->setOwner(owner);
            utility->setFestivalState(fmult, fdur);
        } else {
            throw FileException("Jenis properti tidak valid: " + type);
        }
    }
}

void SaveLoadManager::saveDecks(const Game& game, ostream& out) {
    const CardDeck<HandCard>& handDeck = game.getCardManager().getHandDeck();
    const auto& drawPile = handDeck.getDrawPile();
    const auto& discardPile = handDeck.getDiscardPile();

    int cardCount = 0;
    for (const shared_ptr<HandCard>& card : drawPile) {
        if (card != nullptr) cardCount++;
    }
    for (const shared_ptr<HandCard>& card : discardPile) {
        if (card != nullptr) cardCount++;
    }

    out << cardCount << "\n";
    for (const shared_ptr<HandCard>& card : drawPile) {
        if (!card) continue;
        out << card->getName() << "\n";
    }
    for (const shared_ptr<HandCard>& card : discardPile) {
        if (!card) continue;
        out << card->getName() << "\n";
    }
}

void SaveLoadManager::loadDecks(Game& game, istream& in) {
    CardDeck<HandCard>& handDeck = game.getCardManager().getHandDeck();
    handDeck.clear();

    int count = parseCountLine(in, "STATE_DECK");

    for (int i = 0; i < count; i++) {
        string line = readRequiredLine(in, "DECK_CARD_" + to_string(i + 1));
        vector<string> cardTokens = splitWords(line);
        if (cardTokens.empty()) {
            throw FileException("Format kartu deck tidak valid.");
        }

        string type = cardTokens[0];
        int value = 0;
        int duration = 0;
        if (cardTokens.size() >= 2) {
            tryParseInt(cardTokens[1], value);
        }
        if (cardTokens.size() >= 3) {
            tryParseInt(cardTokens[2], duration);
        }

        auto card = makeHandCard(type, value, duration);
        if (!card) {
            throw FileException("Jenis kartu deck tidak dikenali: " + type);
        }
        handDeck.addCard(card);
    }
}

void SaveLoadManager::saveLogs(const Game& game, ostream& out) {
    const vector<LogManager::LogEntry>& logs = game.getLogManager().getLogs();
    out << logs.size() << "\n";
    for (const LogManager::LogEntry& entry : logs) {
        out << entry.getTurnNumber() << " "
            << entry.getUsername() << " "
            << entry.getActionType() << " "
            << entry.getDetail() << "\n";
    }
}

void SaveLoadManager::loadLogs(Game& game, istream& in) {
    LogManager& lm = game.getLogManager();
    lm = LogManager();

    int count = parseCountLine(in, "STATE_LOG");

    for (int i = 0; i < count; i++) {
        string line = readRequiredLine(in, "LOG_" + to_string(i + 1));
        istringstream ls(line);
        int turnNumber = 0;
        string username, actionType, detail;
        if (!(ls >> turnNumber >> username >> actionType)) {
            throw FileException("Format log tidak valid pada entri ke-" + to_string(i + 1));
        }
        getline(ls, detail);
        detail = trim(detail);
        lm.addLog(turnNumber, username, actionType, detail);
    }
}
