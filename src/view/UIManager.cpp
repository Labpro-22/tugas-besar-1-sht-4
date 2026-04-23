#include "view/UIManager.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

string UIManager::repeatChar(char ch, int count) const {
    return string(static_cast<size_t>(max(0, count)), ch);
}

string UIManager::trim(const string& value) const {
    size_t begin = 0;
    while (begin < value.size() && isspace(static_cast<unsigned char>(value[begin]))) {
        begin++;
    }

    size_t end = value.size();
    while (end > begin && isspace(static_cast<unsigned char>(value[end - 1]))) {
        end--;
    }

    return value.substr(begin, end - begin);
}

string UIManager::toUpperCopy(string value) const {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return value;
}

string UIManager::normalizeKey(const string& value) const {
    string normalized;
    for (char ch : value) {
        if (isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

string UIManager::truncateToWidth(const string& value, int width) const {
    if (static_cast<int>(value.size()) <= width) {
        return value;
    }
    if (width <= 1) {
        return value.substr(0, static_cast<size_t>(max(0, width)));
    }
    return value.substr(0, static_cast<size_t>(width - 1)) + ".";
}

string UIManager::padRight(const string& value, int width) const {
    const string clipped = truncateToWidth(value, width);
    return clipped + repeatChar(' ', width - static_cast<int>(clipped.size()));
}

string UIManager::centerText(const string& value, int width) const {
    const string clipped = truncateToWidth(value, width);
    const int left = (width - static_cast<int>(clipped.size())) / 2;
    const int right = width - static_cast<int>(clipped.size()) - left;
    return repeatChar(' ', left) + clipped + repeatChar(' ', right);
}

string UIManager::formatMoney(int amount) const {
    const bool negative = amount < 0;
    long long value = amount;
    if (negative) {
        value *= -1;
    }

    string digits = to_string(value);
    string formatted;
    int count = 0;

    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; i--) {
        if (count == 3) {
            formatted.push_back('.');
            count = 0;
        }
        formatted.push_back(digits[static_cast<size_t>(i)]);
        count++;
    }

    reverse(formatted.begin(), formatted.end());

    string prefix = "M";
    if (negative) prefix += "-";
    return prefix + formatted;
}

string UIManager::colorCode(const string& colorGroup) const {
    const string key = normalizeKey(colorGroup);
    if (key == "COKLAT" || key == "BROWN" || key == "CK") return "CK";
    if (key == "BIRUMUDA" || key == "LIGHTBLUE" || key == "BM") return "BM";
    if (key == "PINK" || key == "PK") return "PK";
    if (key == "ORANGE" || key == "ORANYE" || key == "OR") return "OR";
    if (key == "MERAH" || key == "RED" || key == "MR") return "MR";
    if (key == "KUNING" || key == "YELLOW" || key == "KN") return "KN";
    if (key == "HIJAU" || key == "GREEN" || key == "HJ") return "HJ";
    if (key == "BIRUTUA" || key == "DARKBLUE" || key == "NAVY" || key == "BT") return "BT";
    if (key == "STASIUN" || key == "RAILROAD" || key == "RR") return "STASIUN";
    if (key == "UTILITAS" || key == "UTILITY" || key == "AB") return "AB";
    if (key == "AKSI" || key == "ACTION" || key == "DF") return "DF";
    if (colorGroup.empty()) return "DF";
    return colorGroup;
}

string UIManager::colorName(const string& colorGroup) const {
    const string code = colorCode(colorGroup);
    if (code == "CK") return "COKLAT";
    if (code == "BM") return "BIRU MUDA";
    if (code == "PK") return "PINK";
    if (code == "OR") return "ORANGE";
    if (code == "MR") return "MERAH";
    if (code == "KN") return "KUNING";
    if (code == "HJ") return "HIJAU";
    if (code == "BT") return "BIRU TUA";
    if (code == "STASIUN") return "STASIUN";
    if (code == "AB") return "UTILITAS";
    return "AKSI";
}

string UIManager::ansiForCode(const string& rawCode) const {
    const string code = colorCode(rawCode);
    if (code == "CK") return "\033[48;5;94;38;5;231m";
    if (code == "BM") return "\033[48;5;45;38;5;16m";
    if (code == "PK") return "\033[48;5;205;38;5;16m";
    if (code == "OR") return "\033[48;5;208;38;5;16m";
    if (code == "MR") return "\033[48;5;196;38;5;231m";
    if (code == "KN") return "\033[48;5;220;38;5;16m";
    if (code == "HJ") return "\033[48;5;34;38;5;231m";
    if (code == "BT") return "\033[48;5;19;38;5;231m";
    if (code == "AB") return "\033[48;5;51;38;5;16m";
    return "\033[48;5;240;38;5;231m";
}

int UIManager::findCellOffset(const vector<int>& cellIndices, int index) const {
    for (size_t i = 0; i < cellIndices.size(); i++) {
        if (cellIndices[i] == index) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

string UIManager::boardCellColor(
    const vector<int>& cellIndices,
    const vector<string>& cellColorCodes,
    int index
) const {
    const int offset = findCellOffset(cellIndices, index);
    if (offset >= 0 && offset < static_cast<int>(cellColorCodes.size())) {
        return cellColorCodes[static_cast<size_t>(offset)];
    }
    return "DF";
}

string UIManager::boardCellLine(
    const vector<int>& cellIndices,
    const vector<vector<string>>& cellLines,
    int index,
    int lineIndex
) const {
    const int offset = findCellOffset(cellIndices, index);
    if (
        offset >= 0 &&
        offset < static_cast<int>(cellLines.size()) &&
        lineIndex >= 0 &&
        lineIndex < static_cast<int>(cellLines[static_cast<size_t>(offset)].size())
    ) {
        return cellLines[static_cast<size_t>(offset)][static_cast<size_t>(lineIndex)];
    }

    if (lineIndex == 0) {
        return "[DF] ???";
    }
    return "";
}

string UIManager::colorizeCellLine(const string& colorCodeValue, const string& line) const {
    return ansiForCode(colorCodeValue) + line + "\033[0m";
}

string UIManager::horizontalBorder(int cellCount) const {
    string border = "+";
    for (int i = 0; i < cellCount; i++) {
        border += repeatChar('-', CELL_WIDTH) + "+";
    }
    return border;
}

string UIManager::sideBorder() const {
    return "+" + repeatChar('-', CELL_WIDTH) + "+" +
           repeatChar(' ', CENTER_WIDTH) +
           "+" + repeatChar('-', CELL_WIDTH) + "+";
}

void UIManager::printTileRow(
    const vector<int>& cellIndices,
    const vector<string>& cellColorCodes,
    const vector<vector<string>>& cellLines,
    const vector<int>& rowIndices
) const {
    for (int line = 0; line < CELL_HEIGHT; line++) {
        cout << "|";
        for (int index : rowIndices) {
            const string lineText = boardCellLine(cellIndices, cellLines, index, line);
            const string color = boardCellColor(cellIndices, cellColorCodes, index);
            cout << colorizeCellLine(color, padRight(lineText, CELL_WIDTH)) << "|";
        }
        cout << '\n';
    }
}

vector<string> UIManager::boardCenterLines(int currentTurn, int maxTurn, const string& currentPlayerLabel) const {
    vector<string> lines(27, "");
    const string maxTurnText = maxTurn > 0 ? to_string(maxTurn) : "TANPA BATAS";
    lines[2] = "==================================";
    lines[3] = "||          NIMONSPOLI          ||";
    lines[4] = "==================================";
    lines[6] = "TURN " + to_string(currentTurn) + " / " + maxTurnText;
    lines[9] = "----------------------------------";
    lines[10] = "LEGENDA KEPEMILIKAN & STATUS";
    lines[11] = "P1-P4 : Properti milik Pemain 1-4";
    lines[12] = "^     : Rumah Level 1";
    lines[13] = "^^    : Rumah Level 2";
    lines[14] = "^^^   : Rumah Level 3";
    lines[15] = "* : Hotel (Maksimal)";
    lines[16] = "(1)-(4): Bidak (IN=Tahanan, V=Mampir)";
    string currentPlayerText = currentPlayerLabel;
    if (currentPlayerText.empty()) currentPlayerText = "-";
    lines[17] = "Giliran: " + currentPlayerText;
    lines[19] = "----------------------------------";
    lines[20] = "KODE WARNA:";
    lines[21] = "[CK]=Coklat    [MR]=Merah";
    lines[22] = "[BM]=Biru Muda [KN]=Kuning";
    lines[23] = "[PK]=Pink      [HJ]=Hijau";
    lines[24] = "[OR]=Orange    [BT]=Biru Tua";
    lines[25] = "[DF]=Aksi      [AB]=Utilitas";
    return lines;
}

int UIManager::readIntInRange(const string& prompt, int minimum, int maximum, bool hasMaximum) const {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.eof()) {
            return -1;
        }

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input harus berupa angka.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (value < minimum) {
            cout << "Pilihan minimal " << minimum << ".\n";
            continue;
        }
        if (hasMaximum && value > maximum) {
            cout << "Pilihan maksimal " << maximum << ".\n";
            continue;
        }
        return value;
    }
}

size_t UIManager::propertyOptionCount(
    const vector<string>& propertyGroups,
    const vector<string>& propertyNames,
    const vector<string>& propertyCodes,
    const vector<string>& valueLabels,
    const vector<int>& values,
    const vector<string>& propertyStatuses
) const {
    size_t count = propertyGroups.size();
    count = min(count, propertyNames.size());
    count = min(count, propertyCodes.size());
    count = min(count, valueLabels.size());
    count = min(count, values.size());
    count = min(count, propertyStatuses.size());
    return count;
}

void UIManager::printShortPropertyLine(
    int index,
    const string& propertyGroup,
    const string& propertyName,
    const string& propertyCode,
    const string& valueLabel,
    int value,
    const string& propertyStatus
) const {
    string status = propertyStatus;
    if (status == "MORTGAGED") {
        status = "MORTGAGED [M]";
    }

    cout << setw(2) << index << ". " << left << setw(24)
         << (propertyName + " (" + propertyCode + ")")
         << " [" << setw(10) << colorName(propertyGroup) << "] "
         << valueLabel << ": " << formatMoney(value);
    if (!status.empty()) {
        cout << " " << status;
    }
    cout << '\n' << right;
}

string UIManager::displayTitleFromDeedTitle(const string& title) const {
    const size_t closeBracket = title.find("] ");
    if (closeBracket == string::npos) {
        return title;
    }
    return title.substr(closeBracket + 2);
}

UIManager::UIManager() = default;

UIManager::UIManager(const UIManager& other) {
    (void) other;
}

UIManager::~UIManager() {}

UIManager& UIManager::operator=(const UIManager& other) {
    if (this != &other) {
    }
    return *this;
}

void UIManager::showMainMenu() const {
    cout << "=================================\n";
    cout << "        NIMONSPOLI CLI\n";
    cout << "=================================\n";
    cout << "1. New Game\n";
    cout << "2. Load Game\n";
    cout << "3. Exit\n";
    cout << "=================================\n";
}

int UIManager::readMainMenuChoice() const {
    return readIntInRange("Pilih menu (1-3): ", 1, 3, true);
}

int UIManager::readPlayerCount() const {
    return readIntInRange("Masukkan jumlah pemain (2-4): ", 2, 4, true);
}

vector<string> UIManager::readUsernames(int playerCount) const {
    vector<string> usernames;

    for (int i = 0; i < playerCount; i++) {
        string username;

        while (true) {
            cout << "Masukkan username pemain " << (i + 1) << ": ";
            getline(cin, username);
            if (cin.eof()) {
                return usernames;
            }
            username = trim(username);

            if (username.empty()) {
                cout << "Username tidak boleh kosong.\n";
                continue;
            }

            if (find(usernames.begin(), usernames.end(), username) != usernames.end()) {
                cout << "Username sudah digunakan. Pilih username lain.\n";
                continue;
            }

            usernames.push_back(username);
            break;
        }
    }

    return usernames;
}

string UIManager::readCommand() const {
    string command;
    cout << "> ";
    getline(cin, command);
    return trim(command);
}

void UIManager::printMessage(const string& msg) const {
    cout << msg << '\n';
}

void UIManager::printError(const string& msg) const {
    cerr << msg << '\n';
}

void UIManager::printHelp() const {
    cout << "=== Daftar Command ===\n";
    cout << "HELP                         : Menampilkan daftar command.\n";
    cout << "CETAK_PAPAN                  : Menampilkan papan, posisi pemain, dan status petak.\n";
    cout << "LEMPAR_DADU                  : Melempar dadu dan memindahkan pemain aktif.\n";
    cout << "ATUR_DADU <X> <Y>            : Mengatur hasil dadu manual, X dan Y bernilai 1-6.\n";
    cout << "CETAK_AKTA                   : Menampilkan akta properti berdasarkan kode petak.\n";
    cout << "CETAK_PROPERTI               : Menampilkan daftar properti pemain aktif.\n";
    cout << "GADAI                        : Menggadaikan properti yang memenuhi syarat.\n";
    cout << "TEBUS                        : Menebus properti yang sedang digadaikan.\n";
    cout << "BANGUN                       : Membangun rumah atau hotel pada properti yang memenuhi syarat.\n";
    cout << "GUNAKAN_KEMAMPUAN            : Menggunakan satu kartu kemampuan dari tangan.\n";
    cout << "CETAK_LOG                    : Menampilkan seluruh log transaksi.\n";
    cout << "CETAK_LOG <N>                : Menampilkan N log transaksi terakhir.\n";
    cout << "SIMPAN <nama_file>           : Menyimpan permainan sebelum melempar dadu.\n";
}

string UIManager::readPropertyCode() const {
    string code;
    while (true) {
        cout << "Masukkan kode petak: ";
        getline(cin, code);
        if (cin.eof()) {
            return "";
        }
        code = toUpperCopy(trim(code));
        if (!code.empty()) {
            return code;
        }
        cout << "Kode petak tidak boleh kosong.\n";
    }
}

void UIManager::printBoard(
    const vector<int>& cellIndices,
    const vector<string>& cellColorCodes,
    const vector<vector<string>>& cellLines,
    int currentTurn,
    int maxTurn,
    const string& currentPlayerLabel
) const {
    if (cellIndices.empty()) {
        cout << "Papan belum diinisialisasi.\n";
        return;
    }

    if (cellIndices.size() < 40) {
        cout << "=== Papan NIMONSPOLI ===\n";
        vector<int> sortedOffsets;
        for (size_t i = 0; i < cellIndices.size(); i++) {
            sortedOffsets.push_back(static_cast<int>(i));
        }

        sort(sortedOffsets.begin(), sortedOffsets.end(), [&](int first, int second) {
            return cellIndices[static_cast<size_t>(first)] < cellIndices[static_cast<size_t>(second)];
        });

        for (int offset : sortedOffsets) {
            const int index = cellIndices[static_cast<size_t>(offset)];
            cout << setw(2) << index << ". "
                 << boardCellLine(cellIndices, cellLines, index, 0) << " | "
                 << boardCellLine(cellIndices, cellLines, index, 1) << " | "
                 << boardCellLine(cellIndices, cellLines, index, 2) << '\n';
        }
        return;
    }

    vector<int> top;
    for (int index = 21; index <= 31; index++) top.push_back(index);

    vector<int> bottom;
    for (int index = 11; index >= 1; index--) bottom.push_back(index);

    const vector<string> centerLines = boardCenterLines(currentTurn, maxTurn, currentPlayerLabel);
    int centerLineIndex = 0;

    cout << horizontalBorder(11) << '\n';
    printTileRow(cellIndices, cellColorCodes, cellLines, top);
    cout << horizontalBorder(11) << '\n';

    for (int row = 0; row < 9; row++) {
        const int leftIndex = 20 - row;
        const int rightIndex = 32 + row;

        for (int line = 0; line < CELL_HEIGHT; line++) {
            const string leftLine = boardCellLine(cellIndices, cellLines, leftIndex, line);
            const string rightLine = boardCellLine(cellIndices, cellLines, rightIndex, line);
            const string leftColor = boardCellColor(cellIndices, cellColorCodes, leftIndex);
            const string rightColor = boardCellColor(cellIndices, cellColorCodes, rightIndex);

            cout << "|"
                 << colorizeCellLine(leftColor, padRight(leftLine, CELL_WIDTH))
                 << "|"
                 << centerText(centerLines[static_cast<size_t>(centerLineIndex++)], CENTER_WIDTH)
                 << "|"
                 << colorizeCellLine(rightColor, padRight(rightLine, CELL_WIDTH))
                 << "|\n";
        }
        cout << sideBorder() << '\n';
    }

    printTileRow(cellIndices, cellColorCodes, cellLines, bottom);
    cout << horizontalBorder(11) << '\n';
}

void UIManager::printDeed(
    const string& title,
    int purchasePrice,
    int mortgageValue,
    const vector<string>& moneyRowLabels,
    const vector<int>& moneyRowValues,
    const vector<string>& detailRowLabels,
    const vector<string>& detailRowValues,
    const string& ownershipStatus,
    const string& ownerName
) const {
    cout << "+================================+\n";
    cout << "|        AKTA KEPEMILIKAN        |\n";
    cout << "|" << centerText(title, 32) << "|\n";
    cout << "+================================+\n";
    cout << "| Harga Beli        : " << left << setw(11) << formatMoney(purchasePrice) << "|\n";
    cout << "| Nilai Gadai       : " << left << setw(11) << formatMoney(mortgageValue) << "|\n";

    const size_t moneyRows = min(moneyRowLabels.size(), moneyRowValues.size());
    if (moneyRows > 0) {
        cout << "+--------------------------------+\n";
        for (size_t i = 0; i < moneyRows; i++) {
            if (i > 0 && moneyRowLabels[i] == "Harga Rumah") {
                cout << "+--------------------------------+\n";
            }
            cout << "| " << left << setw(18) << truncateToWidth(moneyRowLabels[i], 18)
                 << ": " << setw(10) << formatMoney(moneyRowValues[i]) << "|\n";
        }
    }

    const size_t detailRows = min(detailRowLabels.size(), detailRowValues.size());
    bool hasVisibleDetailRows = false;
    for (size_t i = 0; i < detailRows; i++) {
        if (!(detailRowLabels[i] == "Bangunan" && detailRowValues[i] == "-")) {
            hasVisibleDetailRows = true;
        }
    }

    if (hasVisibleDetailRows) {
        cout << "+--------------------------------+\n";
        for (size_t i = 0; i < detailRows; i++) {
            if (detailRowLabels[i] == "Bangunan" && detailRowValues[i] == "-") {
                continue;
            }
            cout << "| " << left << setw(18) << truncateToWidth(detailRowLabels[i], 18)
                 << ": " << setw(10) << truncateToWidth(detailRowValues[i], 10) << "|\n";
        }
    }

    cout << "+================================+\n";
    string status = ownershipStatus;
    if (!ownerName.empty() && ownerName != "BANK") {
        status += " (" + ownerName + ")";
    }
    cout << "| Status : " << left << setw(23)
         << truncateToWidth(status, 23) << "|\n";
    cout << right;
    cout << "+================================+\n";
}

void UIManager::printPlayerProperties(
    const string& playerName,
    int playerMoney,
    int cardCount,
    const vector<string>& propertyGroups,
    const vector<string>& propertyNames,
    const vector<string>& propertyCodes,
    const vector<string>& propertyBuildingStatuses,
    const vector<string>& propertyStatuses,
    const vector<int>& propertyValues,
    int totalPropertyWealth
) const {
    cout << "=== Properti Milik: " << playerName << " ===\n\n";

    size_t count = propertyGroups.size();
    count = min(count, propertyNames.size());
    count = min(count, propertyCodes.size());
    count = min(count, propertyBuildingStatuses.size());
    count = min(count, propertyStatuses.size());
    count = min(count, propertyValues.size());

    if (count == 0) {
        cout << "Kamu belum memiliki properti apapun.\n";
        (void) playerMoney;
        (void) cardCount;
        return;
    }

    string currentGroup;
    for (size_t i = 0; i < count; i++) {
        const string group = colorName(propertyGroups[i]);
        if (group != currentGroup) {
            currentGroup = group;
            cout << "\n[" << group << "]\n";
        }

        string status = propertyStatuses[i];
        if (status == "MORTGAGED") {
            status = "MORTGAGED [M]";
        }

        cout << "- " << left << setw(30)
             << (propertyNames[i] + " (" + propertyCodes[i] + ")")
             << setw(12) << truncateToWidth(propertyBuildingStatuses[i], 12)
             << setw(8) << formatMoney(propertyValues[i])
             << status << '\n';
    }

    cout << right;
    cout << "Total kekayaan properti: " << formatMoney(totalPropertyWealth) << '\n';
}

void UIManager::printLogs(const vector<string>& formattedLogs) const {
    cout << "=== Log Transaksi Penuh ===\n\n";
    if (formattedLogs.empty()) {
        cout << "Belum ada log transaksi.\n";
        return;
    }

    for (const string& log : formattedLogs) {
        cout << log << '\n';
    }
}

void UIManager::printRecentLogs(const vector<string>& formattedLogs, int n) const {
    cout << "=== Log Transaksi (" << n << " Terakhir) ===\n\n";
    if (formattedLogs.empty()) {
        cout << "Belum ada log transaksi.\n";
        return;
    }

    for (const string& log : formattedLogs) {
        cout << log << '\n';
    }
}

void UIManager::printDiceRoll(int die1, int die2, int total) const {
    cout << "Mengocok dadu...\n";
    cout << "Hasil: " << die1 << " + " << die2 << " = " << total << '\n';
}

void UIManager::printPlayerMovement(
    const string& playerName,
    int steps,
    const string& destinationName,
    const string& destinationCode
) const {
    cout << "Memajukan Bidak " << playerName << " sebanyak " << steps << " petakk...\n";
    cout << "Bidak mendarat di: " << destinationName;
    (void) destinationCode;
    cout << ".\n";
}

void UIManager::printStreetPurchasePrompt(
    const string& playerName,
    int playerMoney,
    const string& title,
    int purchasePrice,
    int mortgageValue,
    const vector<string>& moneyRowLabels,
    const vector<int>& moneyRowValues,
    const vector<string>& detailRowLabels,
    const vector<string>& detailRowValues,
    const string& ownershipStatus,
    const string& ownerName
) const {
    (void) playerName;
    (void) mortgageValue;
    (void) detailRowLabels;
    (void) detailRowValues;
    (void) ownershipStatus;
    (void) ownerName;

    cout << "Kamu mendarat di " << displayTitleFromDeedTitle(title) << "!\n";
    cout << "+================================+\n";
    cout << "| " << left << setw(31) << truncateToWidth(title, 31) << "|\n";
    cout << "| Harga Beli    : " << setw(14) << formatMoney(purchasePrice) << "|\n";

    const size_t moneyRows = min(moneyRowLabels.size(), moneyRowValues.size());
    for (size_t i = 0; i < moneyRows; i++) {
        cout << "| " << left << setw(14) << truncateToWidth(moneyRowLabels[i], 14)
             << ": " << setw(14) << formatMoney(moneyRowValues[i]) << "|\n";
    }
    cout << right;
    cout << "+================================+\n";
    cout << "Uang kamu saat ini: " << formatMoney(playerMoney) << '\n';
    cout << "Apakah kamu ingin membeli properti ini seharga "
         << formatMoney(purchasePrice) << "? (y/n): ";
}

bool UIManager::readYesNo() const {
    string answer;
    while (true) {
        getline(cin, answer);
        if (cin.eof()) {
            return false;
        }
        answer = toUpperCopy(trim(answer));

        if (answer == "Y" || answer == "YES" || answer == "YA") {
            return true;
        }
        if (answer == "N" || answer == "NO" || answer == "TIDAK") {
            return false;
        }

        cout << "Masukkan y atau n: ";
    }
}

void UIManager::printRailroadAcquired(
    const string& playerName,
    const string& tileName,
    const string& tileCode
) const {
    cout << "Kamu mendarat di " << tileName << " (" << tileCode << ")!\n";
    (void) playerName;
    cout << "Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!\n";
}

void UIManager::printUtilityAcquired(
    const string& playerName,
    const string& tileName,
    const string& tileCode
) const {
    cout << "Kamu mendarat di " << tileName;
    if (!tileCode.empty()) {
        cout << " (" << tileCode << ")";
    }
    cout << "!\n";
    (void) playerName;
    cout << "Belum ada yang menginjaknya duluan, " << tileName
         << " kini menjadi milikmu!\n";
}

void UIManager::printRentPayment(
    const string& payerName,
    int payerMoney,
    const string& ownerName,
    int ownerMoney,
    const string& tileName,
    const string& tileCode,
    const string& condition,
    const string& festivalStatus,
    int rent
) const {
    cout << payerName << " mendarat di " << tileName << " (" << tileCode
         << "), milik " << ownerName << "!\n\n";

    if (!condition.empty()) {
        cout << "Kondisi      : " << condition << '\n';
    }
    if (!festivalStatus.empty()) {
        cout << "Festival     : " << festivalStatus << '\n';
    }

    cout << "Sewa         : " << formatMoney(rent) << "\n\n";
    cout << "Uang " << payerName << " : " << formatMoney(payerMoney)
         << " -> " << formatMoney(payerMoney - rent) << '\n';
    cout << "Uang " << ownerName << " : " << formatMoney(ownerMoney)
         << " -> " << formatMoney(ownerMoney + rent) << '\n';
}

void UIManager::printMortgagedNoRent(
    const string& tileName,
    const string& tileCode,
    const string& ownerName
) const {
    cout << "Kamu mendarat di " << tileName << " (" << tileCode
         << "), milik " << ownerName << ".\n";
    cout << "Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n";
}

void UIManager::printIncomeTaxState(
    const string& playerName,
    int playerMoney,
    int flatTax,
    int percentTax
) const {
    cout << playerName << " mendarat di Pajak Penghasilan (PPH)!\n";
    cout << "Pilih opsi pembayaran pajak:\n";
    cout << "1. Bayar flat " << formatMoney(flatTax) << '\n';
    cout << "2. Bayar " << percentTax << "% dari total kekayaan\n";
    cout << "(Pilih sebelum menghitung kekayaan!)\n";
    (void) playerMoney;
}

int UIManager::readIncomeTaxChoice() const {
    return readIntInRange("Pilihan (1/2): ", 1, 2, true);
}

void UIManager::printIncomeTaxBreakdown(
    int currentMoney,
    int cash,
    int propertyValue,
    int buildingValue,
    int totalWealth,
    int percent,
    int taxAmount
) const {
    cout << "\nTotal kekayaan kamu:\n";
    cout << "- Uang tunai           : " << formatMoney(cash) << '\n';
    cout << "- Harga beli properti  : " << formatMoney(propertyValue)
         << " (termasuk yang digadaikan)\n";
    cout << "- Harga beli bangunan  : " << formatMoney(buildingValue) << '\n';
    cout << "Total                  : " << formatMoney(totalWealth) << '\n';
    cout << "Pajak " << percent << "%              : " << formatMoney(taxAmount) << '\n';
    cout << "Uang kamu: " << formatMoney(currentMoney)
         << " -> " << formatMoney(currentMoney - taxAmount) << '\n';
}

void UIManager::printLuxuryTaxState(int playerMoney, int taxAmount) const {
    cout << "Kamu mendarat di Pajak Barang Mewah (PBM)!\n";
    cout << "Pajak sebesar " << formatMoney(taxAmount) << " langsung dipotong.\n";
    cout << "Uang kamu: " << formatMoney(playerMoney)
         << " -> " << formatMoney(playerMoney - taxAmount) << '\n';
}

void UIManager::printFestivalState(
    const string& playerName,
    const vector<string>& propertyNames,
    const vector<string>& propertyCodes,
    const vector<string>& propertyStatuses
) const {
    cout << "Kamu mendarat di petak Festival!\n\n";

    size_t count = propertyNames.size();
    count = min(count, propertyCodes.size());
    count = min(count, propertyStatuses.size());

    if (count == 0) {
        cout << playerName << " belum memiliki properti yang dapat diberi efek festival.\n";
        return;
    }

    cout << "Daftar properti milikmu:\n";
    for (size_t i = 0; i < count; i++) {
        cout << (i + 1) << ". " << propertyCodes[i] << " (" << propertyNames[i] << ")";
        if (!propertyStatuses[i].empty()) {
            cout << " [" << propertyStatuses[i] << "]";
        }
        cout << '\n';
    }
    cout << "0. Batal\n";
}

int UIManager::readFestivalPropertyChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih properti festival (0 untuk batal): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printFestivalActivated(
    const string& tileName,
    const string& tileCode,
    int oldRent,
    int newRent,
    int duration
) const {
    cout << "Efek festival aktif!\n\n";
    cout << "Properti     : " << tileName << " (" << tileCode << ")\n";
    cout << "Sewa awal    : " << formatMoney(oldRent) << '\n';
    cout << "Sewa sekarang: " << formatMoney(newRent) << '\n';
    cout << "Durasi       : " << duration << " giliran\n";
}

void UIManager::printFestivalMaxed(
    const string& tileName,
    const string& tileCode,
    int duration
) const {
    cout << "Efek sudah maksimum untuk " << tileName << " (" << tileCode << ").\n";
    cout << "Harga sewa sudah digandakan tiga kali.\n";
    cout << "Durasi di-reset menjadi: " << duration << " giliran\n";
}

void UIManager::printAuctionState(
    const string& tileName,
    const string& tileCode,
    int currentBid,
    const string& highestBidderName,
    const string& currentPlayerName
) const {
    (void) tileName;
    (void) tileCode;
    cout << "Giliran: " << currentPlayerName << '\n';
    if (!highestBidderName.empty()) {
        cout << "Penawaran tertinggi: " << formatMoney(currentBid)
             << " (" << highestBidderName << ")\n";
    }
    cout << "Aksi (PASS / BID <jumlah>)\n> ";
}

string UIManager::readAuctionAction() const {
    string action;
    while (true) {
        getline(cin, action);
        if (cin.eof()) {
            return "PASS";
        }
        action = toUpperCopy(trim(action));
        if (action == "PASS" || action.rfind("BID ", 0) == 0) {
            return action;
        }
        cout << "Aksi harus PASS atau BID <jumlah>: ";
    }
}

void UIManager::printAuctionWinner(
    const string& tileName,
    const string& tileCode,
    const string& winnerName,
    int finalBid
) const {
    cout << "Lelang selesai!\n";
    cout << "Pemenang: " << winnerName << '\n';
    cout << "Harga akhir: " << formatMoney(finalBid) << "\n\n";
    cout << "Properti " << tileName << " (" << tileCode
         << ") kini dimiliki " << winnerName << ".\n";
}

void UIManager::printLiquidationState(
    const string& playerName,
    int playerMoney,
    int requiredAmount,
    int estimatedLiquidationValue
) const {
    cout << "=== Panel Likuidasi ===\n";
    cout << "Pemain: " << playerName << '\n';
    cout << "Uang kamu saat ini: " << formatMoney(playerMoney)
         << " | Kewajiban: " << formatMoney(requiredAmount) << '\n';
    cout << "Estimasi dana maksimum dari likuidasi: "
         << formatMoney(estimatedLiquidationValue) << '\n';
    cout << "Pilih aset yang ingin dilikuidasi dari daftar yang disediakan controller.\n";
}

int UIManager::readLiquidationChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih aksi (0 jika sudah cukup): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printForceDropState(
    const string& playerName,
    const vector<string>& cardNames
) const {
    (void) playerName;
    cout << "PERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal 3)!\n";
    cout << "Kamu diwajibkan membuang 1 kartu.\n\n";
    cout << "Daftar Kartu Kemampuan Anda:\n";
    for (size_t i = 0; i < cardNames.size(); i++) {
        cout << (i + 1) << ". " << cardNames[i] << '\n';
    }
}

int UIManager::readForceDropChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih nomor kartu yang ingin dibuang (1-" + to_string(maxIndex) + "): ",
        1,
        maxIndex,
        true
    );
}

