#include "view/raylib/GameplayScreenRenderer.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace view::raylibgui {
void GameplayScreenRenderer::draw(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    toolkit.drawHeader(state.getGame());

    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    toolkit.drawButton(
        "Menu",
        {screenWidth - 480.0f, 40.0f, 92.0f, 34.0f},
        toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f),
        toolkit.theme().getPaperSoft(),
        false,
        16.0f
    );

    const float marginX = 28.0f;
    const float top = 118.0f;
    const float gap = 18.0f;
    const float availableHeight = screenHeight - top - 28.0f;
    const float desiredSidebarWidth = std::max(360.0f, screenWidth * 0.28f);
    const float boardSize = std::min(availableHeight, screenWidth - desiredSidebarWidth - marginX * 2.0f - gap);
    const Rectangle boardRect = {marginX, top, boardSize, boardSize};
    const Rectangle sidebarRect = {boardRect.x + boardRect.width + gap, top, screenWidth - (boardRect.x + boardRect.width + gap) - marginX, boardSize};

    const bool interactive = !session.isOverlayOpen();

    drawBoardPanel(session, toolkit, boardRect, interactive);

    const float summaryH = 318.0f;
    const float actionH = 272.0f;
    const float logsH = std::max(180.0f, sidebarRect.height - summaryH - actionH - gap * 2.0f);
    const Rectangle summaryRect = {sidebarRect.x, sidebarRect.y, sidebarRect.width, summaryH};
    const Rectangle actionRect = {sidebarRect.x, summaryRect.y + summaryRect.height + gap, sidebarRect.width, actionH};
    const Rectangle logsRect = {sidebarRect.x, actionRect.y + actionRect.height + gap, sidebarRect.width, logsH};

    drawCurrentPlayerPanel(session, toolkit, summaryRect);
    drawActionPanel(session, toolkit, actionRect, interactive);
    drawLogsPreview(session, toolkit, logsRect);
}

