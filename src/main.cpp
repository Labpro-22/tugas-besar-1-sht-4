#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <string>

#include "view/Application.hpp"
#include "view/RaylibGuiApp.hpp"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

class MainApplication {
public:
    static int run(int argc, char* argv[]) {
        try {
            AppMode mode;

            if (argc > 1) {
                if (!tryParseMode(argv[1], mode)) {
                    cerr << "Mode tidak dikenal: " << argv[1] << endl;
                    cerr << "Gunakan 'cli' atau 'gui'." << endl;
                    return 1;
                }
            } else {
                mode = readRunMode();
            }

            switch (mode) {
                case AppMode::Cli: {
                    Application app;
                    app.run();
                    break;
                }
                case AppMode::Gui: {
                    RaylibGuiApp app;
                    app.run();
                    break;
                }
                case AppMode::Exit:
                    cout << "Keluar dari aplikasi." << endl;
                    break;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error : " << e.what() << std::endl;
            return 1;
        }
        catch (...) {
            std::cerr << "Unknown Error" << std::endl;
            return 1;
        }

        return 0;
    }

private:
    enum class AppMode {
        Cli,
        Gui,
        Exit
    };

    static string normalizeChoice(string input) {
        input.erase(
            std::remove_if(input.begin(), input.end(), [](unsigned char character) {
                return std::isspace(character);
            }),
            input.end()
        );

        std::transform(input.begin(), input.end(), input.begin(), [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });

        return input;
    }

    static bool tryParseMode(const string& input, AppMode& mode) {
        string normalized = normalizeChoice(input);

        if (normalized == "1" || normalized == "cli" || normalized == "--cli") {
            mode = AppMode::Cli;
            return true;
        }

        if (normalized == "2" || normalized == "gui" || normalized == "--gui") {
            mode = AppMode::Gui;
            return true;
        }

        if (normalized == "0" || normalized == "exit" || normalized == "keluar" || normalized == "quit") {
            mode = AppMode::Exit;
            return true;
        }

        return false;
    }

    static AppMode readRunMode() {
        while (true) {
            cout << "Pilih mode aplikasi:" << endl;
            cout << "1. CLI" << endl;
            cout << "2. GUI" << endl;
            cout << "0. Keluar" << endl;
            cout << "Masukkan pilihan [1/2/0]: ";

            string input;
            if (!getline(cin, input)) {
                cout << endl << "Input tidak tersedia. Keluar dari aplikasi." << endl;
                return AppMode::Exit;
            }

            AppMode mode;
            if (tryParseMode(input, mode)) {
                return mode;
            }

            cout << "Pilihan tidak valid. Ketik 1/CLI atau 2/GUI." << endl << endl;
        }
    }
};

int main(int argc, char* argv[]) {
    return MainApplication::run(argc, argv);
}
