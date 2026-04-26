#include "view/raylib/MenuScreenRenderer.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace view::raylibgui {
namespace {
std::string professorName(int index) {
    return index == 0 ? "ProfessorRayapSunggal" : "ProfessorRayapSunggal" + std::to_string(index + 1);
}
}

void MenuScreenRenderer::draw(GUIGameController& session, const UiToolkit& toolkit) const {
    switch (session.state().getScreen()) {
        case Screen::MainMenu:
            drawMainMenu(session, toolkit);
            break;
        case Screen::NewGame:
            drawNewGame(session, toolkit);
            break;
        case Screen::LoadGame:
            drawLoadGame(session, toolkit);
            break;
        case Screen::Tutorial:
            drawTutorial(session, toolkit);
            break;
        case Screen::Gameplay:
            break;
    }
}

void MenuScreenRenderer::drawMainMenu(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    const Rectangle hero = {40.0f, 120.0f , screenWidth * 0.56f, screenHeight - 180.0f};
    const float menuCardHeight = hero.height - 700.0f;
    const Rectangle menuCard = {
        hero.x + hero.width + 20.0f,
        (screenHeight - menuCardHeight) * 0.5f,
        screenWidth - hero.width - 120.0f,
        menuCardHeight
    };

    toolkit.drawPanel(hero, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.96f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.15f), 0.0f);
    toolkit.drawPanel(menuCard, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.15f), 0.0f);

    const Font font = toolkit.font();
    DrawTextEx(font, "NIMONSPOLI", {hero.x + 30.0f, hero.y + 34.0f}, 38.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Monopoli Tapi Versi Minion.", {hero.x + 30.0f, hero.y + 78.0f}, 38.0f, 1.0f, toolkit.theme().getTeal());
    toolkit.drawWrappedText(
        "Beli properti, bangun aset, kelola kartu, hadapi pajak, ikuti lelang, dan pertahankan saldo sampai akhir permainan.",
        {hero.x + 32.0f, hero.y + 148.0f, hero.width - 64.0f, 120.0f},
        22.0f,
        toolkit.theme().getInkMuted(),
        7.0f
    );

    const Rectangle boardMini = {hero.x + 34.0f, hero.y + 300.0f, hero.width - 68.0f, hero.height - 340.0f};
    toolkit.drawPanel(boardMini, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getGold(), 0.08f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);

    const float squareSize = std::min(boardMini.width - 36.0f, boardMini.height - 36.0f);
    const Rectangle square = {
        boardMini.x + (boardMini.width - squareSize) * 0.5f,
        boardMini.y + (boardMini.height - squareSize) * 0.5f,
        squareSize,
        squareSize
    };
    DrawRectangleRec(square, toolkit.mix(toolkit.theme().getPaperSoft(), toolkit.theme().getTeal(), 0.06f));
    for (int index = 0; index < 4 * (kGridCells - 1); index++) {
        const Rectangle tile = session.boardTileRect(square, index);
        const Color tint = index % 4 == 0 ? toolkit.withAlpha(toolkit.theme().getCoral(), 0.25f) : toolkit.withAlpha(toolkit.theme().getTeal(), 0.18f);
        DrawRectangleRec(toolkit.inset(tile, 3.0f), tint);
    }

    const Vector2 center = {square.x + square.width * 0.5f, square.y + square.height * 0.5f};
    for (int token = 0; token < 4; token++) {
        const float angle = state.getTime() * (0.6f + token * 0.15f) + token;
        const Vector2 position = {
            center.x + std::cos(angle) * 70.0f,
            center.y + std::sin(angle * 1.3f) * 45.0f,
        };
        const Color tokenColor = token == 0 ? toolkit.theme().getCoral() : (token == 1 ? toolkit.theme().getTeal() : (token == 2 ? toolkit.theme().getGold() : toolkit.theme().getNavy()));
        DrawCircleV(position, 17.0f, tokenColor);
        DrawCircleLines(static_cast<int>(position.x), static_cast<int>(position.y), 17.0f, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.8f));
    }

    DrawTextEx(font, "Menu Utama", {menuCard.x + 26.0f, menuCard.y + 28.0f}, 28.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(
        font,
        "Mulai permainan baru, muat permainan, atau buka panduan.",
        {menuCard.x + 26.0f, menuCard.y + 62.0f},
        18.0f,
        1.0f,
        toolkit.theme().getInkMuted()
    );

    const float buttonX = menuCard.x + 24.0f;
    const float buttonW = menuCard.width - 48.0f;
    const float buttonHeight = 62.0f;
    const float buttonGap = 20.0f;
    const float buttonGroupHeight = 4.0f * buttonHeight + 3.0f * buttonGap;
    const float menuContentTop = menuCard.y + 112.0f;
    const float menuContentBottom = menuCard.y + menuCard.height - 138.0f;
    const float menuContentHeight = std::max(buttonGroupHeight, menuContentBottom - menuContentTop);
    const float buttonStartY = menuContentTop + (menuContentHeight - buttonGroupHeight) * 0.5f;

    if (toolkit.drawButton("Game Baru", {buttonX, buttonStartY, buttonW, buttonHeight}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 25.0f)) {
        state.setScreen(Screen::NewGame);
        state.getActiveField().clear();
    }
    if (toolkit.drawButton("Load Game", {buttonX, buttonStartY + buttonHeight + buttonGap, buttonW, buttonHeight}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 25.0f)) {
        state.setScreen(Screen::LoadGame);
        state.getActiveField().clear();
    }
    if (toolkit.drawButton("Panduan", {buttonX, buttonStartY + 2.0f * (buttonHeight + buttonGap), buttonW, buttonHeight}, toolkit.theme().getGold(), toolkit.theme().getInk(), true, 25.0f)) {
        state.setScreen(Screen::Tutorial);
        state.getActiveField().clear();
    }
    if (toolkit.drawButton("Keluar", {buttonX, buttonStartY + 3.0f * (buttonHeight + buttonGap), buttonW, buttonHeight}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 25.0f)) {
        state.setQuitRequested(true);
    }

    toolkit.drawWrappedText(
        "Shortcut: S mulai turn, R lempar dadu, D atur dadu, E akhiri turn, P portfolio, C kartu, L log, H help, Esc tutup modal.",
        {menuCard.x + 26.0f, menuCard.y + menuCard.height - 116.0f, menuCard.width - 52.0f, 90.0f},
        18.0f,
        toolkit.theme().getInkMuted(),
        6.0f
    );
}

