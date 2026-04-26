#pragma once

#include "controller/GUIController/GUIGameController.hpp"
#include "view/raylib/UiToolkit.hpp"

namespace view::raylibgui {
class OverlayRenderer {
public:
    void draw(GUIGameController& controller, const UiToolkit& toolkit) const;

private:
    static int effectiveMoneyFor(const PlayerInfo& player, int amount);

    void drawTileDetail(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawPurchase(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawAuction(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawIncomeTax(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawLuxuryTax(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawFestival(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawJail(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawCards(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawPortfolio(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawBuild(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawMortgage(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawRedeem(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawSave(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawLogs(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawSetDice(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawHelp(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawForceDrop(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawLiquidation(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawCardDraw(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawGameOver(GUIGameController& controller, const UiToolkit& toolkit) const;
};
}
