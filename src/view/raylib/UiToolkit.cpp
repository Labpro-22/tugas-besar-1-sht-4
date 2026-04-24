#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace view::raylibgui {
namespace {
constexpr float kGuiFontScale = 1.3f;
}

UiToolkit::UiToolkit()
    : theme_(
          {247, 239, 223, 255},
          {255, 250, 243, 255},
          {31, 45, 44, 255},
          {90, 98, 96, 255},
          {31, 91, 99, 255},
          {214, 108, 80, 255},
          {220, 176, 78, 255},
          {133, 170, 141, 255},
          {45, 73, 101, 255},
          {160, 67, 55, 255},
          {78, 132, 92, 255},
          {17, 25, 34, 42}) {}

UiToolkit::~UiToolkit() {
    if (IsWindowReady()) {
        unloadFont();
    }
}

const Theme& UiToolkit::theme() const {
    return theme_;
}

bool UiToolkit::loadFont(const std::string& path, int baseSize) {
    unloadFont();

    if (!FileExists(path.c_str())) {
        return false;
    }

    Font loadedFont = LoadFontEx(path.c_str(), baseSize, nullptr, 0);
    if (loadedFont.texture.id == 0) {
        return false;
    }

    SetTextureFilter(loadedFont.texture, TEXTURE_FILTER_BILINEAR);
    customFont_ = loadedFont;
    customFontLoaded_ = true;
    return true;
}

void UiToolkit::unloadFont() {
    if (!customFontLoaded_) {
        return;
    }

    UnloadFont(customFont_);
    customFont_ = {};
    customFontLoaded_ = false;
}

Font UiToolkit::font() const {
    Font guiFont = customFontLoaded_ ? customFont_ : GetFontDefault();
    guiFont.baseSize = std::max(1, static_cast<int>(std::round(guiFont.baseSize / kGuiFontScale)));
    return guiFont;
}

float UiToolkit::clamp01(float value) const {
    return std::max(0.0f, std::min(1.0f, value));
}

float UiToolkit::easeOutCubic(float value) const {
    const float t = clamp01(value);
    const float oneMinus = 1.0f - t;
    return 1.0f - (oneMinus * oneMinus * oneMinus);
}

Color UiToolkit::withAlpha(Color color, float alpha) const {
    color.a = static_cast<unsigned char>(255.0f * clamp01(alpha));
    return color;
}

Color UiToolkit::mix(Color a, Color b, float amount) const {
    const float t = clamp01(amount);
    return {
        static_cast<unsigned char>(a.r + (b.r - a.r) * t),
        static_cast<unsigned char>(a.g + (b.g - a.g) * t),
        static_cast<unsigned char>(a.b + (b.b - a.b) * t),
        static_cast<unsigned char>(a.a + (b.a - a.a) * t),
    };
}

Color UiToolkit::lighten(Color color, float amount) const {
    return mix(color, WHITE, amount);
}

Rectangle UiToolkit::inset(Rectangle rect, float value) const {
    return {
        rect.x + value,
        rect.y + value,
        rect.width - 2.0f * value,
        rect.height - 2.0f * value,
    };
}

Rectangle UiToolkit::scaleFromCenter(Rectangle rect, float scale) const {
    const float newWidth = rect.width * scale;
    const float newHeight = rect.height * scale;
    return {
        rect.x + (rect.width - newWidth) * 0.5f,
        rect.y + (rect.height - newHeight) * 0.5f,
        newWidth,
        newHeight,
    };
}

