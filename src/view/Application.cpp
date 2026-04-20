#include "view/Application.hpp"

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
    game.startNewGame();
    controller.runGameLoop();
}

void Application::handleLoadGame() {
    string filename = uiManager.readFilename();
    game.loadGame(filename);
    controller.runGameLoop();
}

void Application::exitApplication() {
    running = false;
    uiManager.printMessage("Terima kasih telah bermain.");
}