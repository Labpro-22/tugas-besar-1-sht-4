#pragma once

#include "raylib.h"

#include <array>
#include <deque>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace view::raylibgui {
inline constexpr int kGridCells = 11;
inline constexpr int kMaxPlayers = 4;
inline constexpr int kMinPlayers = 2;
inline constexpr int kMaxLogEntries = 48;
inline constexpr int kChanceDeckKey = 1;
inline constexpr int kCommunityDeckKey = 2;

class Theme {
public:
    Theme() = default;
    Theme(
        Color paper,
        Color paperSoft,
        Color ink,
        Color inkMuted,
        Color teal,
        Color coral,
        Color gold,
        Color sage,
        Color navy,
        Color danger,
        Color success,
        Color shadow
    )
        : paper_(paper),
          paperSoft_(paperSoft),
          ink_(ink),
          inkMuted_(inkMuted),
          teal_(teal),
          coral_(coral),
          gold_(gold),
          sage_(sage),
          navy_(navy),
          danger_(danger),
          success_(success),
          shadow_(shadow) {}

    Color getPaper() const { return paper_; }
    void setPaper(Color value) { paper_ = value; }
    Color getPaperSoft() const { return paperSoft_; }
    void setPaperSoft(Color value) { paperSoft_ = value; }
    Color getInk() const { return ink_; }
    void setInk(Color value) { ink_ = value; }
    Color getInkMuted() const { return inkMuted_; }
    void setInkMuted(Color value) { inkMuted_ = value; }
    Color getTeal() const { return teal_; }
    void setTeal(Color value) { teal_ = value; }
    Color getCoral() const { return coral_; }
    void setCoral(Color value) { coral_ = value; }
    Color getGold() const { return gold_; }
    void setGold(Color value) { gold_ = value; }
    Color getSage() const { return sage_; }
    void setSage(Color value) { sage_ = value; }
    Color getNavy() const { return navy_; }
    void setNavy(Color value) { navy_ = value; }
    Color getDanger() const { return danger_; }
    void setDanger(Color value) { danger_ = value; }
    Color getSuccess() const { return success_; }
    void setSuccess(Color value) { success_ = value; }
    Color getShadow() const { return shadow_; }
    void setShadow(Color value) { shadow_ = value; }

private:
    Color paper_{};
    Color paperSoft_{};
    Color ink_{};
    Color inkMuted_{};
    Color teal_{};
    Color coral_{};
    Color gold_{};
    Color sage_{};
    Color navy_{};
    Color danger_{};
    Color success_{};
    Color shadow_{};
};

enum class Screen {
    MainMenu,
    NewGame,
    LoadGame,
    Tutorial,
    Gameplay,
};

enum class NewGameConfigMode {
    DefaultConfig,
    CustomConfig,
};

enum class OverlayType {
    None,
    TileDetail,
    Purchase,
    Auction,
    IncomeTax,
    LuxuryTax,
    Festival,
    Jail,
    Cards,
    Portfolio,
    Build,
    Mortgage,
    Redeem,
    Save,
    Logs,
    SetDice,
    Help,
    ForceDrop,
    Liquidation,
    CardDraw,
    GameOver,
};

enum class TileKind {
    Go,
    Street,
    Railroad,
    Utility,
    Chance,
    CommunityChest,
    IncomeTax,
    LuxuryTax,
    Festival,
    Jail,
    GoToJail,
    FreeParking,
};

enum class CardKind {
    Instant,
    Hand,
};

enum class CardEffect {
    GainMoney,
    LoseMoney,
    MoveToGo,
    MoveBackThree,
    GoToJail,
    TeleportAnywhere,
    ActivateShield,
    ActivateDiscount,
};

class CardInfo {
public:
    CardInfo() = default;
    CardInfo(
        std::string title,
        std::string description,
        CardKind kind = CardKind::Instant,
        CardEffect effect = CardEffect::GainMoney,
        int magnitude = 0,
        Color accent = WHITE
    )
        : title_(std::move(title)),
          description_(std::move(description)),
          kind_(kind),
          effect_(effect),
          magnitude_(magnitude),
          accent_(accent) {}