void UIManager::printAbilityCardOptions(
    const vector<string>& cardNames,
    const vector<string>& cardDescriptions
) const {
    cout << "Daftar Kartu Kemampuan Spesial Anda:\n";
    const size_t count = min(cardNames.size(), cardDescriptions.size());
    for (size_t i = 0; i < count; i++) {
        cout << (i + 1) << ". " << cardNames[i];
        if (!cardDescriptions[i].empty()) {
            cout << " - " << cardDescriptions[i];
        }
        cout << '\n';
    }
    cout << "0. Batal\n";
}

int UIManager::readAbilityCardChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih kartu yang ingin digunakan (0-" + to_string(maxIndex) + "): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printBuildOptions(int playerMoney, const vector<string>& eligibleGroups) const {
    if (eligibleGroups.empty()) {
        cout << "Tidak ada color group yang memenuhi syarat untuk dibangun.\n";
        cout << "Kamu harus memiliki seluruh petak dalam satu color group terlebih dahulu.\n";
        return;
    }

    cout << "=== Color Group yang Memenuhi Syarat ===\n";
    for (size_t i = 0; i < eligibleGroups.size(); i++) {
        cout << (i + 1) << ". [" << colorName(eligibleGroups[i]) << "]\n";
    }
    cout << "\nUang kamu saat ini : " << formatMoney(playerMoney) << '\n';
}