bool UiToolkit::isHovered(Rectangle rect) const {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

std::string UiToolkit::formatMoney(int value) const {
    std::ostringstream stream;
    stream << "Rp" << value;
    return stream.str();
}

std::string UiToolkit::tileKindLabel(TileKind kind) const {
    switch (kind) {
        case TileKind::Go: return "GO";
        case TileKind::Street: return "Street";
        case TileKind::Railroad: return "Railroad";
        case TileKind::Utility: return "Utility";
        case TileKind::Chance: return "Chance";
        case TileKind::CommunityChest: return "Dana Umum";
        case TileKind::IncomeTax: return "PPh";
        case TileKind::LuxuryTax: return "PBM";
        case TileKind::Festival: return "Festival";
        case TileKind::Jail: return "Jail";
        case TileKind::GoToJail: return "Go To Jail";
        case TileKind::FreeParking: return "Free Parking";
    }

    return "Tile";
}

Color UiToolkit::colorForGroup(const std::string& group) const {
    if (group == "Amber") return {210, 149, 63, 255};
    if (group == "Cyan") return {72, 156, 174, 255};
    if (group == "Rose") return {196, 96, 123, 255};
    if (group == "Orange") return {226, 127, 59, 255};
    if (group == "Scarlet") return {180, 72, 64, 255};
    if (group == "Midnight") return {58, 73, 111, 255};
    return theme_.getSage();
}

bool UiToolkit::tileIsOwnable(const TileInfo& tile) const {
    return tile.getKind() == TileKind::Street ||
           tile.getKind() == TileKind::Railroad ||
           tile.getKind() == TileKind::Utility;
}

bool UiToolkit::tileIsStreet(const TileInfo& tile) const {
    return tile.getKind() == TileKind::Street;
}

std::vector<std::string> UiToolkit::wrapText(
    const std::string& text,
    float fontSize,
    float maxWidth
) const {
    std::istringstream stream(text);
    std::string word;
    std::string currentLine;
    std::vector<std::string> lines;
    const Font guiFont = font();

    while (stream >> word) {
        const std::string candidate = currentLine.empty() ? word : currentLine + " " + word;
        const Vector2 size = MeasureTextEx(guiFont, candidate.c_str(), fontSize, 1.0f);
        if (!currentLine.empty() && size.x > maxWidth) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine = candidate;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    if (lines.empty()) {
        lines.push_back("");
    }

    return lines;
}

void UiToolkit::drawWrappedText(
    const std::string& text,
    Rectangle rect,
    float fontSize,
    Color color,
    float lineGap,
    int maxLines
) const {
    const Font guiFont = font();
    const std::vector<std::string> lines = wrapText(text, fontSize, rect.width);
    const int lineCount = maxLines > 0 ? std::min<int>(maxLines, lines.size()) : static_cast<int>(lines.size());

    for (int index = 0; index < lineCount; index++) {
        const float y = rect.y + index * (fontSize + lineGap);
        if (y + fontSize > rect.y + rect.height) {
            break;
        }

        DrawTextEx(guiFont, lines.at(index).c_str(), {rect.x, y}, fontSize, 1.0f, color);
    }
}

void UiToolkit::drawPanel(Rectangle rect, Color fill, Color border, float roundness) const {
    (void) roundness;
    const Rectangle shadow = {rect.x, rect.y + rect.height, rect.width, 5.0f};
    DrawRectangleRec(shadow, withAlpha(theme_.getShadow(), 0.42f));
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 1.5f, border);
}

bool UiToolkit::drawButton(
    const std::string& label,
    Rectangle rect,
    Color fill,
    Color textColor,
    bool enabled,
    float fontSize
) const {
    const bool hovered = enabled && isHovered(rect);
    const bool pressed = hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    const Color buttonFill = enabled
        ? (hovered ? lighten(fill, 0.08f) : fill)
        : mix(fill, theme_.getPaperSoft(), 0.55f);
    const Color border = enabled ? mix(fill, theme_.getInk(), 0.22f) : mix(fill, theme_.getPaperSoft(), 0.30f);
    const Rectangle visualRect = hovered ? Rectangle{rect.x, rect.y - 2.0f, rect.width, rect.height} : rect;

    drawPanel(visualRect, buttonFill, border, 0.0f);

    const Font guiFont = font();
    const Vector2 size = MeasureTextEx(guiFont, label.c_str(), fontSize, 1.0f);
    DrawTextEx(
        guiFont,
        label.c_str(),
        {visualRect.x + (visualRect.width - size.x) * 0.5f, visualRect.y + (visualRect.height - size.y) * 0.5f},
        fontSize,
        1.0f,
        enabled ? textColor : mix(textColor, theme_.getPaper(), 0.45f)
    );

    return pressed;
}

bool UiToolkit::drawChip(
    const std::string& label,
    Rectangle rect,
    bool selected,
    Color accent,
    bool enabled
) const {
    const Color fill = selected ? accent : mix(theme_.getPaperSoft(), accent, 0.14f);
    const Color textColor = selected ? theme_.getPaperSoft() : theme_.getInk();
    return drawButton(label, rect, fill, textColor, enabled, 17.0f);
}

bool UiToolkit::drawTextField(
    AppState& state,
    const std::string& id,
    std::string& value,
    const std::string& placeholder,
    Rectangle rect,
    int maxLength
) const {
    const bool focused = state.getActiveField() == id;
    const bool hovered = isHovered(rect);
    const bool clicked = hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    if (clicked) {
        state.setActiveField(id);
    }

    if (focused) {
        int character = GetCharPressed();
        while (character > 0) {
            if (character >= 32 && character <= 125 && static_cast<int>(value.size()) < maxLength) {
                value.push_back(static_cast<char>(character));
            }
            character = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !value.empty()) {
            value.pop_back();
        }
    }

    const Color fill = focused ? mix(theme_.getPaperSoft(), theme_.getTeal(), 0.08f) : theme_.getPaperSoft();
    const Color border = focused ? theme_.getTeal() : mix(theme_.getPaperSoft(), theme_.getInkMuted(), 0.30f);
    drawPanel(rect, fill, border, 0.0f);

    const std::string visibleText = value.empty() ? placeholder : value;
    const Color textColor = value.empty() ? mix(theme_.getInkMuted(), theme_.getPaper(), 0.15f) : theme_.getInk();
    const Font guiFont = font();
    DrawTextEx(guiFont, visibleText.c_str(), {rect.x + 14.0f, rect.y + 14.0f}, 22.0f, 1.0f, textColor);

    if (focused) {
        const float blink = std::fmod(state.getTime() * 1.8f, 1.0f);
        if (blink > 0.45f) {
            const Vector2 size = MeasureTextEx(guiFont, visibleText.c_str(), 22.0f, 1.0f);
            const float caretX = rect.x + 16.0f + size.x + 2.0f;
            DrawLineEx({caretX, rect.y + 12.0f}, {caretX, rect.y + rect.height - 12.0f}, 2.0f, theme_.getTeal());
        }
    }

    return clicked;
}

void UiToolkit::drawBadge(
    const std::string& label,
    Rectangle rect,
    Color fill,
    Color textColor
) const {
    DrawRectangleRec(rect, fill);
    const Font guiFont = font();
    const Vector2 size = MeasureTextEx(guiFont, label.c_str(), 16.0f, 1.0f);
    DrawTextEx(
        guiFont,
        label.c_str(),
        {rect.x + (rect.width - size.x) * 0.5f, rect.y + (rect.height - size.y) * 0.5f},
        16.0f,
        1.0f,
        textColor
    );
}

void UiToolkit::drawBackground(float time) const {
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), theme_.getPaperSoft(), theme_.getPaper());

    for (int index = 0; index < 7; index++) {
        const float angle = time * (0.22f + 0.03f * index) + index;
        const float radius = 120.0f + index * 70.0f;
        const float x = GetScreenWidth() * 0.52f + std::cos(angle) * radius;
        const float y = GetScreenHeight() * 0.20f + std::sin(angle * 1.2f) * radius * 0.5f;
        const float circleRadius = 28.0f + 8.0f * index;
        const Color tint = index % 2 == 0 ? withAlpha(theme_.getGold(), 0.10f) : withAlpha(theme_.getTeal(), 0.07f);
        DrawCircleV({x, y}, circleRadius, tint);
    }

    for (int lineIndex = 0; lineIndex < GetScreenWidth(); lineIndex += 64) {
        DrawLineEx(
            {static_cast<float>(lineIndex), 0.0f},
            {static_cast<float>(lineIndex - 140), static_cast<float>(GetScreenHeight())},
            1.0f,
            withAlpha(theme_.getInkMuted(), 0.05f)
        );
    }
}