    const std::string& getTitle() const { return title_; }
    void setTitle(std::string value) { title_ = std::move(value); }
    const std::string& getDescription() const { return description_; }
    void setDescription(std::string value) { description_ = std::move(value); }
    CardKind getKind() const { return kind_; }
    void setKind(CardKind value) { kind_ = value; }
    CardEffect getEffect() const { return effect_; }
    void setEffect(CardEffect value) { effect_ = value; }
    int getMagnitude() const { return magnitude_; }
    void setMagnitude(int value) { magnitude_ = value; }
    Color getAccent() const { return accent_; }
    void setAccent(Color value) { accent_ = value; }

private:
    std::string title_;
    std::string description_;
    CardKind kind_ = CardKind::Instant;
    CardEffect effect_ = CardEffect::GainMoney;
    int magnitude_ = 0;
    Color accent_ = WHITE;
};

class TileInfo {
public:
    TileInfo() = default;
    TileInfo(
        int index,
        std::string code,
        std::string name,
        std::string flavor,
        std::string group,
        TileKind kind = TileKind::Go,
        Color accent = WHITE,
        int price = 0,
        int mortgageValue = 0,
        int baseRent = 0,
        int houseCost = 0,
        int hotelCost = 0,
        int ownerIndex = -1,
        int buildings = 0,
        int festivalTurns = 0,
        bool mortgaged = false
    )
        : index_(index),
          code_(std::move(code)),
          name_(std::move(name)),
          flavor_(std::move(flavor)),
          group_(std::move(group)),
          kind_(kind),
          accent_(accent),
          price_(price),
          mortgageValue_(mortgageValue),
          baseRent_(baseRent),
          houseCost_(houseCost),
          hotelCost_(hotelCost),
          ownerIndex_(ownerIndex),
          buildings_(buildings),
          festivalTurns_(festivalTurns),
          mortgaged_(mortgaged) {}

    int getIndex() const { return index_; }
    void setIndex(int value) { index_ = value; }
    const std::string& getCode() const { return code_; }
    void setCode(std::string value) { code_ = std::move(value); }
    const std::string& getName() const { return name_; }
    void setName(std::string value) { name_ = std::move(value); }
    const std::string& getFlavor() const { return flavor_; }
    void setFlavor(std::string value) { flavor_ = std::move(value); }
    const std::string& getGroup() const { return group_; }
    void setGroup(std::string value) { group_ = std::move(value); }
    TileKind getKind() const { return kind_; }
    void setKind(TileKind value) { kind_ = value; }
    Color getAccent() const { return accent_; }
    void setAccent(Color value) { accent_ = value; }
    int getPrice() const { return price_; }
    void setPrice(int value) { price_ = value; }
    int getMortgageValue() const { return mortgageValue_; }
    void setMortgageValue(int value) { mortgageValue_ = value; }
    int getBaseRent() const { return baseRent_; }
    void setBaseRent(int value) { baseRent_ = value; }
    int getHouseCost() const { return houseCost_; }
    void setHouseCost(int value) { houseCost_ = value; }
    int getHotelCost() const { return hotelCost_; }
    void setHotelCost(int value) { hotelCost_ = value; }
    int getOwnerIndex() const { return ownerIndex_; }
    void setOwnerIndex(int value) { ownerIndex_ = value; }
    int getBuildings() const { return buildings_; }
    void setBuildings(int value) { buildings_ = value; }
    int getFestivalTurns() const { return festivalTurns_; }
    void setFestivalTurns(int value) { festivalTurns_ = value; }
    bool isMortgaged() const { return mortgaged_; }
    void setMortgaged(bool value) { mortgaged_ = value; }

private:
    int index_ = 0;
    std::string code_;
    std::string name_;
    std::string flavor_;
    std::string group_;
    TileKind kind_ = TileKind::Go;
    Color accent_ = WHITE;
    int price_ = 0;
    int mortgageValue_ = 0;
    int baseRent_ = 0;
    int houseCost_ = 0;
    int hotelCost_ = 0;
    int ownerIndex_ = -1;
    int buildings_ = 0;
    int festivalTurns_ = 0;
    bool mortgaged_ = false;
};

