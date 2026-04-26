#pragma once

#include "view/raylib/GuiTypes.hpp"

#include <string>
#include <vector>

namespace view::raylibgui {
class UiToolkit {
public:
    UiToolkit();
    ~UiToolkit();

    const Theme& theme() const;
    bool loadFont(const std::string& path, int baseSize = 64);
    void unloadFont();
    Font font() const;

    float clamp01(float value) const;
    float easeOutCubic(float value) const;
    Color withAlpha(Color color, float alpha) const;
    Color mix(Color a, Color b, float amount) const;
    Color lighten(Color color, float amount) const;

    Rectangle inset(Rectangle rect, float value) const;
    Rectangle scaleFromCenter(Rectangle rect, float scale) const;
    bool isHovered(Rectangle rect) const;

    std::string formatMoney(int value) const;
    std::string tileKindLabel(TileKind kind) const;
    Color colorForGroup(const std::string& group) const;
    bool tileIsOwnable(const TileInfo& tile) const;
    bool tileIsStreet(const TileInfo& tile) const;

    std::vector<std::string> wrapText(
        const std::string& text,
        float fontSize,
        float maxWidth
    ) const;

    void drawWrappedText(
        const std::string& text,
        Rectangle rect,
        float fontSize,
        Color color,
        float lineGap = 5.0f,
        int maxLines = 0
    ) const;

    void drawPanel(
        Rectangle rect,
        Color fill,
        Color border,
        float roundness = 0.18f
    ) const;

    bool drawButton(
        const std::string& label,
        Rectangle rect,
        Color fill,
        Color textColor,
        bool enabled = true,
        float fontSize = 20.0f
    ) const;

    bool drawChip(
        const std::string& label,
        Rectangle rect,
        bool selected,
        Color accent,
        bool enabled = true
    ) const;

    bool drawTextField(
        AppState& state,
        const std::string& id,
        std::string& value,
        const std::string& placeholder,
        Rectangle rect,
        int maxLength
    ) const;

    void drawBadge(
        const std::string& label,
        Rectangle rect,
        Color fill,
        Color textColor
    ) const;

    void drawBackground(float time) const;
    void drawHeader(const GameState& game) const;
    Rectangle drawModalShell(float animation, float widthRatio, float heightRatio) const;
    void drawCloseHint(Rectangle modal) const;

private:
    static constexpr float GuiFontScale = 1.3f;

    Theme theme_;
    Font customFont_{};
    bool customFontLoaded_ = false;
};
}