void GameplayScreenRenderer::drawCurrentPlayerPanel(
    const GUIGameController& session,
    const UiToolkit& toolkit,
    Rectangle rect
) const {
    const GameState& game = session.state().getGame();
    toolkit.drawPanel(rect, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
    const Font font = toolkit.font();
    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());

    DrawTextEx(font, player.getName().c_str(), {rect.x + 18.0f, rect.y + 16.0f}, 30.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, toolkit.formatMoney(player.getMoney()).c_str(), {rect.x + 18.0f, rect.y + 52.0f}, 26.0f, 1.0f, player.getMoney() >= 0 ? toolkit.theme().getTeal() : toolkit.theme().getDanger());
    DrawTextEx(font, ("Posisi: " + game.getBoard().at(player.getPosition()).getName()).c_str(), {rect.x + 18.0f, rect.y + 86.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    toolkit.drawBadge(player.isJailed() ? "Jailed" : "Active", {rect.x + rect.width - 118.0f, rect.y + 18.0f, 92.0f, 28.0f}, player.isJailed() ? toolkit.mix(toolkit.theme().getDanger(), WHITE, 0.32f) : toolkit.mix(toolkit.theme().getSuccess(), WHITE, 0.42f), toolkit.theme().getInk());
    if (player.isShieldActive()) {
        toolkit.drawBadge("Shield", {rect.x + rect.width - 118.0f, rect.y + 54.0f, 92.0f, 28.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.24f), toolkit.theme().getPaperSoft());
    }
    if (player.getDiscountPercent() > 0) {
        toolkit.drawBadge("Disc " + std::to_string(player.getDiscountPercent()) + "%", {rect.x + rect.width - 118.0f, rect.y + 90.0f, 92.0f, 28.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.24f), toolkit.theme().getInk());
    }

    DrawTextEx(font, "Roster", {rect.x + 18.0f, rect.y + 136.0f}, 19.0f, 1.0f, toolkit.theme().getInk());
    const float rosterY = rect.y + 170.0f;
    for (int index = 0; index < static_cast<int>(game.getPlayers().size()); index++) {
        const PlayerInfo& listedPlayer = game.getPlayers().at(index);
        const Rectangle row = {rect.x + 14.0f, rosterY + index * 34.0f, rect.width - 28.0f, 28.0f};
        DrawRectangleRec(row, index == game.getCurrentPlayer() ? toolkit.mix(toolkit.theme().getPaperSoft(), listedPlayer.getAccent(), 0.20f) : toolkit.mix(toolkit.theme().getPaperSoft(), listedPlayer.getAccent(), 0.08f));
        DrawCircle(static_cast<int>(row.x + 14.0f), static_cast<int>(row.y + 14.0f), 6.0f, listedPlayer.getAccent());
        DrawTextEx(font, listedPlayer.getName().c_str(), {row.x + 28.0f, row.y + 5.0f}, 16.0f, 1.0f, toolkit.theme().getInk());
        DrawTextEx(font, toolkit.formatMoney(listedPlayer.getMoney()).c_str(), {row.x + row.width - 110.0f, row.y + 5.0f}, 16.0f, 1.0f, listedPlayer.getMoney() >= 0 ? toolkit.theme().getInk() : toolkit.theme().getDanger());
    }
}

void GameplayScreenRenderer::drawBoardPanel(
    GUIGameController& session,
    const UiToolkit& toolkit,
    Rectangle rect,
    bool interactive
) const {
    AppState& state = session.state();
    toolkit.drawPanel(rect, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
    const Font font = toolkit.font();

    const float squareSize = std::min(rect.width - 34.0f, rect.height - 34.0f);
    const Rectangle square = {rect.x + 18.0f, rect.y + 18.0f, squareSize, squareSize};
    DrawRectangleRec(square, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.06f));

    state.getGame().setHoveredTile(-1);

    for (const TileInfo& tile : state.getGame().getBoard()) {
        const Rectangle tileRect = session.boardTileRect(square, tile.getIndex());
        const bool hovered = interactive && toolkit.isHovered(tileRect);
        const bool selected = state.getGame().getSelectedTile() == tile.getIndex();
        if (hovered) {
            state.getGame().setHoveredTile(tile.getIndex());
        }

        Color fill = toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.12f);
        if (selected) {
            fill = toolkit.mix(tile.getAccent(), toolkit.theme().getPaperSoft(), 0.28f);
        } else if (hovered) {
            fill = toolkit.mix(tile.getAccent(), WHITE, 0.40f);
        }

        DrawRectangleRec(toolkit.inset(tileRect, 2.0f), fill);
        DrawRectangleRec({tileRect.x + 2.0f, tileRect.y + 2.0f, tileRect.width - 4.0f, 8.0f}, tile.getAccent());
        DrawRectangleLinesEx(toolkit.inset(tileRect, 1.0f), 1.0f, toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.25f));

        DrawTextEx(font, tile.getCode().c_str(), {tileRect.x + 8.0f, tileRect.y + 16.0f}, 12.0f, 1.0f, toolkit.theme().getInkMuted());
        toolkit.drawWrappedText(tile.getName(), {tileRect.x + 8.0f, tileRect.y + 32.0f, tileRect.width - 16.0f, tileRect.height - 38.0f}, 15.0f, toolkit.theme().getInk(), 3.0f, 3);

        if (tile.getOwnerIndex() >= 0) {
            const Color ownerColor = state.getGame().getPlayers().at(tile.getOwnerIndex()).getAccent();
            DrawCircle(static_cast<int>(tileRect.x + tileRect.width - 12.0f), static_cast<int>(tileRect.y + 18.0f), 5.0f, ownerColor);
        }

        if (interactive && hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            session.setSelectedTile(tile.getIndex());
        }
    }

    const Rectangle center = {square.x + square.width / 7.0f, square.y + square.height / 7.0f, square.width * 5.0f / 7.0f, square.height * 5.0f / 7.0f};
    toolkit.drawPanel(center, toolkit.mix(toolkit.theme().getPaperSoft(), toolkit.theme().getGold(), 0.04f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
    const TileInfo& tile = state.getGame().getBoard().at(state.getGame().getSelectedTile());

    class CenterLine {
    public:
        CenterLine(std::string text, float fontSize, Color color, float gapAfter)
            : text_(std::move(text)),
              fontSize_(fontSize),
              color_(color),
              gapAfter_(gapAfter) {}

        const std::string& getText() const { return text_; }
        void setText(std::string text) { text_ = std::move(text); }

        float getFontSize() const { return fontSize_; }
        void setFontSize(float fontSize) { fontSize_ = fontSize; }

        Color getColor() const { return color_; }
        void setColor(Color color) { color_ = color; }

        float getGapAfter() const { return gapAfter_; }
        void setGapAfter(float gapAfter) { gapAfter_ = gapAfter; }

    private:
        std::string text_;
        float fontSize_;
        Color color_;
        float gapAfter_;
    };

    const float contentWidth = center.width - 96.0f;
    std::vector<CenterLine> detailLines;
    detailLines.emplace_back(tile.getName(), 30.0f, toolkit.theme().getInk(), 6.0f);

    const std::string kindLabel = toolkit.tileKindLabel(tile.getKind());
    if (kindLabel != tile.getName()) {
        detailLines.emplace_back(kindLabel, 18.0f, tile.getAccent(), 18.0f);
    } else {
        detailLines.back().setGapAfter(18.0f);
    }

    const std::vector<std::string> flavorLines = toolkit.wrapText(tile.getFlavor(), 18.0f, contentWidth);
    for (int index = 0; index < static_cast<int>(flavorLines.size()); index++) {
        detailLines.emplace_back(
            flavorLines.at(index),
            18.0f,
            toolkit.theme().getInkMuted(),
            index == static_cast<int>(flavorLines.size()) - 1 ? 24.0f : 6.0f
        );
    }

    const std::string ownerText = tile.getOwnerIndex() >= 0 ? "Owner: " + state.getGame().getPlayers().at(tile.getOwnerIndex()).getName() : "Owner: belum dimiliki";
    detailLines.emplace_back(ownerText, 18.0f, toolkit.theme().getInk(), 8.0f);
    if (toolkit.tileIsOwnable(tile)) {
        detailLines.emplace_back("Harga: " + toolkit.formatMoney(tile.getPrice()), 18.0f, toolkit.theme().getInkMuted(), 6.0f);
        detailLines.emplace_back("Sewa: " + toolkit.formatMoney(session.computeRent(tile)), 18.0f, toolkit.theme().getInkMuted(), 6.0f);
        if (toolkit.tileIsStreet(tile)) {
            detailLines.emplace_back("Bangunan: " + std::to_string(tile.getBuildings()), 18.0f, toolkit.theme().getInkMuted(), 0.0f);
        }
    } else {
        detailLines.emplace_back("Petak ini memiliki efek khusus saat pemain mendarat.", 18.0f, toolkit.theme().getInkMuted(), 0.0f);
    }

    float blockHeight = 0.0f;
    for (const CenterLine& line : detailLines) {
        blockHeight += line.getFontSize() + line.getGapAfter();
    }

    const Rectangle contentArea = {center.x + 48.0f, center.y + 42.0f, center.width - 96.0f, center.height - 128.0f};
    float detailY = contentArea.y + std::max(0.0f, (contentArea.height - blockHeight) * 0.5f);
    for (const CenterLine& line : detailLines) {
        const Vector2 size = MeasureTextEx(font, line.getText().c_str(), line.getFontSize(), 1.0f);
        DrawTextEx(
            font,
            line.getText().c_str(),
            {contentArea.x + (contentArea.width - size.x) * 0.5f, detailY},
            line.getFontSize(),
            1.0f,
            line.getColor()
        );
        detailY += line.getFontSize() + line.getGapAfter();
    }

    const std::string hint = "Klik petak untuk melihat detail atau memilih target kartu.";
    const Vector2 hintSize = MeasureTextEx(font, hint.c_str(), 16.0f, 1.0f);
    DrawTextEx(font, hint.c_str(), {center.x + (center.width - hintSize.x) * 0.5f, center.y + center.height - 42.0f}, 16.0f, 1.0f, toolkit.theme().getTeal());

    for (int index = 0; index < static_cast<int>(state.getGame().getPlayers().size()); index++) {
        const PlayerInfo& player = state.getGame().getPlayers().at(index);
        const Rectangle tileRect = session.boardTileRect(square, player.getPosition());
        const float chipOffsetX = 18.0f + (index % 2) * 18.0f;
        const float chipOffsetY = 18.0f + (index / 2) * 18.0f;
        const float pulse = 1.0f + (index == state.getGame().getCurrentPlayer() ? std::sin(state.getTime() * 4.0f) * 0.08f : 0.0f);
        const Vector2 chipCenter = {tileRect.x + chipOffsetX, tileRect.y + chipOffsetY};
        DrawCircleV(chipCenter, 8.0f * pulse, player.getAccent());
        DrawCircleLines(static_cast<int>(chipCenter.x), static_cast<int>(chipCenter.y), 8.0f * pulse, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.9f));
    }
}

void GameplayScreenRenderer::drawActionPanel(
    GUIGameController& session,
    const UiToolkit& toolkit,
    Rectangle rect,
    bool interactive
) const {
    const AppState& state = session.state();
    toolkit.drawPanel(rect, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
    const Font font = toolkit.font();
    DrawTextEx(font, "Aksi Utama", {rect.x + 18.0f, rect.y + 16.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(
        font,
        ("Dadu: " + std::to_string(state.getGame().getLastDieOne()) + " + " + std::to_string(state.getGame().getLastDieTwo())).c_str(),
        {rect.x + rect.width - 150.0f, rect.y + 23.0f},
        16.0f,
        1.0f,
        toolkit.theme().getInkMuted()
    );

    const float colWidth = (rect.width - 52.0f) / 2.0f;
    const float buttonWidth = colWidth;
    const float buttonHeight = 34.0f;
    const float buttonGap = 7.0f;
    const float startX = rect.x + 18.0f;
    const float startY = rect.y + 58.0f;

    const std::vector<std::string> labels = {
        "Start Turn",
        "Roll Dice",
        "Atur Dadu",
        "End Turn",
        "Detail Tile",
        "Portfolio",
        "Cards",
        "Save",
        "Logs",
        "Help",
    };

    for (int index = 0; index < static_cast<int>(labels.size()); index++) {
        const float x = startX + (index % 2) * (buttonWidth + 16.0f);
        const float y = startY + (index / 2) * (buttonHeight + buttonGap);
        const Rectangle button = {x, y, buttonWidth, buttonHeight};

        if (labels.at(index) == "Start Turn") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), interactive, 15.0f)) {
                session.startTurn();
            }
        } else if (labels.at(index) == "Roll Dice") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), interactive, 15.0f)) {
                session.rollDice();
            }
        } else if (labels.at(index) == "Atur Dadu") {
            const bool canSetDice = interactive && state.getGame().isTurnStarted() && !state.getGame().isRolledThisTurn();
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.18f), toolkit.theme().getInk(), canSetDice, 15.0f)) {
                session.openSetDice();
            }
        } else if (labels.at(index) == "End Turn") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.08f), toolkit.theme().getPaperSoft(), interactive, 15.0f)) {
                session.endTurn();
            }
        } else if (labels.at(index) == "Detail Tile") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.16f), toolkit.theme().getInk(), interactive, 15.0f)) {
                session.openTileDetail(state.getGame().getSelectedTile());
            }
        } else if (labels.at(index) == "Portfolio") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getSage(), WHITE, 0.18f), toolkit.theme().getInk(), interactive, 15.0f)) {
                session.openPortfolio();
            }
        } else if (labels.at(index) == "Cards") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getTeal(), WHITE, 0.22f), toolkit.theme().getInk(), interactive, 15.0f)) {
                session.openCards();
            }
        } else if (labels.at(index) == "Save") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getCoral(), WHITE, 0.22f), toolkit.theme().getInk(), interactive && session.canSaveNow(), 15.0f)) {
                session.openSave();
            }
        } else if (labels.at(index) == "Logs") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getNavy(), 0.18f), toolkit.theme().getInk(), interactive, 15.0f)) {
                session.openLogs();
            }
        } else if (labels.at(index) == "Help") {
            if (toolkit.drawButton(labels.at(index), button, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getSage(), 0.30f), toolkit.theme().getInk(), interactive, 15.0f)) {
                session.openHelp();
            }
        }
    }
}