class TileDeedInfo {
public:
    TileDeedInfo() = default;
    TileDeedInfo(
        bool valid,
        std::string title,
        int purchasePrice = 0,
        int mortgageValue = 0,
        std::vector<std::string> moneyRowLabels = {},
        std::vector<int> moneyRowValues = {},
        std::vector<std::string> detailRowLabels = {},
        std::vector<std::string> detailRowValues = {},
        std::string ownershipStatus = {},
        std::string ownerName = {}
    )
        : valid_(valid),
          title_(std::move(title)),
          purchasePrice_(purchasePrice),
          mortgageValue_(mortgageValue),
          moneyRowLabels_(std::move(moneyRowLabels)),
          moneyRowValues_(std::move(moneyRowValues)),
          detailRowLabels_(std::move(detailRowLabels)),
          detailRowValues_(std::move(detailRowValues)),
          ownershipStatus_(std::move(ownershipStatus)),
          ownerName_(std::move(ownerName)) {}

    bool isValid() const { return valid_; }
    void setValid(bool value) { valid_ = value; }
    const std::string& getTitle() const { return title_; }
    void setTitle(std::string value) { title_ = std::move(value); }
    int getPurchasePrice() const { return purchasePrice_; }
    void setPurchasePrice(int value) { purchasePrice_ = value; }
    int getMortgageValue() const { return mortgageValue_; }
    void setMortgageValue(int value) { mortgageValue_ = value; }
    const std::vector<std::string>& getMoneyRowLabels() const { return moneyRowLabels_; }
    void setMoneyRowLabels(std::vector<std::string> value) { moneyRowLabels_ = std::move(value); }
    const std::vector<int>& getMoneyRowValues() const { return moneyRowValues_; }
    void setMoneyRowValues(std::vector<int> value) { moneyRowValues_ = std::move(value); }
    const std::vector<std::string>& getDetailRowLabels() const { return detailRowLabels_; }
    void setDetailRowLabels(std::vector<std::string> value) { detailRowLabels_ = std::move(value); }
    const std::vector<std::string>& getDetailRowValues() const { return detailRowValues_; }
    void setDetailRowValues(std::vector<std::string> value) { detailRowValues_ = std::move(value); }
    const std::string& getOwnershipStatus() const { return ownershipStatus_; }
    void setOwnershipStatus(std::string value) { ownershipStatus_ = std::move(value); }
    const std::string& getOwnerName() const { return ownerName_; }
    void setOwnerName(std::string value) { ownerName_ = std::move(value); }

private:
    bool valid_ = false;
    std::string title_;
    int purchasePrice_ = 0;
    int mortgageValue_ = 0;
    std::vector<std::string> moneyRowLabels_;
    std::vector<int> moneyRowValues_;
    std::vector<std::string> detailRowLabels_;
    std::vector<std::string> detailRowValues_;
    std::string ownershipStatus_;
    std::string ownerName_;
};

class PlayerInfo {
public:
    PlayerInfo() = default;
    PlayerInfo(
        std::string name,
        Color accent = WHITE,
        int money = 0,
        int position = 0,
        bool jailed = false,
        bool bankrupt = false,
        bool shieldActive = false,
        int failedJailRolls = 0,
        int discountPercent = 0,
        std::vector<int> properties = {},
        std::vector<CardInfo> handCards = {},
        bool computerPlayer = false
    )
        : name_(std::move(name)),
          accent_(accent),
          money_(money),
          position_(position),
          jailed_(jailed),
          bankrupt_(bankrupt),
          shieldActive_(shieldActive),
          failedJailRolls_(failedJailRolls),
          discountPercent_(discountPercent),
          properties_(std::move(properties)),
          handCards_(std::move(handCards)),
          computerPlayer_(computerPlayer) {}

