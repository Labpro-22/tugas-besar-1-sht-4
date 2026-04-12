#pragma once

#include "controller/GameController.hpp"
#include "model/Game.hpp"
#include "view/UIManager.hpp"

using namespace std;

class Application {
private:
    Game game;
    UIManager uiManager;
    GameController controller;
    bool running;

public:
    Application();
    Application(Game game, UIManager uiManager, bool running);
    Application(const Application& other);
    ~Application();
    Application& operator=(const Application& other);

    void run();
    void initialize();
    void showMainMenu();
    int getMainMenuChoice();
    void handleNewGame();
    void handleLoadGame();
    void exitApplication();
};