#include "model/managers/SaveLoadManager.hpp"

#include "model/Game.hpp"
#include "model/NimonException.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/UtilityTile.hpp"

#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

SaveLoadManager::SaveLoadManager() {}
SaveLoadManager::SaveLoadManager(const SaveLoadManager&) {}
SaveLoadManager::~SaveLoadManager() {}
SaveLoadManager& SaveLoadManager::operator=(const SaveLoadManager&) { return *this; }

static string statusToStr(PlayerStatus s) {
    switch (s) {
        case PlayerStatus::JAILED:   return "JAILED";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
        default:                     return "ACTIVE";
    }
}

static PlayerStatus strToStatus(const string& s) {
    if (s == "JAILED")   return PlayerStatus::JAILED;
    if (s == "BANKRUPT") return PlayerStatus::BANKRUPT;
    return PlayerStatus::ACTIVE;
}

static string ownershipToStr(OwnershipStatus s) {
    switch (s) {
        case OwnershipStatus::OWNED:     return "OWNED";
        case OwnershipStatus::MORTGAGED: return "MORTGAGED";
        default:                         return "BANK";
    }
}

static OwnershipStatus strToOwnership(const string& s) {
    if (s == "OWNED")     return OwnershipStatus::OWNED;
    if (s == "MORTGAGED") return OwnershipStatus::MORTGAGED;
    return OwnershipStatus::BANK;
}

static Player* findPlayer(vector<Player>& players, const string& username) {
    for (Player& p : players) {
        if (p.getUsername() == username) return &p;
    }
    return nullptr;
}

static map<string, string> readSections(istream& in) {
    map<string, string> sections;
    string currentSection;
    string accumulated;
    string line;
    while (getline(in, line)) {
        if (line.rfind("SECTION ", 0) == 0) {
            if (!currentSection.empty()) {
                sections[currentSection] = accumulated;
            }
            currentSection = line.substr(8);
            accumulated.clear();
        } else {
            accumulated += line + "\n";
        }
    }
    if (!currentSection.empty()) {
        sections[currentSection] = accumulated;
    }
    return sections;
}

void SaveLoadManager::saveGame(const Game& game, const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        throw FileException("Gagal membuka file untuk disimpan: " + filename);
    }
    savePlayers(game, out);
    saveTurnState(game, out);
    saveProperties(game, out);
    saveDecks(game, out);
    saveLogs(game, out);
}

void SaveLoadManager::loadGame(Game& game, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw FileException("Gagal membuka file save: " + filename);
    }

    map<string, string> sections = readSections(file);

    game.getConfigManager().loadAllConfigs();
    game.getBoard().initializeBoard(game.getConfigManager());
    game.getCardManager().initializeDecks();

    if (sections.count("PLAYERS")) {
        istringstream ss(sections["PLAYERS"]);
        loadPlayers(game, ss);
    }
    if (sections.count("TURN_STATE")) {
        istringstream ss(sections["TURN_STATE"]);
        loadTurnState(game, ss);
    }
    if (sections.count("PROPERTIES")) {
        istringstream ss(sections["PROPERTIES"]);
        loadProperties(game, ss);
    }
    if (sections.count("DECKS")) {
        istringstream ss(sections["DECKS"]);
        loadDecks(game, ss);
    }
    if (sections.count("LOGS")) {
        istringstream ss(sections["LOGS"]);
        loadLogs(game, ss);
    }
}

void SaveLoadManager::savePlayers(const Game& game, ostream& out) {
    const vector<Player>& players = game.getPlayers();
    out << "SECTION PLAYERS\n";
    out << "count " << players.size() << "\n";
    for (const Player& p : players) {
        out << p.getUsername() << " "
            << p.getMoney() << " "
            << p.getPosition() << " "
            << statusToStr(p.getStatus()) << " "
            << p.getFailedJailRolls() << " "
            << (p.isUsedHandCardThisTurn() ? 1 : 0) << " "
            << (p.isShieldActive() ? 1 : 0) << " "
            << p.getDiscountPercent() << " "
            << p.getDiscountDuration() << "\n";
    }
}