    const std::string& getName() const { return name_; }
    void setName(std::string value) { name_ = std::move(value); }
    Color getAccent() const { return accent_; }
    void setAccent(Color value) { accent_ = value; }
    int getMoney() const { return money_; }
    void setMoney(int value) { money_ = value; }
    int getPosition() const { return position_; }
    void setPosition(int value) { position_ = value; }
    bool isJailed() const { return jailed_; }
    void setJailed(bool value) { jailed_ = value; }
    bool isBankrupt() const { return bankrupt_; }
    void setBankrupt(bool value) { bankrupt_ = value; }
    bool isShieldActive() const { return shieldActive_; }
    void setShieldActive(bool value) { shieldActive_ = value; }
    int getFailedJailRolls() const { return failedJailRolls_; }
    void setFailedJailRolls(int value) { failedJailRolls_ = value; }
    int getDiscountPercent() const { return discountPercent_; }
    void setDiscountPercent(int value) { discountPercent_ = value; }
    std::vector<int>& getProperties() { return properties_; }
    const std::vector<int>& getProperties() const { return properties_; }
    void setProperties(std::vector<int> value) { properties_ = std::move(value); }
    std::vector<CardInfo>& getHandCards() { return handCards_; }
    const std::vector<CardInfo>& getHandCards() const { return handCards_; }
    void setHandCards(std::vector<CardInfo> value) { handCards_ = std::move(value); }
    bool isComputerPlayer() const { return computerPlayer_; }
    void setComputerPlayer(bool value) { computerPlayer_ = value; }

private:
    std::string name_;
    Color accent_ = WHITE;
    int money_ = 0;
    int position_ = 0;
    bool jailed_ = false;
    bool bankrupt_ = false;
    bool shieldActive_ = false;
    int failedJailRolls_ = 0;
    int discountPercent_ = 0;
    std::vector<int> properties_;
    std::vector<CardInfo> handCards_;
    bool computerPlayer_ = false;
};

class SaveSlot {
public:
    SaveSlot() = default;
    SaveSlot(
        std::string name,
        std::string subtitle,
        int turn = 1,
        int playerCount = kMinPlayers,
        Color accent = WHITE
    )
        : name_(std::move(name)),
          subtitle_(std::move(subtitle)),
          turn_(turn),
          playerCount_(playerCount),
          accent_(accent) {}

    const std::string& getName() const { return name_; }
    void setName(std::string value) { name_ = std::move(value); }
    const std::string& getSubtitle() const { return subtitle_; }
    void setSubtitle(std::string value) { subtitle_ = std::move(value); }
    int getTurn() const { return turn_; }
    void setTurn(int value) { turn_ = value; }
    int getPlayerCount() const { return playerCount_; }
    void setPlayerCount(int value) { playerCount_ = value; }
    Color getAccent() const { return accent_; }
    void setAccent(Color value) { accent_ = value; }

private:
    std::string name_;
    std::string subtitle_;
    int turn_ = 1;
    int playerCount_ = kMinPlayers;
    Color accent_ = WHITE;
};

class LogItem {
public:
    LogItem() = default;
    LogItem(int turn, std::string actor, std::string action, std::string detail)
        : turn_(turn),
          actor_(std::move(actor)),
          action_(std::move(action)),
          detail_(std::move(detail)) {}

    int getTurn() const { return turn_; }
    void setTurn(int value) { turn_ = value; }
    const std::string& getActor() const { return actor_; }
    void setActor(std::string value) { actor_ = std::move(value); }
    const std::string& getAction() const { return action_; }
    void setAction(std::string value) { action_ = std::move(value); }
    const std::string& getDetail() const { return detail_; }
    void setDetail(std::string value) { detail_ = std::move(value); }

private:
    int turn_ = 1;
    std::string actor_;
    std::string action_;
    std::string detail_;
};

class Toast {
public:
    Toast() = default;
    Toast(std::string text, Color accent = WHITE, float timeLeft = 0.0f, float duration = 0.0f)
        : text_(std::move(text)),
          accent_(accent),
          timeLeft_(timeLeft),
          duration_(duration) {}

    const std::string& getText() const { return text_; }
    void setText(std::string value) { text_ = std::move(value); }
    Color getAccent() const { return accent_; }
    void setAccent(Color value) { accent_ = value; }
    float getTimeLeft() const { return timeLeft_; }
    void setTimeLeft(float value) { timeLeft_ = value; }
    float getDuration() const { return duration_; }
    void setDuration(float value) { duration_ = value; }

private:
    std::string text_;
    Color accent_ = WHITE;
    float timeLeft_ = 0.0f;
    float duration_ = 0.0f;
};

class AuctionState {
public:
    AuctionState() = default;
    AuctionState(
        int tileIndex,
        int selectedBidder,
        int highestBid,
        int highestBidder,
        std::vector<bool> passed,
        std::string bidInput = {},
        std::string bidError = {}
    )
        : tileIndex_(tileIndex),
          selectedBidder_(selectedBidder),
          highestBid_(highestBid),
          highestBidder_(highestBidder),
          passed_(std::move(passed)),
          bidInput_(std::move(bidInput)),
          bidError_(std::move(bidError)) {}