int UIManager::readBuildGroupChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih nomor color group (0 untuk batal): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printBuildableTiles(
    const string& colorGroup,
    const vector<string>& tileNames,
    const vector<string>& tileCodes,
    const vector<string>& buildingStatuses,
    const vector<string>& buildStatuses
) const {
    cout << "Color group [" << colorName(colorGroup) << "]:\n";

    size_t count = tileNames.size();
    count = min(count, tileCodes.size());
    count = min(count, buildingStatuses.size());
    count = min(count, buildStatuses.size());

    if (count == 0) {
        cout << "Tidak ada petak yang dapat dibangun saat ini.\n";
        return;
    }

    for (size_t i = 0; i < count; i++) {
        cout << (i + 1) << ". " << left << setw(28)
             << (tileNames[i] + " (" + tileCodes[i] + ")")
             << ": " << setw(14) << truncateToWidth(buildingStatuses[i], 14)
             << " <- " << buildStatuses[i] << '\n';
    }
    cout << right;
}

int UIManager::readBuildTileChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih petak (0 untuk batal): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printRedeemOptions(
    int playerMoney,
    const vector<string>& propertyGroups,
    const vector<string>& propertyNames,
    const vector<string>& propertyCodes,
    const vector<string>& valueLabels,
    const vector<int>& values,
    const vector<string>& propertyStatuses
) const {
    cout << "=== Properti yang Sedang Digadaikan ===\n";

    const size_t count = propertyOptionCount(
        propertyGroups,
        propertyNames,
        propertyCodes,
        valueLabels,
        values,
        propertyStatuses
    );

    if (count == 0) {
        cout << "Tidak ada properti yang sedang digadaikan.\n";
        return;
    }

    for (size_t i = 0; i < count; i++) {
        printShortPropertyLine(
            static_cast<int>(i) + 1,
            propertyGroups[i],
            propertyNames[i],
            propertyCodes[i],
            valueLabels[i],
            values[i],
            propertyStatuses[i]
        );
    }
    cout << "\nUang kamu saat ini: " << formatMoney(playerMoney) << '\n';
}

