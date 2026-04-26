#include "view/raylib/OverlayRenderer.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace view::raylibgui {
namespace {
int effectiveMoneyFor(const PlayerInfo& player, int amount) {
    const int discount = player.getDiscountPercent();
    if (discount <= 0) {
        return amount;
    }
    return std::max(0, amount * (100 - discount) / 100);
}
}

void OverlayRenderer::draw(GUIGameController& session, const UiToolkit& toolkit) const {
    if (!session.isOverlayOpen()) {
        return;
    }

    switch (session.state().getOverlay().getType()) {
        case OverlayType::TileDetail: drawTileDetail(session, toolkit); break;
        case OverlayType::Purchase: drawPurchase(session, toolkit); break;
        case OverlayType::Auction: drawAuction(session, toolkit); break;
        case OverlayType::IncomeTax: drawIncomeTax(session, toolkit); break;
        case OverlayType::LuxuryTax: drawLuxuryTax(session, toolkit); break;
        case OverlayType::Festival: drawFestival(session, toolkit); break;
        case OverlayType::Jail: drawJail(session, toolkit); break;
        case OverlayType::Cards: drawCards(session, toolkit); break;
        case OverlayType::Portfolio: drawPortfolio(session, toolkit); break;
        case OverlayType::Build: drawBuild(session, toolkit); break;
        case OverlayType::Mortgage: drawMortgage(session, toolkit); break;
        case OverlayType::Redeem: drawRedeem(session, toolkit); break;
        case OverlayType::Save: drawSave(session, toolkit); break;
        case OverlayType::Logs: drawLogs(session, toolkit); break;
        case OverlayType::SetDice: drawSetDice(session, toolkit); break;
        case OverlayType::Help: drawHelp(session, toolkit); break;
        case OverlayType::ForceDrop: drawForceDrop(session, toolkit); break;
        case OverlayType::Liquidation: drawLiquidation(session, toolkit); break;
        case OverlayType::CardDraw: drawCardDraw(session, toolkit); break;
        case OverlayType::GameOver: drawGameOver(session, toolkit); break;
        case OverlayType::None: break;
    }
}