    int getTileIndex() const { return tileIndex_; }
    void setTileIndex(int value) { tileIndex_ = value; }
    int getSelectedBidder() const { return selectedBidder_; }
    void setSelectedBidder(int value) { selectedBidder_ = value; }
    int getHighestBid() const { return highestBid_; }
    void setHighestBid(int value) { highestBid_ = value; }
    int getHighestBidder() const { return highestBidder_; }
    void setHighestBidder(int value) { highestBidder_ = value; }
    std::vector<bool>& getPassed() { return passed_; }
    const std::vector<bool>& getPassed() const { return passed_; }
    void setPassed(std::vector<bool> value) { passed_ = std::move(value); }
    const std::string& getBidInput() const { return bidInput_; }
    std::string& getBidInput() { return bidInput_; }
    void setBidInput(std::string value) { bidInput_ = std::move(value); }
    const std::string& getBidError() const { return bidError_; }
    void setBidError(std::string value) { bidError_ = std::move(value); }

private:
    int tileIndex_ = -1;
    int selectedBidder_ = 0;
    int highestBid_ = 0;
    int highestBidder_ = -1;
    std::vector<bool> passed_;
    std::string bidInput_;
    std::string bidError_;
};

class OverlayState {
public:
    OverlayState() = default;
    OverlayState(
        OverlayType type,
        int tileIndex = -1,
        int selectedIndex = 0,
        int selectedPlayer = -1,
        int deckKey = kChanceDeckKey,
        int manualDieOne = 1,
        int manualDieTwo = 1,
        float anim = 0.0f,
        std::string message = {},
        CardInfo card = {},
        AuctionState auction = {}
    )
        : type_(type),
          tileIndex_(tileIndex),
          selectedIndex_(selectedIndex),
          selectedPlayer_(selectedPlayer),
          deckKey_(deckKey),
          manualDieOne_(manualDieOne),
          manualDieTwo_(manualDieTwo),
          anim_(anim),
          message_(std::move(message)),
          card_(std::move(card)),
          auction_(std::move(auction)) {}

    OverlayType getType() const { return type_; }
    void setType(OverlayType value) { type_ = value; }
    int getTileIndex() const { return tileIndex_; }
    void setTileIndex(int value) { tileIndex_ = value; }
    int getSelectedIndex() const { return selectedIndex_; }
    void setSelectedIndex(int value) { selectedIndex_ = value; }
    int getSelectedPlayer() const { return selectedPlayer_; }
    void setSelectedPlayer(int value) { selectedPlayer_ = value; }
    int getDeckKey() const { return deckKey_; }
    void setDeckKey(int value) { deckKey_ = value; }
    int getManualDieOne() const { return manualDieOne_; }
    void setManualDieOne(int value) { manualDieOne_ = value; }
    int getManualDieTwo() const { return manualDieTwo_; }
    void setManualDieTwo(int value) { manualDieTwo_ = value; }
    float getAnim() const { return anim_; }
    void setAnim(float value) { anim_ = value; }
    const std::string& getMessage() const { return message_; }
    void setMessage(std::string value) { message_ = std::move(value); }
    CardInfo& getCard() { return card_; }
    const CardInfo& getCard() const { return card_; }
    void setCard(CardInfo value) { card_ = std::move(value); }
    AuctionState& getAuction() { return auction_; }
    const AuctionState& getAuction() const { return auction_; }
    void setAuction(AuctionState value) { auction_ = std::move(value); }

private:
    OverlayType type_ = OverlayType::None;
    int tileIndex_ = -1;
    int selectedIndex_ = 0;
    int selectedPlayer_ = -1;
    int deckKey_ = kChanceDeckKey;
    int manualDieOne_ = 1;
    int manualDieTwo_ = 1;
    float anim_ = 0.0f;
    std::string message_;
    CardInfo card_;
    AuctionState auction_;
};