int UIManager::readRedeemChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih nomor properti (0 untuk batal): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printMortgageOptions(
    int playerMoney,
    const vector<string>& propertyGroups,
    const vector<string>& propertyNames,
    const vector<string>& propertyCodes,
    const vector<string>& valueLabels,
    const vector<int>& values,
    const vector<string>& propertyStatuses
) const {
    cout << "=== Properti yang Dapat Digadaikan ===\n";

    const size_t count = propertyOptionCount(
        propertyGroups,
        propertyNames,
        propertyCodes,
        valueLabels,
        values,
        propertyStatuses
    );

    if (count == 0) {
        cout << "Tidak ada properti yang dapat digadaikan saat ini.\n";
        return;
    }

    for (size_t i = 0; i < count; i++) {
        printShortPropertyLine(
            static_cast<int>(i) + 1,
            propertyGroups[i],
            propertyNames[i],
            propertyCodes[i],
            valueLabels[i],
            values[i],
            propertyStatuses[i]
        );
    }
    cout << "\nUang kamu saat ini: " << formatMoney(playerMoney) << '\n';
}

int UIManager::readMortgageChoice(int maxIndex) const {
    return readIntInRange(
        "Pilih nomor properti (0 untuk batal): ",
        0,
        maxIndex,
        true
    );
}