void UiToolkit::drawHeader(const GameState& game) const {
    const Rectangle header = {26.0f, 22.0f, static_cast<float>(GetScreenWidth()) - 52.0f, 76.0f};
    drawPanel(header, withAlpha(theme_.getPaperSoft(), 0.98f), withAlpha(theme_.getInkMuted(), 0.15f), 0.0f);

    const Font guiFont = font();
    DrawTextEx(guiFont, "NIMONSPOLI", {header.x + 22.0f, header.y + 16.0f}, 30.0f, 1.0f, theme_.getInk());

    const Rectangle badgeTurn = {header.x + header.width - 340.0f, header.y + 18.0f, 86.0f, 32.0f};
    const Rectangle badgePhase = {header.x + header.width - 244.0f, header.y + 18.0f, 102.0f, 32.0f};
    const Rectangle badgeMode = {header.x + header.width - 132.0f, header.y + 18.0f, 110.0f, 32.0f};
    const std::string activePlayer = game.getPlayers().empty()
        ? "Menu"
        : game.getPlayers().at(std::min<int>(game.getCurrentPlayer(), static_cast<int>(game.getPlayers().size()) - 1)).getName();
    drawBadge("Turn " + std::to_string(game.getTurn()), badgeTurn, mix(theme_.getGold(), WHITE, 0.24f), theme_.getInk());
    drawBadge(game.isTurnStarted() ? "In Turn" : "Standby", badgePhase, mix(theme_.getTeal(), WHITE, 0.34f), theme_.getInk());
    drawBadge(activePlayer, badgeMode, mix(theme_.getCoral(), WHITE, 0.30f), theme_.getInk());
}

Rectangle UiToolkit::drawModalShell(float animation, float widthRatio, float heightRatio) const {
    const float animated = easeOutCubic(animation);
    const Rectangle base = {
        GetScreenWidth() * (1.0f - widthRatio) * 0.5f,
        GetScreenHeight() * (1.0f - heightRatio) * 0.5f,
        GetScreenWidth() * widthRatio,
        GetScreenHeight() * heightRatio,
    };
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), withAlpha(theme_.getInk(), 0.42f * animated));
    const Rectangle modal = scaleFromCenter(base, 0.92f + 0.08f * animated);
    drawPanel(modal, withAlpha(theme_.getPaperSoft(), 0.995f), withAlpha(theme_.getInkMuted(), 0.15f), 0.0f);
    return modal;
}

void UiToolkit::drawCloseHint(Rectangle modal) const {
    DrawTextEx(
        font(),
        "Esc atau tombol tutup untuk kembali.",
        {modal.x + modal.width - 210.0f, modal.y + 18.0f},
        16.0f,
        1.0f,
        theme_.getInkMuted()
    );
}
}