class GameState {
public:
    GameState() = default;
    GameState(
        std::string sessionLabel,
        std::vector<TileInfo> board = {},
        std::vector<PlayerInfo> players = {},
        std::vector<LogItem> logs = {},
        std::deque<Toast> toasts = {},
        int turn = 1,
        int turnLimit = 24,
        int currentPlayer = 0,
        int selectedTile = 0,
        int hoveredTile = -1,
        int lastDieOne = 0,
        int lastDieTwo = 0,
        int startCash = 1800,
        bool turnStarted = false,
        bool rolledThisTurn = false
    )
        : sessionLabel_(std::move(sessionLabel)),
          board_(std::move(board)),
          players_(std::move(players)),
          logs_(std::move(logs)),
          toasts_(std::move(toasts)),
          turn_(turn),
          turnLimit_(turnLimit),
          currentPlayer_(currentPlayer),
          selectedTile_(selectedTile),
          hoveredTile_(hoveredTile),
          lastDieOne_(lastDieOne),
          lastDieTwo_(lastDieTwo),
          startCash_(startCash),
          turnStarted_(turnStarted),
          rolledThisTurn_(rolledThisTurn) {}

    const std::string& getSessionLabel() const { return sessionLabel_; }
    void setSessionLabel(std::string value) { sessionLabel_ = std::move(value); }
    std::vector<TileInfo>& getBoard() { return board_; }
    const std::vector<TileInfo>& getBoard() const { return board_; }
    void setBoard(std::vector<TileInfo> value) { board_ = std::move(value); }
    std::vector<PlayerInfo>& getPlayers() { return players_; }
    const std::vector<PlayerInfo>& getPlayers() const { return players_; }
    void setPlayers(std::vector<PlayerInfo> value) { players_ = std::move(value); }
    std::vector<LogItem>& getLogs() { return logs_; }
    const std::vector<LogItem>& getLogs() const { return logs_; }
    void setLogs(std::vector<LogItem> value) { logs_ = std::move(value); }
    std::deque<Toast>& getToasts() { return toasts_; }
    const std::deque<Toast>& getToasts() const { return toasts_; }
    void setToasts(std::deque<Toast> value) { toasts_ = std::move(value); }
    int getTurn() const { return turn_; }
    void setTurn(int value) { turn_ = value; }
    int getTurnLimit() const { return turnLimit_; }
    void setTurnLimit(int value) { turnLimit_ = value; }
    int getCurrentPlayer() const { return currentPlayer_; }
    void setCurrentPlayer(int value) { currentPlayer_ = value; }
    int getSelectedTile() const { return selectedTile_; }
    void setSelectedTile(int value) { selectedTile_ = value; }
    int getHoveredTile() const { return hoveredTile_; }
    void setHoveredTile(int value) { hoveredTile_ = value; }
    int getLastDieOne() const { return lastDieOne_; }
    void setLastDieOne(int value) { lastDieOne_ = value; }
    int getLastDieTwo() const { return lastDieTwo_; }
    void setLastDieTwo(int value) { lastDieTwo_ = value; }
    int getStartCash() const { return startCash_; }
    void setStartCash(int value) { startCash_ = value; }
    bool isTurnStarted() const { return turnStarted_; }
    void setTurnStarted(bool value) { turnStarted_ = value; }
    bool isRolledThisTurn() const { return rolledThisTurn_; }
    void setRolledThisTurn(bool value) { rolledThisTurn_ = value; }

private:
    std::string sessionLabel_ = "NIMONSPOLI";
    std::vector<TileInfo> board_;
    std::vector<PlayerInfo> players_;
    std::vector<LogItem> logs_;
    std::deque<Toast> toasts_;
    int turn_ = 1;
    int turnLimit_ = 24;
    int currentPlayer_ = 0;
    int selectedTile_ = 0;
    int hoveredTile_ = -1;
    int lastDieOne_ = 0;
    int lastDieTwo_ = 0;
    int startCash_ = 1800;
    bool turnStarted_ = false;
    bool rolledThisTurn_ = false;
};