void UIManager::printJailOptions(
    const string& playerName,
    int playerMoney,
    int jailFine,
    int failedRolls
) const {
    cout << "=== Giliran Penjara ===\n";
    cout << "Pemain       : " << playerName << '\n';
    cout << "Uang         : " << formatMoney(playerMoney) << '\n';
    cout << "Denda keluar : " << formatMoney(jailFine) << '\n';
    cout << "Gagal double : " << failedRolls << " kali\n\n";
    cout << "1. Bayar denda\n";
    cout << "2. Coba lempar double\n";
    cout << "0. Batal\n";
}

int UIManager::readJailChoice() const {
    return readIntInRange("Pilihan (0-2): ", 0, 2, true);
}

string UIManager::readFilename() const {
    string filename;

    while (true) {
        cout << "Masukkan nama file: ";
        getline(cin, filename);
        if (cin.eof()) {
            return "";
        }
        filename = trim(filename);

        if (!filename.empty()) {
            return filename;
        }

        cout << "Nama file tidak boleh kosong.\n";
    }
}

bool UIManager::confirmOverwrite(const string& filename) const {
    cout << "File \"" << filename << "\" sudah ada.\n";
    cout << "Timpa file lama? (y/n): ";
    return readYesNo();
}

void UIManager::printGameOver(
    const vector<string>& winnerNames,
    const vector<int>& winnerMoney,
    const vector<int>& winnerPropertyCounts,
    const vector<int>& winnerCardCounts,
    bool byMaxTurn
) const {
    cout << "=================================\n";
    cout << "        PERMAINAN SELESAI\n";
    cout << "=================================\n";
    if (byMaxTurn) {
        cout << "Batas giliran tercapai.\n\n";
    } else {
        cout << "Semua pemain kecuali pemenang telah bangkrut.\n\n";
    }

    size_t count = winnerNames.size();
    count = min(count, winnerMoney.size());
    count = min(count, winnerPropertyCounts.size());
    count = min(count, winnerCardCounts.size());

    if (count == 0) {
        cout << "Tidak ada pemenang.\n";
        return;
    }

    cout << "Pemenang:\n";
    for (size_t i = 0; i < count; i++) {
        cout << "- " << winnerNames[i]
             << " | Uang: " << formatMoney(winnerMoney[i])
             << " | Properti: " << winnerPropertyCounts[i]
             << " | Kartu: " << winnerCardCounts[i] << '\n';
    }
}
