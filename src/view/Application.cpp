#include "view/Application.hpp"

#include <exception>

using namespace std;

Application::Application()
    : controller(game, uiManager), running(true) {}

Application::Application(Game game, UIManager uiManager, bool running)
    : game(game), uiManager(uiManager), controller(this->game, this->uiManager), running(running) {}

Application::Application(const Application& other)
    : game(other.game), uiManager(other.uiManager), controller(game, uiManager), running(other.running) {}

Application::~Application() {}

Application& Application::operator=(const Application& other) {
    if (this != &other) {
        game = other.game;
        uiManager = other.uiManager;
        running = other.running;
        controller = GameController(game, uiManager);
    }
    return *this;
}

void Application::run() {
    initialize();

    while (running) {
        showMainMenu();
        int choice = getMainMenuChoice();

        if (choice == -1) {
            running = false;
            break;
        }

        switch (choice) {
            case 1:
                handleNewGame();
                break;
            case 2:
                handleLoadGame();
                break;
            case 3:
                exitApplication();
                break;
            default:
                uiManager.printError("Pilihan tidak valid.");
                break;
        }
    }
}

void Application::initialize() {
    running = true;
}

void Application::showMainMenu() {
    uiManager.showMainMenu();
}

int Application::getMainMenuChoice() {
    return uiManager.readMainMenuChoice();
}

void Application::handleNewGame() {
    int playerCount = uiManager.readPlayerCount();
    if (playerCount < 2) {
        return;
    }
    vector<string> usernames = uiManager.readUsernames(playerCount);
    if (static_cast<int>(usernames.size()) != playerCount) {
        return;
    }
    vector<Player>& players = game.getPlayers();
    players.clear();

    game.getConfigManager().loadAllConfigs();
    int initialBalance = 1500;
    if (game.getConfigManager().getInitialBalance() > 0) initialBalance = game.getConfigManager().getInitialBalance();

    for (const string& username : usernames) {
        players.push_back(Player(
            username,
            initialBalance,
            1,
            PlayerStatus::ACTIVE,
            0,
            false,
            false,
            0,
            0
        ));
    }

    game.startNewGame();
    controller.runGameLoop();
}

void Application::handleLoadGame() {
    string filename = uiManager.readFilename();
    if (filename.empty()) {
        return;
    }

    try {
        uiManager.printMessage("Memuat permainan...");
        game.loadGame(filename);
        uiManager.printMessage(
            "Permainan berhasil dimuat. Melanjutkan giliran " +
            game.getCurrentPlayer().getUsername() + "..."
        );
        controller.runGameLoop();
    } catch (const exception& exception) {
        uiManager.printError(exception.what());
    }
}

void Application::exitApplication() {
    running = false;
    uiManager.printMessage("Terima kasih telah bermain.");
}