void GameplayScreenRenderer::drawInteractionLab(
    GUIGameController& session,
    const UiToolkit& toolkit,
    Rectangle rect,
    bool interactive
) const {
    AppState& state = session.state();
    toolkit.drawPanel(rect, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
    const Font font = toolkit.font();
    DrawTextEx(font, "Aksi Lanjutan", {rect.x + 18.0f, rect.y + 16.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(
        font,
        "Pilih aksi tambahan yang tersedia untuk pemain aktif.",
        {rect.x + 18.0f, rect.y + 46.0f},
        16.0f,
        1.0f,
        toolkit.theme().getInkMuted()
    );

    const std::vector<std::string> buttons = {
        "Purchase",
        "Auction",
        "PPh",
        "PBM",
        "Festival",
        "Jail",
        "Card Draw",
        "Mortgage",
        "Redeem",
        "Build",
        "Liquidate",
        "Game Over",
    };

    const float gridX = rect.x + 18.0f;
    const float gridY = rect.y + 82.0f;
    const float buttonWidth = (rect.width - 52.0f) / 3.0f;
    const float buttonHeight = 42.0f;

    for (int index = 0; index < static_cast<int>(buttons.size()); index++) {
        const Rectangle button = {
            gridX + (index % 3) * (buttonWidth + 8.0f),
            gridY + (index / 3) * (buttonHeight + 10.0f),
            buttonWidth,
            buttonHeight,
        };

        if (!toolkit.drawButton(buttons.at(index), button, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.12f), toolkit.theme().getInk(), interactive, 17.0f)) {
            continue;
        }

        if (buttons.at(index) == "Purchase") {
            int tileIndex = state.getGame().getSelectedTile();
            if (!toolkit.tileIsOwnable(state.getGame().getBoard().at(tileIndex)) || state.getGame().getBoard().at(tileIndex).getOwnerIndex() >= 0) {
                tileIndex = 0;
                for (const TileInfo& tile : state.getGame().getBoard()) {
                    if (toolkit.tileIsOwnable(tile) && tile.getOwnerIndex() < 0) {
                        tileIndex = tile.getIndex();
                        break;
                    }
                }
            }
            session.openPurchase(tileIndex);
        } else if (buttons.at(index) == "Auction") {
            int tileIndex = state.getGame().getSelectedTile();
            if (!toolkit.tileIsOwnable(state.getGame().getBoard().at(tileIndex))) {
                tileIndex = 0;
                for (const TileInfo& tile : state.getGame().getBoard()) {
                    if (toolkit.tileIsOwnable(tile) && tile.getOwnerIndex() < 0) {
                        tileIndex = tile.getIndex();
                        break;
                    }
                }
            }
            session.openAuctionForTile(tileIndex);
        } else if (buttons.at(index) == "PPh") {
            session.openIncomeTax();
        } else if (buttons.at(index) == "PBM") {
            session.openLuxuryTax();
        } else if (buttons.at(index) == "Festival") {
            session.openFestival();
        } else if (buttons.at(index) == "Jail") {
            state.getGame().getPlayers().at(state.getGame().getCurrentPlayer()).setJailed(true);
            state.getGame().getPlayers().at(state.getGame().getCurrentPlayer()).setPosition(session.findJailIndex());
            session.openJail();
        } else if (buttons.at(index) == "Card Draw") {
            session.openRandomCardDraw(kChanceDeckKey);
        } else if (buttons.at(index) == "Mortgage") {
            session.openMortgage();
        } else if (buttons.at(index) == "Redeem") {
            session.openRedeem();
        } else if (buttons.at(index) == "Build") {
            session.openBuild();
        } else if (buttons.at(index) == "Liquidate") {
            state.getGame().getPlayers().at(state.getGame().getCurrentPlayer()).setMoney(-120);
            session.openLiquidation();
        } else if (buttons.at(index) == "Game Over") {
            session.openGameOver();
        }
    }
}

void GameplayScreenRenderer::drawLogsPreview(
    const GUIGameController& session,
    const UiToolkit& toolkit,
    Rectangle rect
) const {
    const GameState& game = session.state().getGame();
    toolkit.drawPanel(rect, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
    const Font font = toolkit.font();
    DrawTextEx(font, "Log Terbaru", {rect.x + 18.0f, rect.y + 16.0f}, 24.0f, 1.0f, toolkit.theme().getInk());

    const int count = std::min<int>(6, game.getLogs().size());
    float y = rect.y + 54.0f;
    for (int index = 0; index < count; index++) {
        const LogItem& log = game.getLogs().at(game.getLogs().size() - count + index);
        const Rectangle row = {rect.x + 14.0f, y, rect.width - 28.0f, 52.0f};
        DrawRectangleRec(row, toolkit.mix(toolkit.theme().getPaperSoft(), toolkit.theme().getTeal(), 0.06f));
        toolkit.drawBadge("T" + std::to_string(log.getTurn()), {row.x + 10.0f, row.y + 12.0f, 44.0f, 24.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.20f), toolkit.theme().getInk());
        DrawTextEx(font, (log.getActor() + " - " + log.getAction()).c_str(), {row.x + 64.0f, row.y + 8.0f}, 17.0f, 1.0f, toolkit.theme().getInk());
        toolkit.drawWrappedText(log.getDetail(), {row.x + 64.0f, row.y + 28.0f, row.width - 74.0f, 18.0f}, 15.0f, toolkit.theme().getInkMuted(), 3.0f, 1);
        y += 58.0f;
    }
}

void GameplayScreenRenderer::drawToasts(const GUIGameController& session, const UiToolkit& toolkit) const {
    float y = 110.0f;
    for (const Toast& toast : session.state().getGame().getToasts()) {
        const float progress = toolkit.clamp01(toast.getTimeLeft() / toast.getDuration());
        const float slide = toolkit.easeOutCubic(1.0f - progress);
        const Rectangle rect = {
            static_cast<float>(GetScreenWidth()) - 350.0f + (1.0f - slide) * 30.0f,
            y,
            320.0f,
            54.0f,
        };
        toolkit.drawPanel(rect, toolkit.mix(toolkit.theme().getPaperSoft(), toast.getAccent(), 0.18f), toolkit.withAlpha(toast.getAccent(), 0.24f), 0.0f);
        toolkit.drawWrappedText(toast.getText(), {rect.x + 16.0f, rect.y + 12.0f, rect.width - 32.0f, 30.0f}, 17.0f, toolkit.theme().getInk(), 3.0f, 2);
        y += 62.0f;
    }
}
}
