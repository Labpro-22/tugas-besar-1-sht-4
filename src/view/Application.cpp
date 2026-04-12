#include "view/Application.hpp"

using namespace std;

Application::Application()
    : controller(game, uiManager) {}

Application::Application(Game game, UIManager uiManager, bool running)
    : game(game), uiManager(uiManager), controller(this->game, this->uiManager), running(running) {}

Application::Application(const Application& other)
    : game(other.game), uiManager(other.uiManager), controller(game, uiManager), running(other.running) {}

Application::~Application() {}

Application& Application::operator=(const Application& other) {
    return *this;
}

void Application::run() {}

void Application::initialize() {}

void Application::showMainMenu() {}

int Application::getMainMenuChoice() {
    return 0;
}

void Application::handleNewGame() {}

void Application::handleLoadGame() {}

void Application::exitApplication() {}