void SaveLoadManager::loadPlayers(Game& game, istream& in) {
    vector<Player>& players = game.getPlayers();
    players.clear();

    string line;
    int count = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ls(line);
        string key;
        ls >> key;
        if (key == "count") {
            ls >> count;
            break;
        }
    }

    for (int i = 0; i < count && getline(in, line); ) {
        if (line.empty()) continue;
        istringstream ls(line);

        string username, statusStr;
        int money, position, failedJailRolls, discountPercent, discountDuration;
        int usedHandCardThisTurnInt, shieldActiveInt;

        if (!(ls >> username >> money >> position >> statusStr
                 >> failedJailRolls >> usedHandCardThisTurnInt
                 >> shieldActiveInt >> discountPercent >> discountDuration)) {
            continue;
        }

        Player p(
            username, money, position,
            strToStatus(statusStr),
            failedJailRolls,
            usedHandCardThisTurnInt != 0,
            shieldActiveInt != 0,
            discountPercent,
            discountDuration
        );
        players.push_back(p);
        i++;
    }
}

void SaveLoadManager::saveTurnState(const Game& game, ostream& out) {
    const GameContext& ctx = game.getGameContext();
    const TurnManager& tm = game.getTurnManager();
    const vector<int>& order = tm.getTurnOrder();

    out << "SECTION TURN_STATE\n";
    out << "currentTurn " << ctx.getCurrentTurn() << "\n";
    out << "maxTurn " << ctx.getMaxTurn() << "\n";
    out << "currentPlayerIndex " << tm.getCurrentPlayerIndex() << "\n";
    out << "turnOrderCount " << order.size() << "\n";
    out << "turnOrder";
    for (int idx : order) out << " " << idx;
    out << "\n";
    out << "rolledThisTurn " << (tm.isRolledThisTurn() ? 1 : 0) << "\n";
    out << "consecutiveDoubles " << tm.getConsecutiveDoubles() << "\n";
    out << "turnCount " << tm.getTurnCount() << "\n";
}

void SaveLoadManager::loadTurnState(Game& game, istream& in) {
    GameContext& ctx = game.getGameContext();
    TurnManager& tm = game.getTurnManager();

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ls(line);
        string key;
        ls >> key;

        if (key == "currentTurn") {
            int v; ls >> v; ctx.setCurrentTurn(v);
        } else if (key == "maxTurn") {
            int v; ls >> v; ctx.setMaxTurn(v);
        } else if (key == "currentPlayerIndex") {
            int v; ls >> v; tm.setCurrentPlayerIndex(v);
        } else if (key == "turnOrderCount") {
        } else if (key == "turnOrder") {
            vector<int> order;
            int idx;
            while (ls >> idx) order.push_back(idx);
            tm.setTurnOrder(order);
        } else if (key == "rolledThisTurn") {
            int v; ls >> v; tm.setRolledThisTurn(v != 0);
        } else if (key == "consecutiveDoubles") {
            int v; ls >> v; tm.setConsecutiveDoubles(v);
        } else if (key == "turnCount") {
            int v; ls >> v; tm.setTurnCount(v);
        }
    }
}

void SaveLoadManager::saveProperties(const Game& game, ostream& out) {
    out << "SECTION PROPERTIES\n";
    const vector<shared_ptr<Tile>>& tiles = game.getBoard().getTiles();
    for (const shared_ptr<Tile>& tile : tiles) {
        shared_ptr<StreetTile> street = dynamic_pointer_cast<StreetTile>(tile);
        if (street) {
            const Player* owner = street->getOwner();
            out << "STREET "
                << street->getCode() << " "
                << (owner ? owner->getUsername() : "-") << " "
                << ownershipToStr(street->getOwnershipStatus()) << " "
                << street->getBuildingLevel() << " "
                << street->getFestivalMultiplier() << " "
                << street->getFestivalDuration() << "\n";
            continue;
        }

        shared_ptr<RailroadTile> railroad = dynamic_pointer_cast<RailroadTile>(tile);
        if (railroad) {
            const Player* owner = railroad->getOwner();
            out << "RAILROAD "
                << railroad->getCode() << " "
                << (owner ? owner->getUsername() : "-") << " "
                << ownershipToStr(railroad->getOwnershipStatus()) << "\n";
            continue;
        }

        shared_ptr<UtilityTile> utility = dynamic_pointer_cast<UtilityTile>(tile);
        if (utility) {
            const Player* owner = utility->getOwner();
            out << "UTILITY "
                << utility->getCode() << " "
                << (owner ? owner->getUsername() : "-") << " "
                << ownershipToStr(utility->getOwnershipStatus()) << "\n";
            continue;
        }
    }
}