void MenuScreenRenderer::drawStepper(
    const UiToolkit& toolkit,
    const std::string& label,
    int& value,
    int minValue,
    int maxValue,
    int step,
    Rectangle rect
) const {
    toolkit.drawPanel(rect, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.96f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
    const Font font = toolkit.font();
    DrawTextEx(font, label.c_str(), {rect.x + 14.0f, rect.y + 14.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
    DrawTextEx(font, std::to_string(value).c_str(), {rect.x + 18.0f, rect.y + 40.0f}, 30.0f, 1.0f, toolkit.theme().getInk());

    const Rectangle minusRect = {rect.x + rect.width - 108.0f, rect.y + 18.0f, 42.0f, 42.0f};
    const Rectangle plusRect = {rect.x + rect.width - 56.0f, rect.y + 18.0f, 42.0f, 42.0f};
    if (toolkit.drawButton("-", minusRect, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getCoral(), 0.16f), toolkit.theme().getInk(), value > minValue, 26.0f)) {
        value = std::max(minValue, value - step);
    }
    if (toolkit.drawButton("+", plusRect, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.16f), toolkit.theme().getInk(), value < maxValue, 26.0f)) {
        value = std::min(maxValue, value + step);
    }
}

void MenuScreenRenderer::drawNewGame(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    toolkit.drawHeader(state.getGame());
    const Rectangle card = {56.0f, 122.0f, static_cast<float>(GetScreenWidth()) - 112.0f, static_cast<float>(GetScreenHeight()) - 164.0f};
    toolkit.drawPanel(card, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.15f), 0.0f);

    const Font font = toolkit.font();
    DrawTextEx(font, "Game Baru", {card.x + 28.0f, card.y + 26.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(
        font,
        "Atur pemain manusia, pemain COM, dan sumber config.",
        {card.x + 28.0f, card.y + 66.0f},
        19.0f,
        1.0f,
        toolkit.theme().getInkMuted()
    );

    DrawTextEx(font, "Jumlah pemain", {card.x + 30.0f, card.y + 124.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
    for (int count = kMinPlayers; count <= kMaxPlayers; count++) {
        const float x = card.x + 30.0f + (count - kMinPlayers) * 136.0f;
        if (toolkit.drawChip(std::to_string(count) + " pemain", {x, card.y + 158.0f, 116.0f, 46.0f}, state.getPlayerCount() == count, toolkit.theme().getTeal())) {
            state.setPlayerCount(count);
            state.setComputerPlayerCount(std::min(state.getComputerPlayerCount(), state.getPlayerCount()));
        }
    }

    const int computerCount = std::max(0, std::min(state.getComputerPlayerCount(), state.getPlayerCount()));
    const int humanCount = state.getPlayerCount() - computerCount;
    state.setComputerPlayerCount(computerCount);

    const float fieldsTop = card.y + 258.0f;
    const float fieldHeight = 60.0f;
    const float fieldStep = 116.0f;
    const float fieldGap = 22.0f;
    const float fieldAreaWidth = std::min(640.0f, card.width * 0.52f);
    const float fieldWidth = (fieldAreaWidth - fieldGap) * 0.5f;
    for (int index = 0; index < kMaxPlayers; index++) {
        const int column = index % 2;
        const int row = index / 2;
        const Rectangle field = {
            card.x + 30.0f + column * (fieldWidth + fieldGap),
            fieldsTop + row * fieldStep,
            fieldWidth,
            fieldHeight
        };
        const std::string label = "Pemain " + std::to_string(index + 1);
        DrawTextEx(font, label.c_str(), {field.x, field.y - 30.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

        if (index < humanCount) {
            toolkit.drawTextField(
                state,
                "player-" + std::to_string(index),
                state.getPlayerNames().at(index),
                "Masukkan nama",
                field,
                18
            );
        } else if (index < state.getPlayerCount()) {
            toolkit.drawPanel(field, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.07f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.12f), 0.0f);
            const std::string displayText = professorName(index - humanCount);
            DrawTextEx(font, displayText.c_str(), {field.x + 16.0f, field.y + 19.0f}, 18.0f, 1.0f, toolkit.theme().getInk());
        } else {
            toolkit.drawPanel(field, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getPaperSoft(), 0.35f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
        }

        if (index >= humanCount && index < state.getPlayerCount()) {
            toolkit.drawBadge("COM", {field.x + field.width - 72.0f, field.y + 15.0f, 52.0f, 30.0f}, toolkit.mix(toolkit.theme().getTeal(), WHITE, 0.18f), toolkit.theme().getInk());
        } else if (index >= state.getPlayerCount()) {
            DrawRectangleRec(field, toolkit.withAlpha(toolkit.theme().getPaper(), 0.50f));
            DrawTextEx(font, "slot nonaktif", {field.x + field.width - 130.0f, field.y + 19.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
        }
    }

    const float configX = card.x + card.width * 0.58f;
    const float configW = card.width * 0.34f;
    DrawTextEx(font, "Sumber config", {configX, card.y + 124.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
    const bool defaultConfig = state.getNewGameConfigMode() == NewGameConfigMode::DefaultConfig;
    if (toolkit.drawChip("Bawaan", {configX, card.y + 158.0f, 118.0f, 44.0f}, defaultConfig, toolkit.theme().getTeal())) {
        state.setNewGameConfigMode(NewGameConfigMode::DefaultConfig);
    }
    if (toolkit.drawChip("Atur sendiri", {configX + 134.0f, card.y + 158.0f, 154.0f, 44.0f}, !defaultConfig, toolkit.theme().getGold())) {
        state.setNewGameConfigMode(NewGameConfigMode::CustomConfig);
    }

    const bool customConfig = state.getNewGameConfigMode() == NewGameConfigMode::CustomConfig;
    if (customConfig) {
        int startingCash = state.getStartingCash();
        drawStepper(toolkit, "Saldo awal", startingCash, 1000, 3000, 100, {configX, card.y + 224.0f, configW, 82.0f});
        state.setStartingCash(startingCash);
        int turnLimit = state.getTurnLimit();
        drawStepper(toolkit, "Batas turn", turnLimit, 12, 60, 2, {configX, card.y + 328.0f, configW, 82.0f});
        state.setTurnLimit(turnLimit);
    } else {
        const Rectangle defaultSummary = {configX, card.y + 224.0f, configW, 154.0f};
        toolkit.drawPanel(defaultSummary, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.08f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
        DrawTextEx(font, "config/misc.txt", {defaultSummary.x + 18.0f, defaultSummary.y + 18.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
        DrawTextEx(font, ("Saldo awal: " + toolkit.formatMoney(session.defaultConfigStartingCash())).c_str(), {defaultSummary.x + 18.0f, defaultSummary.y + 58.0f}, 19.0f, 1.0f, toolkit.theme().getInkMuted());
        const std::string turnLimitText = session.defaultConfigTurnLimit() > 0 ? std::to_string(session.defaultConfigTurnLimit()) : "Tanpa batas";
        DrawTextEx(font, ("Batas turn: " + turnLimitText).c_str(), {defaultSummary.x + 18.0f, defaultSummary.y + 92.0f}, 19.0f, 1.0f, toolkit.theme().getInkMuted());
    }

    int computerPlayers = state.getComputerPlayerCount();
    drawStepper(toolkit, "Pemain COM", computerPlayers, 0, state.getPlayerCount(), 1, {configX, customConfig ? card.y + 432.0f : card.y + 404.0f, configW, 82.0f});
    state.setComputerPlayerCount(std::max(0, std::min(computerPlayers, state.getPlayerCount())));

    const Rectangle note = {configX, customConfig ? card.y + 536.0f : card.y + 506.0f, configW, customConfig ? 112.0f : 132.0f};
    toolkit.drawPanel(note, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getGold(), 0.08f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
    DrawTextEx(font, "Alur Giliran", {note.x + 18.0f, note.y + 18.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
    const std::vector<std::string> flow = {
        "1. Turn pemain aktif dimulai otomatis",
        "2. Profesor memakai keputusan COM backend",
        "3. Turn berganti setelah efek petak selesai",
    };

    float flowY = note.y + 58.0f;
    for (const std::string& line : flow) {
        toolkit.drawWrappedText(line, {note.x + 18.0f, flowY, note.width - 36.0f, 24.0f}, 16.0f, toolkit.theme().getInkMuted());
        flowY += 26.0f;
    }

    if (toolkit.drawButton("Kembali", {card.x + 30.0f, card.y + card.height - 86.0f, 150.0f, 52.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 22.0f)) {
        state.setScreen(Screen::MainMenu);
        state.getActiveField().clear();
    }
    if (toolkit.drawButton("Mulai Game", {card.x + card.width - 238.0f, card.y + card.height - 86.0f, 208.0f, 52.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 22.0f)) {
        session.startFreshSession();
    }
}

void MenuScreenRenderer::drawLoadGame(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    toolkit.drawHeader(state.getGame());
    const Rectangle card = {66.0f, 126.0f, static_cast<float>(GetScreenWidth()) - 132.0f, static_cast<float>(GetScreenHeight()) - 176.0f};
    toolkit.drawPanel(card, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.15f), 0.0f);

    const Font font = toolkit.font();
    DrawTextEx(font, "Load Game", {card.x + 28.0f, card.y + 26.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(
        font,
        "Pilih save yang tersedia untuk melanjutkan permainan.",
        {card.x + 28.0f, card.y + 66.0f},
        19.0f,
        1.0f,
        toolkit.theme().getInkMuted()
    );

    const float slotWidth = (card.width - 76.0f) / 3.0f;
    for (int index = 0; index < static_cast<int>(state.getSaveSlots().size()); index++) {
        const SaveSlot& slot = state.getSaveSlots().at(index);
        const Rectangle slotRect = {card.x + 28.0f + index * (slotWidth + 10.0f), card.y + 130.0f, slotWidth, 240.0f};
        toolkit.drawPanel(slotRect, toolkit.mix(toolkit.theme().getPaperSoft(), slot.getAccent(), 0.10f), toolkit.withAlpha(slot.getAccent(), 0.35f), 0.0f);
        toolkit.drawBadge("Turn " + std::to_string(slot.getTurn()), {slotRect.x + 18.0f, slotRect.y + 18.0f, 84.0f, 30.0f}, toolkit.mix(slot.getAccent(), WHITE, 0.26f), toolkit.theme().getInk());
        toolkit.drawBadge(std::to_string(slot.getPlayerCount()) + "P", {slotRect.x + 112.0f, slotRect.y + 18.0f, 52.0f, 30.0f}, toolkit.mix(toolkit.theme().getPaperSoft(), slot.getAccent(), 0.18f), toolkit.theme().getInk());
        DrawTextEx(font, slot.getName().c_str(), {slotRect.x + 18.0f, slotRect.y + 68.0f}, 26.0f, 1.0f, toolkit.theme().getInk());
        toolkit.drawWrappedText(slot.getSubtitle(), {slotRect.x + 18.0f, slotRect.y + 104.0f, slotRect.width - 36.0f, 70.0f}, 18.0f, toolkit.theme().getInkMuted());
        if (toolkit.drawButton(index == state.getSelectedSave() ? "Terpilih" : "Pilih Slot", {slotRect.x + 18.0f, slotRect.y + slotRect.height - 62.0f, slotRect.width - 36.0f, 44.0f}, index == state.getSelectedSave() ? slot.getAccent() : toolkit.mix(toolkit.theme().getPaper(), slot.getAccent(), 0.18f), index == state.getSelectedSave() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 20.0f)) {
            state.setSelectedSave(index);
        }
    }

    DrawTextEx(font, "Nama save", {card.x + 28.0f, card.y + 420.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
    toolkit.drawTextField(state, "load-name", state.getLoadInput(), "nama save", {card.x + 28.0f, card.y + 454.0f, 360.0f, 56.0f}, 24);
    toolkit.drawWrappedText(
        "Masukkan nama save jika ingin memuat file tertentu.",
        {card.x + 28.0f, card.y + 522.0f, 380.0f, 62.0f},
        17.0f,
        toolkit.theme().getInkMuted()
    );

    const Rectangle notes = {card.x + 450.0f, card.y + 418.0f, card.width - 478.0f, 170.0f};
    toolkit.drawPanel(notes, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.08f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
    DrawTextEx(font, "Ringkasan Save", {notes.x + 18.0f, notes.y + 18.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
    const std::vector<std::string> distinctions = {
        "Save 1: permainan tengah dengan aset tersebar.",
        "Save 2: pemain berada di jail dan aset tergadai.",
        "Save 3: duel akhir menjelang batas turn.",
    };
    float distinctionY = notes.y + 58.0f;
    for (const std::string& text : distinctions) {
        toolkit.drawWrappedText(text, {notes.x + 18.0f, distinctionY, notes.width - 36.0f, 34.0f}, 18.0f, toolkit.theme().getInkMuted());
        distinctionY += 34.0f;
    }

    if (toolkit.drawButton("Kembali", {card.x + 28.0f, card.y + card.height - 82.0f, 150.0f, 52.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 22.0f)) {
        state.setScreen(Screen::MainMenu);
        state.getActiveField().clear();
    }
    if (toolkit.drawButton("Load Game", {card.x + card.width - 222.0f, card.y + card.height - 82.0f, 194.0f, 52.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 22.0f)) {
        session.loadSessionFromSlot(state.getSelectedSave());
    }
}

void MenuScreenRenderer::drawTutorial(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    toolkit.drawHeader(state.getGame());
    const Rectangle card = {60.0f, 126.0f, static_cast<float>(GetScreenWidth()) - 120.0f, static_cast<float>(GetScreenHeight()) - 178.0f};
    toolkit.drawPanel(card, toolkit.withAlpha(toolkit.theme().getPaperSoft(), 0.98f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.15f), 0.0f);

    const Font font = toolkit.font();
    DrawTextEx(font, "Panduan", {card.x + 28.0f, card.y + 26.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(
        font,
        "Ringkasan alur dan fitur utama permainan.",
        {card.x + 28.0f, card.y + 66.0f},
        20.0f,
        1.0f,
        toolkit.theme().getInkMuted()
    );

    const std::vector<std::pair<std::string, std::string>> sections = {
        {"Awal game", "Menu utama, setup pemain, load game, dan panduan."},
        {"Flow turn", "Start turn, roll dice, perpindahan pion, landing event, end turn."},
        {"Ekonomi", "Beli properti, auction, PPh, PBM, mortgage, redeem, liquidation."},
        {"Aset & kartu", "Build rumah/hotel, portfolio, hand cards, dan efek kartu instant."},
        {"Event spesial", "Festival, jail, go to jail, free parking, chance, dana umum."},
        {"Akhir game", "Save, log history, ranking total kekayaan, dan layar game over."},
    };

    float sectionY = card.y + 136.0f;
    for (int index = 0; index < static_cast<int>(sections.size()); index++) {
        const Rectangle sectionRect = {
            card.x + 28.0f + (index % 2) * (card.width * 0.46f + 18.0f),
            sectionY + (index / 2) * 128.0f,
            card.width * 0.46f,
            104.0f,
        };
        const Color tint = index % 3 == 0 ? toolkit.theme().getTeal() : (index % 3 == 1 ? toolkit.theme().getCoral() : toolkit.theme().getGold());
        toolkit.drawPanel(sectionRect, toolkit.mix(toolkit.theme().getPaperSoft(), tint, 0.10f), toolkit.withAlpha(tint, 0.28f), 0.0f);
        DrawTextEx(font, sections.at(index).first.c_str(), {sectionRect.x + 18.0f, sectionRect.y + 16.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
        toolkit.drawWrappedText(sections.at(index).second, {sectionRect.x + 18.0f, sectionRect.y + 50.0f, sectionRect.width - 36.0f, 44.0f}, 18.0f, toolkit.theme().getInkMuted());
    }

    const Rectangle footer = {card.x + 28.0f, card.y + card.height - 152.0f, card.width - 56.0f, 88.0f};
    toolkit.drawPanel(footer, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getGold(), 0.08f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
    toolkit.drawWrappedText(
        "Menangkan permainan dengan menjaga saldo, memperkuat properti, dan mengumpulkan total kekayaan tertinggi.",
        {footer.x + 20.0f, footer.y + 20.0f, footer.width - 40.0f, 52.0f},
        19.0f,
        toolkit.theme().getInkMuted()
    );

    if (toolkit.drawButton("Kembali", {card.x + 28.0f, card.y + card.height - 58.0f, 150.0f, 44.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        state.setScreen(Screen::MainMenu);
    }
    if (toolkit.drawButton("Buka Setup", {card.x + card.width - 180.0f, card.y + card.height - 58.0f, 152.0f, 44.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        state.setScreen(Screen::NewGame);
    }
}
}
