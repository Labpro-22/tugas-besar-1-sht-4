#include "view/Application.hpp"

#include <algorithm>
#include <exception>
#include <random>

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
    game.getConfigManager().loadAllConfigs();
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

    int comCount = uiManager.readComputerPlayerCount(playerCount);
    int humanCount = playerCount - comCount;

    vector<string> humanUsernames;
    if (humanCount > 0) {
        humanUsernames = uiManager.readUsernames(humanCount);
        if (static_cast<int>(humanUsernames.size()) != humanCount) {
            return;
        }
    }

    vector<string> allUsernames = humanUsernames;
    int comSuffix = 1;
    for (int i = 0; i < comCount; i++) {
        string comName = (comSuffix == 1) ? "ProfessorRayapSunggal" : "ProfessorRayapSunggal" + to_string(comSuffix);
        comSuffix++;
        while (find(allUsernames.begin(), allUsernames.end(), comName) != allUsernames.end()) {
            comName = "ProfessorRayapSunggal" + to_string(comSuffix++);
        }
        allUsernames.push_back(comName);
    }

    vector<Player>& players = game.getPlayers();
    players.clear();

    game.getConfigManager().loadAllConfigs();
    int initialBalance = 1500;
    if (game.getConfigManager().getInitialBalance() > 0) initialBalance = game.getConfigManager().getInitialBalance();

    for (int i = 0; i < static_cast<int>(allUsernames.size()); i++) {
        Player p(
            allUsernames[static_cast<size_t>(i)],
            initialBalance,
            1,
            PlayerStatus::ACTIVE,
            0,
            false,
            false,
            0,
            0
        );
        if (i >= humanCount) {
            p.setComputerPlayer(true);
        }
        players.push_back(p);
    }

    random_device device;
    mt19937 generator(device());
    shuffle(players.begin(), players.end(), generator);

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
        controller.resumeLoadedTurn();
        controller.runGameLoop();
    } catch (const exception& exception) {
        uiManager.printError(exception.what());
    }
}

void Application::exitApplication() {
    running = false;
    uiManager.printMessage("The moment you stop taking a step forward, you are taking a step backward");
    uiManager.printMessage("13524010 - Audric Yusuf Maynard Simatupang");  
}