class AppState {
public:
    AppState() = default;
    AppState(
        Screen screen,
        OverlayState overlay = {},
        GameState game = {},
        std::array<std::string, kMaxPlayers> playerNames = {"Aster", "Bima", "Cora", "Danu"},
        int playerCount = 4,
        int computerPlayerCount = 1,
        int startingCash = 1800,
        int turnLimit = 24,
        NewGameConfigMode newGameConfigMode = NewGameConfigMode::DefaultConfig,
        std::string activeField = {},
        std::string saveInput = "save-1",
        std::string loadInput = "save-1",
        std::vector<SaveSlot> saveSlots = {},
        int selectedSave = 0,
        std::mt19937 rng = std::mt19937(0),
        float time = 0.0f,
        bool quitRequested = false
    )
        : screen_(screen),
          overlay_(std::move(overlay)),
          game_(std::move(game)),
          playerNames_(std::move(playerNames)),
          playerCount_(playerCount),
          computerPlayerCount_(computerPlayerCount),
          startingCash_(startingCash),
          turnLimit_(turnLimit),
          newGameConfigMode_(newGameConfigMode),
          activeField_(std::move(activeField)),
          saveInput_(std::move(saveInput)),
          loadInput_(std::move(loadInput)),
          saveSlots_(std::move(saveSlots)),
          selectedSave_(selectedSave),
          rng_(std::move(rng)),
          time_(time),
          quitRequested_(quitRequested) {}

    Screen getScreen() const { return screen_; }
    void setScreen(Screen value) { screen_ = value; }
    OverlayState& getOverlay() { return overlay_; }
    const OverlayState& getOverlay() const { return overlay_; }
    void setOverlay(OverlayState value) { overlay_ = std::move(value); }
    GameState& getGame() { return game_; }
    const GameState& getGame() const { return game_; }
    void setGame(GameState value) { game_ = std::move(value); }
    std::array<std::string, kMaxPlayers>& getPlayerNames() { return playerNames_; }
    const std::array<std::string, kMaxPlayers>& getPlayerNames() const { return playerNames_; }
    void setPlayerNames(std::array<std::string, kMaxPlayers> value) { playerNames_ = std::move(value); }
    int getPlayerCount() const { return playerCount_; }
    void setPlayerCount(int value) { playerCount_ = value; }
    int getComputerPlayerCount() const { return computerPlayerCount_; }
    void setComputerPlayerCount(int value) { computerPlayerCount_ = value; }
    int getStartingCash() const { return startingCash_; }
    void setStartingCash(int value) { startingCash_ = value; }
    int getTurnLimit() const { return turnLimit_; }
    void setTurnLimit(int value) { turnLimit_ = value; }
    NewGameConfigMode getNewGameConfigMode() const { return newGameConfigMode_; }
    void setNewGameConfigMode(NewGameConfigMode value) { newGameConfigMode_ = value; }
    const std::string& getActiveField() const { return activeField_; }
    void setActiveField(std::string value) { activeField_ = std::move(value); }
    std::string& getActiveField() { return activeField_; }
    const std::string& getSaveInput() const { return saveInput_; }
    std::string& getSaveInput() { return saveInput_; }
    void setSaveInput(std::string value) { saveInput_ = std::move(value); }
    const std::string& getLoadInput() const { return loadInput_; }
    std::string& getLoadInput() { return loadInput_; }
    void setLoadInput(std::string value) { loadInput_ = std::move(value); }
    std::vector<SaveSlot>& getSaveSlots() { return saveSlots_; }
    const std::vector<SaveSlot>& getSaveSlots() const { return saveSlots_; }
    void setSaveSlots(std::vector<SaveSlot> value) { saveSlots_ = std::move(value); }
    int getSelectedSave() const { return selectedSave_; }
    void setSelectedSave(int value) { selectedSave_ = value; }
    std::mt19937& getRng() { return rng_; }
    const std::mt19937& getRng() const { return rng_; }
    void setRng(std::mt19937 value) { rng_ = std::move(value); }
    float getTime() const { return time_; }
    void setTime(float value) { time_ = value; }
    bool isQuitRequested() const { return quitRequested_; }
    void setQuitRequested(bool value) { quitRequested_ = value; }

private:
    Screen screen_ = Screen::MainMenu;
    OverlayState overlay_;
    GameState game_;
    std::array<std::string, kMaxPlayers> playerNames_ = {"Aster", "Bima", "Cora", "Danu"};
    int playerCount_ = 4;
    int computerPlayerCount_ = 1;
    int startingCash_ = 1800;
    int turnLimit_ = 24;
    NewGameConfigMode newGameConfigMode_ = NewGameConfigMode::DefaultConfig;
    std::string activeField_;
    std::string saveInput_ = "save-1";
    std::string loadInput_ = "save-1";
    std::vector<SaveSlot> saveSlots_;
    int selectedSave_ = 0;
    std::mt19937 rng_ = std::mt19937(0);
    float time_ = 0.0f;
    bool quitRequested_ = false;
};
}