void OverlayRenderer::drawTileDetail(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const OverlayState& overlay = state.getOverlay();
    const GameState& game = state.getGame();
    const Rectangle modal = toolkit.drawModalShell(overlay.getAnim(), 0.58f, 0.66f);
    toolkit.drawCloseHint(modal);
    const Font font = toolkit.font();
    const TileInfo& tile = game.getBoard().at(overlay.getTileIndex() >= 0 ? overlay.getTileIndex() : game.getSelectedTile());
    const PlayerInfo& currentPlayer = game.getPlayers().at(game.getCurrentPlayer());

    DrawTextEx(font, tile.getName().c_str(), {modal.x + 28.0f, modal.y + 26.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, toolkit.tileKindLabel(tile.getKind()).c_str(), {modal.x + 28.0f, modal.y + 64.0f}, 18.0f, 1.0f, tile.getAccent());
    toolkit.drawWrappedText(tile.getFlavor(), {modal.x + 28.0f, modal.y + 98.0f, modal.width - 56.0f, 60.0f}, 20.0f, toolkit.theme().getInkMuted());

    const Rectangle deed = {modal.x + 28.0f, modal.y + 178.0f, modal.width - 56.0f, 238.0f};
    toolkit.drawPanel(deed, toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.12f), toolkit.withAlpha(tile.getAccent(), 0.24f), 0.0f);

    DrawTextEx(font, ("Kode: " + tile.getCode()).c_str(), {deed.x + 18.0f, deed.y + 18.0f}, 20.0f, 1.0f, toolkit.theme().getInk());
    if (toolkit.tileIsOwnable(tile)) {
        DrawTextEx(font, ("Harga beli: " + toolkit.formatMoney(effectiveMoneyFor(currentPlayer, tile.getPrice()))).c_str(), {deed.x + 18.0f, deed.y + 58.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
        DrawTextEx(font, ("Nilai gadai: " + toolkit.formatMoney(session.getMortgageValue(tile))).c_str(), {deed.x + 18.0f, deed.y + 88.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
        DrawTextEx(font, ("Sewa: " + toolkit.formatMoney(effectiveMoneyFor(currentPlayer, session.computeRent(tile)))).c_str(), {deed.x + 18.0f, deed.y + 118.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
        if (toolkit.tileIsStreet(tile)) {
            DrawTextEx(font, ("Biaya rumah: " + toolkit.formatMoney(effectiveMoneyFor(currentPlayer, tile.getHouseCost()))).c_str(), {deed.x + 18.0f, deed.y + 148.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
            DrawTextEx(font, ("Biaya hotel: " + toolkit.formatMoney(effectiveMoneyFor(currentPlayer, tile.getHotelCost()))).c_str(), {deed.x + 18.0f, deed.y + 178.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
            DrawTextEx(font, ("Bangunan aktif: " + std::to_string(tile.getBuildings())).c_str(), {deed.x + 18.0f, deed.y + 208.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
        }
    } else {
        DrawTextEx(font, "Petak ini memiliki efek khusus saat pemain mendarat.", {deed.x + 18.0f, deed.y + 58.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
    }

    const std::string owner = tile.getOwnerIndex() >= 0 ? game.getPlayers().at(tile.getOwnerIndex()).getName() : "Belum dimiliki";
    DrawTextEx(font, ("Owner: " + owner).c_str(), {modal.x + 28.0f, modal.y + 442.0f}, 20.0f, 1.0f, toolkit.theme().getInk());

    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 144.0f, modal.y + modal.height - 62.0f, 116.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawPurchase(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const OverlayState& overlay = state.getOverlay();
    const GameState& game = state.getGame();
    const Rectangle modal = toolkit.drawModalShell(overlay.getAnim(), 0.56f, 0.60f);
    toolkit.drawCloseHint(modal);
    const Font font = toolkit.font();
    const TileInfo& tile = game.getBoard().at(overlay.getTileIndex());
    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());

    DrawTextEx(font, "Beli Properti", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, tile.getName().c_str(), {modal.x + 28.0f, modal.y + 70.0f}, 24.0f, 1.0f, tile.getAccent());

    const int discount = player.getDiscountPercent();
    const int finalPrice = session.currentPurchasePrice();
    toolkit.drawWrappedText("Konfirmasi pembelian properti oleh pemain aktif.", {modal.x + 28.0f, modal.y + 108.0f, modal.width - 56.0f, 56.0f}, 19.0f, toolkit.theme().getInkMuted());

    const Rectangle summary = {modal.x + 28.0f, modal.y + 184.0f, modal.width - 56.0f, 138.0f};
    toolkit.drawPanel(summary, toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.10f), toolkit.withAlpha(tile.getAccent(), 0.24f), 0.0f);
    DrawTextEx(font, ("Harga dasar: " + toolkit.formatMoney(tile.getPrice())).c_str(), {summary.x + 18.0f, summary.y + 20.0f}, 20.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, ("Saldo pemain: " + toolkit.formatMoney(player.getMoney())).c_str(), {summary.x + 18.0f, summary.y + 54.0f}, 20.0f, 1.0f, toolkit.theme().getInkMuted());
    DrawTextEx(font, ("Harga akhir: " + toolkit.formatMoney(finalPrice)).c_str(), {summary.x + 18.0f, summary.y + 88.0f}, 22.0f, 1.0f, finalPrice <= player.getMoney() ? toolkit.theme().getSuccess() : toolkit.theme().getDanger());
    if (discount > 0) {
        DrawTextEx(font, ("Diskon aktif: " + std::to_string(discount) + "%").c_str(), {summary.x + 260.0f, summary.y + 88.0f}, 20.0f, 1.0f, toolkit.theme().getGold());
    }

    if (toolkit.drawButton("Beli", {modal.x + 28.0f, modal.y + modal.height - 72.0f, 140.0f, 46.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), session.canCurrentPlayerAffordSelectedPurchase(), 22.0f)) {
        session.buySelectedProperty();
    }
    if (toolkit.drawButton("Auction", {modal.x + 186.0f, modal.y + modal.height - 72.0f, 150.0f, 46.0f}, toolkit.theme().getGold(), toolkit.theme().getInk(), true, 22.0f)) {
        session.openAuctionForTile(tile.getIndex());
    }
    if (toolkit.drawButton("Lewati", {modal.x + modal.width - 146.0f, modal.y + modal.height - 72.0f, 118.0f, 46.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 22.0f)) {
        session.skipSelectedPurchase();
    }
}

void OverlayRenderer::drawAuction(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    OverlayState& overlay = state.getOverlay();
    const GameState& game = state.getGame();
    const Rectangle modal = toolkit.drawModalShell(overlay.getAnim(), 0.62f, 0.70f);
    toolkit.drawCloseHint(modal);
    const Font font = toolkit.font();
    const TileInfo& tile = game.getBoard().at(overlay.getAuction().getTileIndex());
    AuctionState& auction = overlay.getAuction();

    DrawTextEx(font, "Lelang", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, tile.getName().c_str(), {modal.x + 28.0f, modal.y + 68.0f}, 24.0f, 1.0f, tile.getAccent());
    int highestBid = auction.getHighestBid();
    if (auction.getHighestBidder() >= 0) {
        highestBid = effectiveMoneyFor(game.getPlayers().at(auction.getHighestBidder()), highestBid);
    }
    DrawTextEx(font, ("Bid tertinggi: " + toolkit.formatMoney(highestBid)).c_str(), {modal.x + 28.0f, modal.y + 104.0f}, 22.0f, 1.0f, toolkit.theme().getInk());

    const std::string leader = auction.getHighestBidder() >= 0 ? game.getPlayers().at(auction.getHighestBidder()).getName() : "Belum ada";
    DrawTextEx(font, ("Leader: " + leader).c_str(), {modal.x + 300.0f, modal.y + 104.0f}, 22.0f, 1.0f, toolkit.theme().getInkMuted());

    DrawTextEx(font, "Bidder aktif mengikuti urutan turn backend", {modal.x + 28.0f, modal.y + 150.0f}, 20.0f, 1.0f, toolkit.theme().getInk());
    for (int index = 0; index < static_cast<int>(game.getPlayers().size()); index++) {
        const PlayerInfo& player = game.getPlayers().at(index);
        const Rectangle row = {modal.x + 28.0f, modal.y + 186.0f + index * 62.0f, modal.width - 56.0f, 52.0f};
        DrawRectangleRec(row, index == auction.getSelectedBidder() ? toolkit.mix(toolkit.theme().getPaperSoft(), player.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), player.getAccent(), 0.08f));
        DrawCircle(static_cast<int>(row.x + 16.0f), static_cast<int>(row.y + 26.0f), 7.0f, player.getAccent());
        DrawTextEx(font, player.getName().c_str(), {row.x + 32.0f, row.y + 8.0f}, 20.0f, 1.0f, toolkit.theme().getInk());
        DrawTextEx(font, toolkit.formatMoney(player.getMoney()).c_str(), {row.x + 32.0f, row.y + 28.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());

        toolkit.drawButton(index == auction.getSelectedBidder() ? "Aktif" : "Tunggu", {row.x + row.width - 86.0f, row.y + 8.0f, 68.0f, 34.0f}, index == auction.getSelectedBidder() ? player.getAccent() : toolkit.mix(toolkit.theme().getPaper(), player.getAccent(), 0.16f), index == auction.getSelectedBidder() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), false, 16.0f);
    }

    const float bidButtonY = modal.y + modal.height - 138.0f;
    const bool hasBidder = auction.getHighestBidder() >= 0;
    const int minimumBid = hasBidder ? auction.getHighestBid() + 1 : 0;
    DrawTextEx(font, ("Min. bid: " + toolkit.formatMoney(minimumBid)).c_str(), {modal.x + 28.0f, bidButtonY - 50.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
    DrawTextEx(font, "Nilai bid:", {modal.x + 28.0f, bidButtonY - 26.0f}, 18.0f, 1.0f, toolkit.theme().getInk());

    toolkit.drawTextField(state, "auction-bid", auction.getBidInput(), "ketik nilai bid", {modal.x + 120.0f, bidButtonY, 200.0f, 42.0f}, 12);
    if (!auction.getBidError().empty()) {
        DrawTextEx(font, auction.getBidError().c_str(), {modal.x + 28.0f, bidButtonY + 50.0f}, 17.0f, 1.0f, toolkit.theme().getDanger());
    }

    if (toolkit.drawButton("Bid", {modal.x + 332.0f, bidButtonY, 110.0f, 42.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.18f), toolkit.theme().getInk(), true, 20.0f)) {
        const std::string raw = auction.getBidInput();
        std::string trimmed;
        for (char ch : raw) {
            if (!std::isspace(static_cast<unsigned char>(ch))) {
                trimmed.push_back(ch);
            }
        }

        if (trimmed.empty()) {
            auction.setBidError("Masukkan nilai bid terlebih dahulu.");
        } else if (trimmed.find_first_not_of("0123456789") != std::string::npos) {
            auction.setBidError("Nilai bid harus berupa angka non-negatif.");
        } else {
            int targetBid = -1;
            try {
                targetBid = std::stoi(trimmed);
            } catch (const std::exception&) {
                auction.setBidError("Nilai bid terlalu besar atau tidak valid.");
            }

            if (targetBid >= 0) {
                if (targetBid < minimumBid) {
                    auction.setBidError(hasBidder
                        ? "Bid harus lebih besar dari " + toolkit.formatMoney(auction.getHighestBid()) + "."
                        : "Bid minimal " + toolkit.formatMoney(minimumBid) + ".");
                } else {
                    const int bidderIndex = auction.getSelectedBidder();
                    const bool affordable = bidderIndex < 0 || bidderIndex >= static_cast<int>(game.getPlayers().size())
                        ? true
                        : targetBid <= game.getPlayers().at(bidderIndex).getMoney();
                    if (!affordable) {
                        auction.setBidError("Saldo bidder aktif tidak cukup.");
                    } else {
                        auction.setBidError("");
                        auction.setBidInput("");
                        session.auctionPlaceBid(targetBid);
                    }
                }
            }
        }
    }

    if (toolkit.drawButton("Pass", {modal.x + 452.0f, bidButtonY, 90.0f, 42.0f}, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getCoral(), 0.18f), toolkit.theme().getInk(), true, 20.0f)) {
        auction.setBidError("");
        auction.setBidInput("");
        session.auctionPass();
    }
    if (toolkit.drawButton("Selesaikan", {modal.x + modal.width - 184.0f, bidButtonY, 156.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.finalizeAuction();
    }
}

void OverlayRenderer::drawIncomeTax(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const GameState& game = state.getGame();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.50f, 0.48f);
    const Font font = toolkit.font();
    const int flatTax = session.flatIncomeTax();
    const int percentageTax = session.percentageIncomeTax();
    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());

    DrawTextEx(font, "PPh", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Pilih pajak flat atau persentase dari total kekayaan.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    const Rectangle flatRect = {modal.x + 28.0f, modal.y + 112.0f, modal.width * 0.42f, 134.0f};
    const Rectangle percentRect = {modal.x + modal.width - modal.width * 0.42f - 28.0f, modal.y + 112.0f, modal.width * 0.42f, 134.0f};
    toolkit.drawPanel(flatRect, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getCoral(), 0.10f), toolkit.withAlpha(toolkit.theme().getCoral(), 0.24f), 0.0f);
    toolkit.drawPanel(percentRect, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.10f), toolkit.withAlpha(toolkit.theme().getTeal(), 0.24f), 0.0f);
    DrawTextEx(font, "Flat", {flatRect.x + 18.0f, flatRect.y + 16.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, toolkit.formatMoney(effectiveMoneyFor(player, flatTax)).c_str(), {flatRect.x + 18.0f, flatRect.y + 56.0f}, 30.0f, 1.0f, toolkit.theme().getCoral());
    DrawTextEx(font, "Tetap, cepat, aman untuk wealth besar.", {flatRect.x + 18.0f, flatRect.y + 96.0f}, 17.0f, 1.0f, toolkit.theme().getInkMuted());
    DrawTextEx(font, "Persentase", {percentRect.x + 18.0f, percentRect.y + 16.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, toolkit.formatMoney(effectiveMoneyFor(player, percentageTax)).c_str(), {percentRect.x + 18.0f, percentRect.y + 56.0f}, 30.0f, 1.0f, toolkit.theme().getTeal());
    DrawTextEx(font, "10% dari total kekayaan saat ini.", {percentRect.x + 18.0f, percentRect.y + 96.0f}, 17.0f, 1.0f, toolkit.theme().getInkMuted());

    if (toolkit.drawButton("Bayar Flat", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 220.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.payIncomeTax(true);
    }
    if (toolkit.drawButton("Bayar Persentase", {modal.x + modal.width - 268.0f, modal.y + modal.height - 64.0f, 240.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.payIncomeTax(false);
    }
}

void OverlayRenderer::drawLuxuryTax(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const GameState& game = state.getGame();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.44f, 0.40f);
    const Font font = toolkit.font();
    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int tax = session.luxuryTaxAmount();

    DrawTextEx(font, "PBM", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    toolkit.drawWrappedText("Petak ini menarik pajak barang mewah dari pemain aktif.", {modal.x + 28.0f, modal.y + 76.0f, modal.width - 56.0f, 54.0f}, 20.0f, toolkit.theme().getInkMuted());
    DrawTextEx(font, toolkit.formatMoney(effectiveMoneyFor(player, tax)).c_str(), {modal.x + 28.0f, modal.y + 146.0f}, 38.0f, 1.0f, toolkit.theme().getDanger());

    if (toolkit.drawButton("Bayar", {modal.x + 28.0f, modal.y + modal.height - 64.0f, modal.width - 56.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.payLuxuryTax();
    }
}

void OverlayRenderer::drawFestival(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.58f, 0.66f);
    const Font font = toolkit.font();
    const std::vector<int> options = session.currentPlayerOwnableOptions();

    DrawTextEx(font, "Festival", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Pilih properti milik pemain aktif untuk diberi boost festival.", {modal.x + 28.0f, modal.y + 70.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (options.empty()) {
        toolkit.drawWrappedText("Belum ada properti yang dimiliki. Festival tidak punya target saat ini.", {modal.x + 28.0f, modal.y + 140.0f, modal.width - 56.0f, 60.0f}, 20.0f, toolkit.theme().getCoral());
    } else {
        for (int index = 0; index < static_cast<int>(options.size()); index++) {
            const TileInfo& tile = state.getGame().getBoard().at(options.at(index));
            const Rectangle row = {modal.x + 28.0f, modal.y + 128.0f + index * 56.0f, modal.width - 56.0f, 48.0f};
            DrawRectangleRec(row, index == state.getOverlay().getSelectedIndex() ? toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.08f));
            DrawTextEx(font, tile.getName().c_str(), {row.x + 16.0f, row.y + 6.0f}, 19.0f, 1.0f, toolkit.theme().getInk());
            DrawTextEx(font, ("Festival aktif: " + std::to_string(tile.getFestivalTurns()) + " turn").c_str(), {row.x + 16.0f, row.y + 26.0f}, 15.0f, 1.0f, toolkit.theme().getInkMuted());
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {row.x + row.width - 84.0f, row.y + 7.0f, 68.0f, 32.0f}, index == state.getOverlay().getSelectedIndex() ? tile.getAccent() : toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.12f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }
    }

    if (toolkit.drawButton("Aktifkan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 132.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), !options.empty(), 20.0f)) {
        session.activateFestivalOnSelectedTile();
    }
    if (toolkit.drawButton("Lewati", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
        session.commands().finishTurnAfterDiceIfReady();
    }
}

void OverlayRenderer::drawJail(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const GameState& game = state.getGame();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.54f, 0.56f);
    const Font font = toolkit.font();
    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int fine = session.jailFineAmount();
    const int effectiveFine = effectiveMoneyFor(player, fine);

    DrawTextEx(font, "Jail", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, player.isJailed() ? "Pemain aktif sedang ditahan." : "Pemain hanya sedang berkunjung.", {modal.x + 28.0f, modal.y + 68.0f}, 19.0f, 1.0f, toolkit.theme().getInkMuted());
    DrawTextEx(font, ("Gagal roll: " + std::to_string(player.getFailedJailRolls())).c_str(), {modal.x + 28.0f, modal.y + 104.0f}, 20.0f, 1.0f, toolkit.theme().getInk());

    const Rectangle choices = {modal.x + 28.0f, modal.y + 146.0f, modal.width - 56.0f, 120.0f};
    toolkit.drawPanel(choices, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getNavy(), 0.08f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);
    DrawTextEx(font, ("Denda keluar: " + toolkit.formatMoney(effectiveFine)).c_str(), {choices.x + 18.0f, choices.y + 18.0f}, 21.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Pilih cara keluar dari jail pada giliran ini.", {choices.x + 18.0f, choices.y + 58.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (toolkit.drawButton("Bayar Denda", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 150.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), player.isJailed(), 20.0f)) {
        session.payJailFine();
    }
    if (toolkit.drawButton("Pakai Kartu", {modal.x + 194.0f, modal.y + modal.height - 64.0f, 156.0f, 42.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.14f), toolkit.theme().getInk(), player.isJailed() && !player.getHandCards().empty(), 20.0f)) {
        session.useJailCard();
    }
    if (player.isJailed()) {
        const bool canAttemptRoll = player.getFailedJailRolls() < 3;
        if (toolkit.drawButton("Roll Double", {modal.x + modal.width - 160.0f, modal.y + modal.height - 64.0f, 132.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), canAttemptRoll, 20.0f)) {
            session.attemptJailRoll();
        }
    } else if (toolkit.drawButton("Tutup", {modal.x + modal.width - 160.0f, modal.y + modal.height - 64.0f, 132.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawCards(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.62f, 0.70f);
    const Font font = toolkit.font();
    const PlayerInfo& player = state.getGame().getPlayers().at(state.getGame().getCurrentPlayer());

    DrawTextEx(font, "Kartu Kemampuan", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Gunakan kartu dari tangan pemain aktif.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (player.getHandCards().empty()) {
        toolkit.drawWrappedText("Belum ada kartu di tangan.", {modal.x + 28.0f, modal.y + 126.0f, modal.width - 56.0f, 60.0f}, 20.0f, toolkit.theme().getCoral());
    } else {
        for (int index = 0; index < static_cast<int>(player.getHandCards().size()); index++) {
            const CardInfo& card = player.getHandCards().at(index);
            const Rectangle cardRect = {modal.x + 28.0f, modal.y + 122.0f + index * 96.0f, modal.width - 56.0f, 84.0f};
            toolkit.drawPanel(cardRect, toolkit.mix(toolkit.theme().getPaperSoft(), card.getAccent(), 0.14f), toolkit.withAlpha(card.getAccent(), 0.24f), 0.0f);
            DrawTextEx(font, card.getTitle().c_str(), {cardRect.x + 18.0f, cardRect.y + 14.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
            toolkit.drawWrappedText(card.getDescription(), {cardRect.x + 18.0f, cardRect.y + 44.0f, cardRect.width - 140.0f, 30.0f}, 17.0f, toolkit.theme().getInkMuted(), 3.0f, 2);
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {cardRect.x + cardRect.width - 96.0f, cardRect.y + 22.0f, 78.0f, 36.0f}, index == state.getOverlay().getSelectedIndex() ? card.getAccent() : toolkit.mix(toolkit.theme().getPaper(), card.getAccent(), 0.16f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }
    }

    const bool hasCards = !player.getHandCards().empty();
    const bool canUse = session.canUseHandCardNow();
    const std::string hint = (!canUse && hasCards && state.getGame().isTurnStarted())
        ? "Kartu hanya bisa digunakan di awal giliran sebelum lempar dadu."
        : "Kartu Teleport akan menuju tile yang sedang dipilih di board.";
    DrawTextEx(font, hint.c_str(), {modal.x + 28.0f, modal.y + modal.height - 104.0f}, 16.0f, 1.0f, state.getGame().isRolledThisTurn() ? toolkit.theme().getCoral() : toolkit.theme().getTeal());
    if (toolkit.drawButton("Gunakan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 120.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), canUse, 20.0f)) {
        session.useSelectedHandCard();
    }
    if (toolkit.drawButton("Buang", {modal.x + 164.0f, modal.y + modal.height - 64.0f, 120.0f, 42.0f}, toolkit.mix(toolkit.theme().getCoral(), WHITE, 0.18f), toolkit.theme().getInk(), canUse, 20.0f)) {
        session.dropSelectedHandCard();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawPortfolio(GUIGameController& session, const UiToolkit& toolkit) const {
    const AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.64f, 0.72f);
    const Font font = toolkit.font();
    const PlayerInfo& player = state.getGame().getPlayers().at(state.getGame().getCurrentPlayer());

    DrawTextEx(font, "Portfolio", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, player.getName().c_str(), {modal.x + 28.0f, modal.y + 68.0f}, 22.0f, 1.0f, player.getAccent());
    DrawTextEx(font, ("Total kekayaan: " + toolkit.formatMoney(session.computeNetWorth(state.getGame().getCurrentPlayer()))).c_str(), {modal.x + 28.0f, modal.y + 100.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    const Rectangle assets = {modal.x + 28.0f, modal.y + 140.0f, modal.width - 56.0f, modal.height - 220.0f};
    toolkit.drawPanel(assets, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getSage(), 0.06f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);

    if (player.getProperties().empty()) {
        DrawTextEx(font, "Belum ada properti.", {assets.x + 18.0f, assets.y + 18.0f}, 20.0f, 1.0f, toolkit.theme().getInkMuted());
    } else {
        float y = assets.y + 18.0f;
        for (int tileIndex : player.getProperties()) {
            const TileInfo& tile = state.getGame().getBoard().at(tileIndex);
            const Rectangle row = {assets.x + 16.0f, y, assets.width - 32.0f, 50.0f};
            DrawRectangleRec(row, toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.10f));
            DrawTextEx(font, tile.getName().c_str(), {row.x + 14.0f, row.y + 10.0f}, 20.0f, 1.0f, toolkit.theme().getInk());
            DrawTextEx(font, toolkit.formatMoney(session.computeTileAssetValue(tile)).c_str(), {row.x + row.width - 100.0f, row.y + 10.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
            y += 58.0f;
            if (y > assets.y + assets.height - 52.0f) {
                break;
            }
        }
    }

    DrawTextEx(font, ("Kartu di tangan: " + std::to_string(player.getHandCards().size())).c_str(), {modal.x + 28.0f, modal.y + modal.height - 100.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 62.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawBuild(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.60f, 0.70f);
    const Font font = toolkit.font();
    const std::vector<int> streets = session.currentPlayerBuildOptions();

    DrawTextEx(font, "Bangun", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Tambah rumah atau upgrade hotel pada properti yang memenuhi syarat.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (streets.empty()) {
        toolkit.drawWrappedText("Tidak ada street yang siap dibangun. Butuh street milik sendiri dan tidak sedang tergadai.", {modal.x + 28.0f, modal.y + 128.0f, modal.width - 56.0f, 60.0f}, 20.0f, toolkit.theme().getCoral());
    } else {
        for (int index = 0; index < static_cast<int>(streets.size()); index++) {
            const TileInfo& tile = state.getGame().getBoard().at(streets.at(index));
            const Rectangle row = {modal.x + 28.0f, modal.y + 122.0f + index * 76.0f, modal.width - 56.0f, 66.0f};
            DrawRectangleRec(row, index == state.getOverlay().getSelectedIndex() ? toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.08f));
            DrawTextEx(font, tile.getName().c_str(), {row.x + 16.0f, row.y + 10.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
            DrawTextEx(font, ("Bangunan: " + std::to_string(tile.getBuildings())).c_str(), {row.x + 16.0f, row.y + 36.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {row.x + row.width - 84.0f, row.y + 16.0f, 68.0f, 34.0f}, index == state.getOverlay().getSelectedIndex() ? tile.getAccent() : toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.14f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }
    }

    if (toolkit.drawButton("Tambah Bangunan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 186.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), !streets.empty(), 20.0f)) {
        session.buildOnSelectedTile();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawMortgage(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.60f, 0.68f);
    const Font font = toolkit.font();
    const std::vector<int> items = session.currentPlayerMortgageOptions();

    DrawTextEx(font, "Mortgage", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Gadai aset untuk menambah cash secara instan.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (items.empty()) {
        toolkit.drawWrappedText("Tidak ada aset yang bisa digadai sekarang.", {modal.x + 28.0f, modal.y + 128.0f, modal.width - 56.0f, 56.0f}, 20.0f, toolkit.theme().getCoral());
    } else {
        for (int index = 0; index < static_cast<int>(items.size()); index++) {
            const TileInfo& tile = state.getGame().getBoard().at(items.at(index));
            const Rectangle row = {modal.x + 28.0f, modal.y + 122.0f + index * 70.0f, modal.width - 56.0f, 60.0f};
            DrawRectangleRec(row, index == state.getOverlay().getSelectedIndex() ? toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.08f));
            DrawTextEx(font, tile.getName().c_str(), {row.x + 16.0f, row.y + 12.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
            DrawTextEx(font, ("Nilai gadai: " + toolkit.formatMoney(session.getMortgageValue(tile))).c_str(), {row.x + 16.0f, row.y + 36.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {row.x + row.width - 84.0f, row.y + 14.0f, 68.0f, 32.0f}, index == state.getOverlay().getSelectedIndex() ? tile.getAccent() : toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.14f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }
    }

    if (toolkit.drawButton("Gadaikan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 132.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), !items.empty(), 20.0f)) {
        session.mortgageSelectedTile();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawRedeem(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.60f, 0.68f);
    const Font font = toolkit.font();
    const std::vector<int> items = session.currentPlayerRedeemOptions();

    DrawTextEx(font, "Redeem", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Tebus kembali aset tergadai dengan biaya 110% dari nilai gadai.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (items.empty()) {
        toolkit.drawWrappedText("Belum ada aset tergadai untuk ditebus.", {modal.x + 28.0f, modal.y + 128.0f, modal.width - 56.0f, 56.0f}, 20.0f, toolkit.theme().getCoral());
    } else {
        for (int index = 0; index < static_cast<int>(items.size()); index++) {
            const TileInfo& tile = state.getGame().getBoard().at(items.at(index));
            const Rectangle row = {modal.x + 28.0f, modal.y + 122.0f + index * 70.0f, modal.width - 56.0f, 60.0f};
            DrawRectangleRec(row, index == state.getOverlay().getSelectedIndex() ? toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.08f));
            DrawTextEx(font, tile.getName().c_str(), {row.x + 16.0f, row.y + 12.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
            DrawTextEx(font, ("Biaya tebus: " + toolkit.formatMoney(session.getRedeemCost(tile))).c_str(), {row.x + 16.0f, row.y + 36.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {row.x + row.width - 84.0f, row.y + 14.0f, 68.0f, 32.0f}, index == state.getOverlay().getSelectedIndex() ? tile.getAccent() : toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.14f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }
    }

    if (toolkit.drawButton("Tebus", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 118.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), !items.empty(), 20.0f)) {
        session.redeemSelectedTile();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawSave(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.52f, 0.48f);
    const Font font = toolkit.font();

    DrawTextEx(font, "Simpan Game", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Masukkan nama save untuk menyimpan permainan.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
    toolkit.drawTextField(state, "save-name", state.getSaveInput(), "nama save", {modal.x + 28.0f, modal.y + 128.0f, modal.width - 56.0f, 58.0f}, 24);

    if (toolkit.drawButton("Simpan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 160.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.saveSession();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawLogs(GUIGameController& session, const UiToolkit& toolkit) const {
    const AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.66f, 0.78f);
    const Font font = toolkit.font();
    DrawTextEx(font, "Log History", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());

    const Rectangle scroller = {modal.x + 28.0f, modal.y + 86.0f, modal.width - 56.0f, modal.height - 160.0f};
    toolkit.drawPanel(scroller, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.06f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);

    float y = scroller.y + 14.0f;
    const int count = static_cast<int>(state.getGame().getLogs().size());
    for (int index = std::max(0, count - 12); index < count; index++) {
        const LogItem& log = state.getGame().getLogs().at(index);
        const Rectangle row = {scroller.x + 14.0f, y, scroller.width - 28.0f, 58.0f};
        DrawRectangleRec(row, toolkit.mix(toolkit.theme().getPaperSoft(), toolkit.theme().getPaper(), 0.35f));
        toolkit.drawBadge("T" + std::to_string(log.getTurn()), {row.x + 12.0f, row.y + 16.0f, 46.0f, 24.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.16f), toolkit.theme().getInk());
        DrawTextEx(font, (log.getActor() + " - " + log.getAction()).c_str(), {row.x + 68.0f, row.y + 10.0f}, 18.0f, 1.0f, toolkit.theme().getInk());
        toolkit.drawWrappedText(log.getDetail(), {row.x + 68.0f, row.y + 30.0f, row.width - 80.0f, 20.0f}, 16.0f, toolkit.theme().getInkMuted(), 3.0f, 1);
        y += 64.0f;
    }

    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 62.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawSetDice(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.50f, 0.56f);
    const Font font = toolkit.font();

    DrawTextEx(font, "Atur Dadu", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Pilih nilai dadu manual untuk giliran ini.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    auto drawDiePicker = [&](int dieIndex, int value, Rectangle rect, Color accent) {
        toolkit.drawPanel(rect, toolkit.mix(toolkit.theme().getPaperSoft(), accent, 0.12f), toolkit.withAlpha(accent, 0.22f), 0.0f);
        DrawTextEx(font, ("Dadu " + std::to_string(dieIndex)).c_str(), {rect.x + 18.0f, rect.y + 16.0f}, 20.0f, 1.0f, toolkit.theme().getInk());

        const std::string valueText = std::to_string(value);
        const Vector2 valueSize = MeasureTextEx(font, valueText.c_str(), 58.0f, 1.0f);
        DrawTextEx(font, valueText.c_str(), {rect.x + (rect.width - valueSize.x) * 0.5f, rect.y + 62.0f}, 58.0f, 1.0f, accent);

        const float buttonY = rect.y + rect.height - 54.0f;
        if (toolkit.drawButton("-", {rect.x + 18.0f, buttonY, 70.0f, 38.0f}, toolkit.mix(toolkit.theme().getPaper(), accent, 0.16f), toolkit.theme().getInk(), value > 1, 22.0f)) {
            session.adjustManualDie(dieIndex, -1);
        }
        if (toolkit.drawButton("+", {rect.x + rect.width - 88.0f, buttonY, 70.0f, 38.0f}, accent, toolkit.theme().getPaperSoft(), value < 6, 22.0f)) {
            session.adjustManualDie(dieIndex, 1);
        }
    };

    const float pickerWidth = (modal.width - 76.0f) * 0.5f;
    const Rectangle firstDie = {modal.x + 28.0f, modal.y + 126.0f, pickerWidth, 184.0f};
    const Rectangle secondDie = {firstDie.x + firstDie.width + 20.0f, firstDie.y, pickerWidth, firstDie.height};
    drawDiePicker(1, state.getOverlay().getManualDieOne(), firstDie, toolkit.theme().getTeal());
    drawDiePicker(2, state.getOverlay().getManualDieTwo(), secondDie, toolkit.theme().getCoral());

    const bool canApply = state.getGame().isTurnStarted() && !state.getGame().isRolledThisTurn();
    const std::string status = canApply
        ? "Nilai ini akan dipakai sebagai hasil lempar dadu."
        : (state.getGame().isRolledThisTurn() ? "Dadu sudah dilempar pada turn ini." : "Turn belum siap untuk mengatur dadu.");
    toolkit.drawWrappedText(status, {modal.x + 28.0f, modal.y + 334.0f, modal.width - 56.0f, 44.0f}, 18.0f, canApply ? toolkit.theme().getInkMuted() : toolkit.theme().getCoral());

    if (toolkit.drawButton("Roll Dadu", {modal.x + 28.0f, modal.y + modal.height - 66.0f, 180.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), canApply, 20.0f)) {
        session.applyManualDice();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 66.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawHelp(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.62f, 0.72f);
    const Font font = toolkit.font();

    DrawTextEx(font, "Help", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Ringkasan aksi yang tersedia saat gameplay.", {modal.x + 28.0f, modal.y + 68.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    const Rectangle list = {modal.x + 28.0f, modal.y + 112.0f, modal.width - 56.0f, modal.height - 190.0f};
    toolkit.drawPanel(list, toolkit.mix(toolkit.theme().getPaper(), toolkit.theme().getTeal(), 0.05f), toolkit.withAlpha(toolkit.theme().getInkMuted(), 0.10f), 0.0f);

    const std::vector<std::string> lines = {
        "Turn dimulai otomatis untuk pemain aktif.",
        "Roll Dice: lempar dadu acak dan gerakkan pion.",
        "Atur Dadu: pilih nilai dadu manual 1-6 untuk giliran ini.",
        "Turn selesai otomatis setelah Roll Dice atau Atur Dadu selesai diproses.",
        "Detail Tile: buka akta atau detail petak yang dipilih.",
        "Portfolio: lihat saldo, aset, dan jumlah kartu pemain aktif.",
        "Cards: gunakan kartu yang sedang ada di tangan pemain.",
        "Save: simpan permainan sebelum dadu dilempar pada turn berjalan.",
        "Logs: lihat riwayat aksi permainan.",
        "Modal lain akan muncul saat pemain mendarat di petak khusus."
    };

    float y = list.y + 18.0f;
    for (const std::string& line : lines) {
        DrawCircle(static_cast<int>(list.x + 18.0f), static_cast<int>(y + 9.0f), 4.0f, toolkit.theme().getGold());
        toolkit.drawWrappedText(line, {list.x + 34.0f, y, list.width - 52.0f, 36.0f}, 17.0f, toolkit.theme().getInk(), 3.0f, 2);
        y += 42.0f;
    }

    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawForceDrop(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.58f, 0.70f);
    const Font font = toolkit.font();
    const PlayerInfo& player = state.getGame().getPlayers().at(state.getGame().getCurrentPlayer());
    const int cardCount = static_cast<int>(player.getHandCards().size());

    DrawTextEx(font, "Batas Kartu", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, (player.getName() + ": " + std::to_string(cardCount) + "/3 kartu").c_str(), {modal.x + 28.0f, modal.y + 68.0f}, 20.0f, 1.0f, cardCount > 3 ? toolkit.theme().getCoral() : toolkit.theme().getSuccess());

    if (cardCount <= 3) {
        toolkit.drawWrappedText("Jumlah kartu pemain sudah sesuai batas.", {modal.x + 28.0f, modal.y + 126.0f, modal.width - 56.0f, 60.0f}, 21.0f, toolkit.theme().getInkMuted());
    } else {
        DrawTextEx(font, "Pilih kartu yang akan dibuang sampai tersisa maksimal 3 kartu.", {modal.x + 28.0f, modal.y + 102.0f}, 17.0f, 1.0f, toolkit.theme().getInkMuted());
        const float listTop = modal.y + 138.0f;
        const float footerTop = modal.y + modal.height - 118.0f;
        const float rowGap = 7.0f;
        const float availableRowsHeight = std::max(64.0f, footerTop - listTop);
        const int visibleCount = std::min(cardCount, std::max(1, static_cast<int>((availableRowsHeight + rowGap) / 64.0f)));
        const float rowHeight = std::min(66.0f, (availableRowsHeight - (visibleCount - 1) * rowGap) / visibleCount);
        const float titleSize = rowHeight < 58.0f ? 18.0f : 21.0f;
        const float bodySize = rowHeight < 58.0f ? 13.0f : 15.0f;
        for (int index = 0; index < visibleCount; index++) {
            const CardInfo& card = player.getHandCards().at(index);
            const Rectangle row = {modal.x + 28.0f, listTop + index * (rowHeight + rowGap), modal.width - 56.0f, rowHeight};
            DrawRectangleRec(row, index == state.getOverlay().getSelectedIndex() ? toolkit.mix(toolkit.theme().getPaperSoft(), card.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), card.getAccent(), 0.08f));
            DrawTextEx(font, card.getTitle().c_str(), {row.x + 16.0f, row.y + 8.0f}, titleSize, 1.0f, toolkit.theme().getInk());
            toolkit.drawWrappedText(card.getDescription(), {row.x + 16.0f, row.y + rowHeight - 26.0f, row.width - 126.0f, 20.0f}, bodySize, toolkit.theme().getInkMuted(), 3.0f, 1);
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {row.x + row.width - 90.0f, row.y + (row.height - 34.0f) * 0.5f, 72.0f, 34.0f}, index == state.getOverlay().getSelectedIndex() ? card.getAccent() : toolkit.mix(toolkit.theme().getPaper(), card.getAccent(), 0.14f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }

        if (cardCount > visibleCount) {
            DrawTextEx(font, ("+" + std::to_string(cardCount - visibleCount) + " kartu lain akan bergeser setelah kartu dibuang.").c_str(), {modal.x + 28.0f, modal.y + modal.height - 104.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
        }
    }

    if (toolkit.drawButton("Buang Kartu", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 150.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), cardCount > 3, 20.0f)) {
        session.dropSelectedHandCard();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), cardCount <= 3, 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawLiquidation(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.62f, 0.74f);
    const Font font = toolkit.font();
    const GameState& game = state.getGame();
    int playerIndex = state.getOverlay().getSelectedPlayer();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(game.getPlayers().size())) {
        playerIndex = game.getCurrentPlayer();
    }
    const PlayerInfo& player = game.getPlayers().at(playerIndex);

    DrawTextEx(font, "Liquidation", {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, (player.getName() + " - saldo: " + toolkit.formatMoney(player.getMoney())).c_str(), {modal.x + 28.0f, modal.y + 68.0f}, 22.0f, 1.0f, player.getMoney() >= 0 ? toolkit.theme().getInk() : toolkit.theme().getDanger());
    DrawTextEx(font, "Jual atau gadaikan aset sampai kewajiban backend terpenuhi.", {modal.x + 28.0f, modal.y + 104.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());

    if (player.getProperties().empty()) {
        toolkit.drawWrappedText("Tidak ada aset tersisa. Satu-satunya opsi adalah bangkrut.", {modal.x + 28.0f, modal.y + 148.0f, modal.width - 56.0f, 56.0f}, 20.0f, toolkit.theme().getCoral());
    } else {
        for (int index = 0; index < static_cast<int>(player.getProperties().size()); index++) {
            const int tileIndex = player.getProperties().at(index);
            const TileInfo& tile = state.getGame().getBoard().at(tileIndex);
            const Rectangle row = {modal.x + 28.0f, modal.y + 148.0f + index * 68.0f, modal.width - 56.0f, 58.0f};
            DrawRectangleRec(row, index == state.getOverlay().getSelectedIndex() ? toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.18f) : toolkit.mix(toolkit.theme().getPaperSoft(), tile.getAccent(), 0.08f));
            DrawTextEx(font, tile.getName().c_str(), {row.x + 16.0f, row.y + 10.0f}, 22.0f, 1.0f, toolkit.theme().getInk());
            DrawTextEx(font, ("Jual: " + toolkit.formatMoney(session.getSellToBankValue(tile)) + " | Gadai: " + toolkit.formatMoney(session.getMortgageValue(tile))).c_str(), {row.x + 16.0f, row.y + 34.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
            if (toolkit.drawButton(index == state.getOverlay().getSelectedIndex() ? "Terpilih" : "Pilih", {row.x + row.width - 84.0f, row.y + 12.0f, 68.0f, 34.0f}, index == state.getOverlay().getSelectedIndex() ? tile.getAccent() : toolkit.mix(toolkit.theme().getPaper(), tile.getAccent(), 0.14f), index == state.getOverlay().getSelectedIndex() ? toolkit.theme().getPaperSoft() : toolkit.theme().getInk(), true, 16.0f)) {
                state.getOverlay().setSelectedIndex(index);
            }
        }
    }

    bool canMortgageSelection = false;
    if (!player.getProperties().empty()) {
        const int selected = std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(player.getProperties().size()) - 1));
        const TileInfo& selectedTile = state.getGame().getBoard().at(player.getProperties().at(selected));
        canMortgageSelection = !selectedTile.isMortgaged();
    }

    if (toolkit.drawButton("Jual Aset", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 136.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), !player.getProperties().empty(), 20.0f)) {
        session.liquidateSelectedTile();
    }
    if (toolkit.drawButton("Gadai", {modal.x + 180.0f, modal.y + modal.height - 64.0f, 104.0f, 42.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.16f), toolkit.theme().getInk(), canMortgageSelection, 20.0f)) {
        session.mortgageLiquidationSelectedTile();
    }
    if (toolkit.drawButton("Bangkrut", {modal.x + 302.0f, modal.y + modal.height - 64.0f, 120.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.declareBankrupt();
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), !session.isLiquidationRequired(), 20.0f)) {
        session.closeOverlay();
    }
}

void OverlayRenderer::drawCardDraw(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.52f, 0.56f);
    const Font font = toolkit.font();
    const CardInfo& card = state.getOverlay().getCard();
    const std::string deckName = state.getOverlay().getDeckKey() == kCommunityDeckKey ? "Dana Umum" : "Chance";

    DrawTextEx(font, deckName.c_str(), {modal.x + 28.0f, modal.y + 28.0f}, 34.0f, 1.0f, toolkit.theme().getInk());

    const Rectangle cardRect = {modal.x + 28.0f, modal.y + 90.0f, modal.width - 56.0f, 210.0f};
    toolkit.drawPanel(cardRect, toolkit.mix(toolkit.theme().getPaperSoft(), card.getAccent(), 0.18f), toolkit.withAlpha(card.getAccent(), 0.24f), 0.0f);
    DrawTextEx(font, card.getTitle().c_str(), {cardRect.x + 20.0f, cardRect.y + 20.0f}, 30.0f, 1.0f, toolkit.theme().getInk());
    toolkit.drawWrappedText(card.getDescription(), {cardRect.x + 20.0f, cardRect.y + 70.0f, cardRect.width - 40.0f, 84.0f}, 20.0f, toolkit.theme().getInkMuted(), 6.0f);
    DrawTextEx(font, card.getKind() == CardKind::Hand ? "Kartu ini bisa disimpan ke tangan." : "Kartu ini langsung dieksekusi.", {cardRect.x + 20.0f, cardRect.y + 170.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());

    if (card.getKind() == CardKind::Hand) {
        if (toolkit.drawButton("Simpan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 120.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
            session.storeDrawnCard();
        }
    } else {
        if (toolkit.drawButton("Terapkan", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 132.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
            session.applyDrawnCard();
        }
    }
    toolkit.drawButton("Wajib", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), false, 20.0f);
}

void OverlayRenderer::drawGameOver(GUIGameController& session, const UiToolkit& toolkit) const {
    AppState& state = session.state();
    const Rectangle modal = toolkit.drawModalShell(state.getOverlay().getAnim(), 0.58f, 0.72f);
    const Font font = toolkit.font();

    std::vector<int> ranking(state.getGame().getPlayers().size());
    for (int index = 0; index < static_cast<int>(ranking.size()); index++) {
        ranking.at(index) = index;
    }
    std::sort(ranking.begin(), ranking.end(), [&](int left, int right) {
        return session.computeNetWorth(left) > session.computeNetWorth(right);
    });

    DrawTextEx(font, "Game Over", {modal.x + 28.0f, modal.y + 28.0f}, 36.0f, 1.0f, toolkit.theme().getInk());
    DrawTextEx(font, "Ranking berdasarkan total kekayaan saat permainan selesai.", {modal.x + 28.0f, modal.y + 70.0f}, 19.0f, 1.0f, toolkit.theme().getInkMuted());

    for (int rank = 0; rank < static_cast<int>(ranking.size()); rank++) {
        const PlayerInfo& player = state.getGame().getPlayers().at(ranking.at(rank));
        const Rectangle row = {modal.x + 28.0f, modal.y + 126.0f + rank * 82.0f, modal.width - 56.0f, 68.0f};
        toolkit.drawPanel(row, toolkit.mix(toolkit.theme().getPaperSoft(), player.getAccent(), 0.12f), toolkit.withAlpha(player.getAccent(), 0.20f), 0.0f);
        toolkit.drawBadge("#" + std::to_string(rank + 1), {row.x + 16.0f, row.y + 18.0f, 52.0f, 28.0f}, toolkit.mix(toolkit.theme().getGold(), WHITE, 0.20f), toolkit.theme().getInk());
        DrawTextEx(font, player.getName().c_str(), {row.x + 84.0f, row.y + 12.0f}, 24.0f, 1.0f, toolkit.theme().getInk());
        DrawTextEx(font, toolkit.formatMoney(session.computeNetWorth(ranking.at(rank))).c_str(), {row.x + 84.0f, row.y + 38.0f}, 18.0f, 1.0f, toolkit.theme().getInkMuted());
        DrawTextEx(font, ("Aset: " + std::to_string(player.getProperties().size()) + " - Kartu: " + std::to_string(player.getHandCards().size())).c_str(), {row.x + row.width - 170.0f, row.y + 24.0f}, 16.0f, 1.0f, toolkit.theme().getInkMuted());
    }

    if (toolkit.drawButton("Main Lagi", {modal.x + 28.0f, modal.y + modal.height - 64.0f, 132.0f, 42.0f}, toolkit.theme().getTeal(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.startFreshSession();
    }
    if (toolkit.drawButton("Menu Utama", {modal.x + 176.0f, modal.y + modal.height - 64.0f, 142.0f, 42.0f}, toolkit.theme().getCoral(), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
        session.state().setScreen(Screen::MainMenu);
    }
    if (toolkit.drawButton("Tutup", {modal.x + modal.width - 120.0f, modal.y + modal.height - 64.0f, 92.0f, 42.0f}, toolkit.mix(toolkit.theme().getNavy(), WHITE, 0.18f), toolkit.theme().getPaperSoft(), true, 20.0f)) {
        session.closeOverlay();
    }
}
}