void SaveLoadManager::loadProperties(Game& game, istream& in) {
    vector<Player>& players = game.getPlayers();
    Board& board = game.getBoard();

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ls(line);
        string type;
        ls >> type;

        if (type == "STREET") {
            string code, ownerStr, ownershipStr;
            int buildingLevel, festMultiplier, festDuration;
            if (!(ls >> code >> ownerStr >> ownershipStr
                     >> buildingLevel >> festMultiplier >> festDuration)) continue;

            shared_ptr<Tile> tile = board.getTileByCode(code);
            if (!tile) continue;
            shared_ptr<StreetTile> street = dynamic_pointer_cast<StreetTile>(tile);
            if (!street) continue;

            OwnershipStatus os = strToOwnership(ownershipStr);
            street->setOwnershipStatus(os);
            if (ownerStr != "-" && os != OwnershipStatus::BANK) {
                street->setOwner(findPlayer(players, ownerStr));
            } else {
                street->setOwner(nullptr);
            }
            street->setBuildingLevel(buildingLevel);
            street->setFestivalState(festMultiplier, festDuration);

        } else if (type == "RAILROAD") {
            string code, ownerStr, ownershipStr;
            if (!(ls >> code >> ownerStr >> ownershipStr)) continue;

            shared_ptr<Tile> tile = board.getTileByCode(code);
            if (!tile) continue;
            shared_ptr<RailroadTile> railroad = dynamic_pointer_cast<RailroadTile>(tile);
            if (!railroad) continue;

            OwnershipStatus os = strToOwnership(ownershipStr);
            railroad->setOwnershipStatus(os);
            if (ownerStr != "-" && os != OwnershipStatus::BANK) {
                railroad->setOwner(findPlayer(players, ownerStr));
            } else {
                railroad->setOwner(nullptr);
            }

        } else if (type == "UTILITY") {
            string code, ownerStr, ownershipStr;
            if (!(ls >> code >> ownerStr >> ownershipStr)) continue;

            shared_ptr<Tile> tile = board.getTileByCode(code);
            if (!tile) continue;
            shared_ptr<UtilityTile> utility = dynamic_pointer_cast<UtilityTile>(tile);
            if (!utility) continue;

            OwnershipStatus os = strToOwnership(ownershipStr);
            utility->setOwnershipStatus(os);
            if (ownerStr != "-" && os != OwnershipStatus::BANK) {
                utility->setOwner(findPlayer(players, ownerStr));
            } else {
                utility->setOwner(nullptr);
            }
        }
    }
}

void SaveLoadManager::saveDecks(const Game& game, ostream& out) {
    out << "SECTION DECKS\n";
}

void SaveLoadManager::loadDecks(Game& game, istream& in) {
    (void)in;
}

void SaveLoadManager::saveLogs(const Game& game, ostream& out) {
    const vector<LogManager::LogEntry>& logs = game.getLogManager().getLogs();
    out << "SECTION LOGS\n";
    out << "count " << logs.size() << "\n";
    for (const LogManager::LogEntry& entry : logs) {
        out << entry.getTurnNumber() << "|"
            << entry.getUsername() << "|"
            << entry.getActionType() << "|"
            << entry.getDetail() << "\n";
    }
}

void SaveLoadManager::loadLogs(Game& game, istream& in) {
    LogManager& lm = game.getLogManager();

    string line;
    int count = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ls(line);
        string key;
        ls >> key;
        if (key == "count") {
            ls >> count;
            break;
        }
    }

    for (int i = 0; i < count && getline(in, line); ) {
        if (line.empty()) continue;

        size_t p1 = line.find('|');
        if (p1 == string::npos) continue;
        size_t p2 = line.find('|', p1 + 1);
        if (p2 == string::npos) continue;
        size_t p3 = line.find('|', p2 + 1);
        if (p3 == string::npos) continue;

        int turnNumber = stoi(line.substr(0, p1));
        string username   = line.substr(p1 + 1, p2 - p1 - 1);
        string actionType = line.substr(p2 + 1, p3 - p2 - 1);
        string detail     = line.substr(p3 + 1);

        lm.addLog(turnNumber, username, actionType, detail);
        i++;
    }
}