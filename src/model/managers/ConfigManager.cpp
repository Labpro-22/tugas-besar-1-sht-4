#include "model/managers/ConfigManager.hpp"
#include "model/NimonException.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

string ConfigManager::trim(const string& value) const {
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

string ConfigManager::toUpperCopy(string value) const {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return value;
}

string ConfigManager::stripInlineComment(const string& line) const {
    const size_t commentPos = line.find('#');
    if (commentPos == string::npos) {
        return line;
    }
    return line.substr(0, commentPos);
}

vector<string> ConfigManager::splitWhitespace(const string& line) const {
    vector<string> tokens;
    istringstream stream(line);
    string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

vector<vector<string>> ConfigManager::readTokenRows(const string& filename) const {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw FileException("Gagal membuka file konfigurasi: " + filename);
    }

    vector<vector<string>> rows;
    string line;
    while (getline(file, line)) {
        const string cleanedLine = trim(stripInlineComment(line));
        if (cleanedLine.empty()) {
            continue;
        }

        vector<string> tokens = splitWhitespace(cleanedLine);
        if (!tokens.empty()) {
            rows.push_back(tokens);
        }
    }

    if (rows.empty()) {
        throw FileException("File konfigurasi kosong: " + filename);
    }

    return rows;
}

int ConfigManager::parseInt(const string& token, const string& filename, const string& fieldName) const {
    try {
        size_t parsedLength = 0;
        const int value = stoi(token, &parsedLength);
        if (parsedLength != token.size()) {
            throw invalid_argument("invalid integer");
        }
        return value;
    } catch (const exception&) {
        throw FileException(
            "Nilai integer tidak valid untuk field '" + fieldName +
            "' pada file " + filename + ": " + token
        );
    }
}

bool ConfigManager::isIntegerToken(const string& token) const {
    if (token.empty()) {
        return false;
    }

    size_t index = 0;
    if (token[0] == '+' || token[0] == '-') {
        if (token.size() == 1) {
            return false;
        }
        index = 1;
    }

    for (; index < token.size(); index++) {
        if (!isdigit(static_cast<unsigned char>(token[index]))) {
            return false;
        }
    }

    return true;
}

string ConfigManager::joinFields(const vector<string>& fields) const {
    string result;
    for (size_t index = 0; index < fields.size(); index++) {
        if (index > 0) {
            result += " ";
        }
        result += fields[index];
    }
    return result;
}

void ConfigManager::requireExactHeader(
    const vector<string>& row,
    const vector<string>& expectedFields,
    const string& filename
) const {
    if (row.size() != expectedFields.size()) {
        throw FileException(
            "Header file " + filename + " tidak sesuai. Format wajib: " +
            joinFields(expectedFields)
        );
    }

    for (size_t index = 0; index < expectedFields.size(); index++) {
        if (toUpperCopy(row[index]) != expectedFields[index]) {
            throw FileException(
                "Header file " + filename + " tidak sesuai. Format wajib: " +
                joinFields(expectedFields)
            );
        }
    }
}

void ConfigManager::requireRowSize(
    const vector<string>& row,
    size_t expectedSize,
    const string& filename,
    size_t rowNumber
) const {
    if (row.size() != expectedSize) {
        throw FileException(
            "Jumlah kolom tidak valid pada file " + filename +
            " baris " + to_string(rowNumber) + ". Diharapkan " +
            to_string(expectedSize) + " kolom, ditemukan " +
            to_string(row.size()) + "."
        );
    }
}

map<string, size_t> ConfigManager::buildHeaderIndex(const vector<string>& headerRow) const {
    map<string, size_t> indexByHeader;
    for (size_t index = 0; index < headerRow.size(); index++) {
        indexByHeader[toUpperCopy(headerRow[index])] = index;
    }
    return indexByHeader;
}

bool ConfigManager::hasHeaderFields(const vector<string>& row, const vector<string>& requiredFields) const {
    const map<string, size_t> headerIndex = buildHeaderIndex(row);
    return all_of(requiredFields.begin(), requiredFields.end(), [&](const string& field) {
        return headerIndex.find(field) != headerIndex.end();
    });
}

int ConfigManager::getValueByAliases(
    const map<string, int>& values,
    const vector<string>& aliases,
    const string& filename
) const {
    for (const string& alias : aliases) {
        const auto it = values.find(toUpperCopy(alias));
        if (it != values.end()) {
            return it->second;
        }
    }

    string joinedAliases;
    for (size_t index = 0; index < aliases.size(); index++) {
        if (index > 0) {
            joinedAliases += ", ";
        }
        joinedAliases += aliases[index];
    }

    throw FileException(
        "Field konfigurasi wajib tidak ditemukan pada file " +
        filename + ": " + joinedAliases
    );
}

map<string, int> ConfigManager::parseScalarConfig(
    const string& filename,
    const vector<string>& expectedFields
) const {
    const vector<vector<string>> rows = readTokenRows(filename);
    map<string, int> values;

    if (rows.size() != 2) {
        throw FileException(
            "Format file " + filename +
            " tidak valid. Diharapkan 1 baris header dan 1 baris nilai."
        );
    }

    requireExactHeader(rows[0], expectedFields, filename);
    requireRowSize(rows[1], expectedFields.size(), filename, 2);

    for (size_t index = 0; index < expectedFields.size(); index++) {
        values[expectedFields[index]] = parseInt(rows[1][index], filename, expectedFields[index]);
    }

    return values;
}

ConfigManager::ActionTileConfig::ActionTileConfig()
    : id(0)
{}

ConfigManager::ActionTileConfig::ActionTileConfig(
    int id,
    const string& code,
    const string& name,
    const string& tileType,
    const string& color
)
    : id(id),
      code(code),
      name(name),
      tileType(tileType),
      color(color)
{}

ConfigManager::ActionTileConfig::ActionTileConfig(
    const ConfigManager::ActionTileConfig& other
)
    : id(other.id),
      code(other.code),
      name(other.name),
      tileType(other.tileType),
      color(other.color)
{}

ConfigManager::ActionTileConfig::~ActionTileConfig() {}

ConfigManager::ActionTileConfig& ConfigManager::ActionTileConfig::operator=(
    const ConfigManager::ActionTileConfig& other
) {
    if (this != &other) {
        id = other.id;
        code = other.code;
        name = other.name;
        tileType = other.tileType;
        color = other.color;
    }
    return *this;
}

int ConfigManager::ActionTileConfig::getId() const {
    return id;
}

const string& ConfigManager::ActionTileConfig::getCode() const {
    return code;
}

const string& ConfigManager::ActionTileConfig::getName() const {
    return name;
}

const string& ConfigManager::ActionTileConfig::getTileType() const {
    return tileType;
}

const string& ConfigManager::ActionTileConfig::getColor() const {
    return color;
}

ConfigManager::PropertyConfig::PropertyConfig()
    : purchasePrice(0),
      mortgageValue(0),
      houseBuildCost(0),
      hotelBuildCost(0),
      rentLevels()
{}

ConfigManager::PropertyConfig::PropertyConfig(
    const string& code,
    const string& name,
    const string& propertyType,
    int purchasePrice,
    int mortgageValue,
    const string& colorGroup,
    int houseBuildCost,
    int hotelBuildCost,
    const vector<int>& rentLevels
)
    : code(code),
      name(name),
      propertyType(propertyType),
      purchasePrice(purchasePrice),
      mortgageValue(mortgageValue),
      colorGroup(colorGroup),
      houseBuildCost(houseBuildCost),
      hotelBuildCost(hotelBuildCost),
      rentLevels(rentLevels)
{}

ConfigManager::PropertyConfig::PropertyConfig(const ConfigManager::PropertyConfig& other)
    : code(other.code),
      name(other.name),
      propertyType(other.propertyType),
      purchasePrice(other.purchasePrice),
      mortgageValue(other.mortgageValue),
      colorGroup(other.colorGroup),
      houseBuildCost(other.houseBuildCost),
      hotelBuildCost(other.hotelBuildCost),
      rentLevels(other.rentLevels)
{}

ConfigManager::PropertyConfig::~PropertyConfig() {}

ConfigManager::PropertyConfig& ConfigManager::PropertyConfig::operator=(
    const ConfigManager::PropertyConfig& other
) {
    if (this != &other) {
        code = other.code;
        name = other.name;
        propertyType = other.propertyType;
        purchasePrice = other.purchasePrice;
        mortgageValue = other.mortgageValue;
        colorGroup = other.colorGroup;
        houseBuildCost = other.houseBuildCost;
        hotelBuildCost = other.hotelBuildCost;
        rentLevels = other.rentLevels;
    }
    return *this;
}

const string& ConfigManager::PropertyConfig::getCode() const {
    return code;
}

const string& ConfigManager::PropertyConfig::getName() const {
    return name;
}

const string& ConfigManager::PropertyConfig::getPropertyType() const {
    return propertyType;
}

int ConfigManager::PropertyConfig::getPurchasePrice() const {
    return purchasePrice;
}

int ConfigManager::PropertyConfig::getMortgageValue() const {
    return mortgageValue;
}

const string& ConfigManager::PropertyConfig::getColorGroup() const {
    return colorGroup;
}

int ConfigManager::PropertyConfig::getHouseBuildCost() const {
    return houseBuildCost;
}

int ConfigManager::PropertyConfig::getHotelBuildCost() const {
    return hotelBuildCost;
}

const vector<int>& ConfigManager::PropertyConfig::getRentLevels() const {
    return rentLevels;
}

ConfigManager::ConfigManager()
    : pphFlat(0),
      pphPercent(0),
      pbmFlat(0),
      goSalary(0),
      jailFine(0),
      maxTurn(0),
      initialBalance(0)
{}

ConfigManager::ConfigManager(
    const map<string, ConfigManager::PropertyConfig>& propertyConfigs,
    const map<int, string>& propertyCodeById,
    const map<int, int>& railroadRentTable,
    const map<int, int>& utilityMultiplierTable,
    int pphFlat,
    int pphPercent,
    int pbmFlat,
    int goSalary,
    int jailFine,
    int maxTurn,
    int initialBalance
)
    : propertyConfigs(propertyConfigs),
      propertyCodeById(propertyCodeById),
      railroadRentTable(railroadRentTable),
      utilityMultiplierTable(utilityMultiplierTable),
      pphFlat(pphFlat),
      pphPercent(pphPercent),
      pbmFlat(pbmFlat),
      goSalary(goSalary),
      jailFine(jailFine),
      maxTurn(maxTurn),
      initialBalance(initialBalance)
{}

ConfigManager::ConfigManager(const ConfigManager& other)
    : actionTileConfigs(other.actionTileConfigs),
      propertyConfigs(other.propertyConfigs),
      propertyCodeById(other.propertyCodeById),
      railroadRentTable(other.railroadRentTable),
      utilityMultiplierTable(other.utilityMultiplierTable),
      pphFlat(other.pphFlat),
      pphPercent(other.pphPercent),
      pbmFlat(other.pbmFlat),
      goSalary(other.goSalary),
      jailFine(other.jailFine),
      maxTurn(other.maxTurn),
      initialBalance(other.initialBalance)
{}

ConfigManager::~ConfigManager() {}

ConfigManager& ConfigManager::operator=(const ConfigManager& other) {
    if (this != &other) {
        actionTileConfigs = other.actionTileConfigs;
        propertyConfigs = other.propertyConfigs;
        propertyCodeById = other.propertyCodeById;
        railroadRentTable = other.railroadRentTable;
        utilityMultiplierTable = other.utilityMultiplierTable;
        pphFlat = other.pphFlat;
        pphPercent = other.pphPercent;
        pbmFlat = other.pbmFlat;
        goSalary = other.goSalary;
        jailFine = other.jailFine;
        maxTurn = other.maxTurn;
        initialBalance = other.initialBalance;
    }
    return *this;
}

void ConfigManager::loadAllConfigs() {
    actionTileConfigs.clear();
    propertyConfigs.clear();
    propertyCodeById.clear();
    railroadRentTable.clear();
    utilityMultiplierTable.clear();
    pphFlat = 0;
    pphPercent = 0;
    pbmFlat = 0;
    goSalary = 0;
    jailFine = 0;
    maxTurn = 0;
    initialBalance = 0;

    loadPropertyConfig(PROPERTY_CONFIG_PATH);
    loadRailroadConfig(RAILROAD_CONFIG_PATH);
    loadUtilityConfig(UTILITY_CONFIG_PATH);
    loadTaxConfig(TAX_CONFIG_PATH);
    loadActionTileConfig(ACTION_CONFIG_PATH);
    loadSpecialConfig(SPECIAL_CONFIG_PATH);
    loadMiscConfig(MISC_CONFIG_PATH);
}

void ConfigManager::loadPropertyConfig(const string& filename) {
    const vector<vector<string>> rows = readTokenRows(filename);
    const vector<string> expectedHeader = {
        "ID", "KODE", "NAMA", "JENIS", "WARNA", "HARGA_LAHAN",
        "NILAI_GADAI", "UPG_RUMAH", "UPG_HT",
        "RENT_L0", "RENT_L1", "RENT_L2", "RENT_L3", "RENT_L4", "RENT_L5"
    };
    const vector<int> requiredPropertyIds = {
        2, 4, 6, 7, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20,
        22, 24, 25, 26, 27, 28, 29, 30, 32, 33, 35, 36, 38, 40
    };

    requireExactHeader(rows[0], expectedHeader, filename);

    size_t parsedRowCount = 0;
    for (size_t rowIndex = 1; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        const size_t rowNumber = rowIndex + 1;
        if (row.size() < 4) {
            requireRowSize(row, 4, filename, rowNumber);
        }

        const string propertyType = toUpperCopy(row[3]);
        const bool isStreet = propertyType == "STREET";
        const bool isRailroad = propertyType == "RAILROAD";
        const bool isUtility = propertyType == "UTILITY";
        if (!isStreet && !isRailroad && !isUtility) {
            throw FileException(
                "Jenis properti tidak valid pada file " + filename +
                " baris " + to_string(rowNumber) + ": " + row[3]
            );
        }

        requireRowSize(row, isStreet ? 15 : 7, filename, rowNumber);

        const int id = parseInt(row[0], filename, "ID");
        const string code = row[1];

        if (code.empty()) {
            throw FileException("Kode properti kosong pada file " + filename);
        }
        if (row[2].empty()) {
            throw FileException("Nama properti kosong pada file " + filename);
        }
        if (row[4].empty()) {
            throw FileException("Warna properti kosong pada file " + filename);
        }
        if (propertyConfigs.find(code) != propertyConfigs.end()) {
            throw FileException("Kode properti duplikat pada file " + filename + ": " + code);
        }
        if (propertyCodeById.find(id) != propertyCodeById.end()) {
            throw FileException("ID properti duplikat pada file " + filename + ": " + row[0]);
        }

        vector<int> rentLevels;
        int houseBuildCost = 0;
        int hotelBuildCost = 0;
        if (isStreet) {
            houseBuildCost = parseInt(row[7], filename, "UPG_RUMAH");
            hotelBuildCost = parseInt(row[8], filename, "UPG_HT");
            for (size_t i = 9; i < row.size(); i++) {
                rentLevels.push_back(parseInt(row[i], filename, "RENT_L" + to_string(i - 9)));
            }
        }

        propertyConfigs[code] = PropertyConfig(
            code,
            row[2],
            propertyType,
            parseInt(row[5], filename, "HARGA_LAHAN"),
            parseInt(row[6], filename, "NILAI_GADAI"),
            row[4],
            houseBuildCost,
            hotelBuildCost,
            rentLevels
        );

        propertyCodeById[id] = code;
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data properti valid pada file " + filename);
    }

    if (propertyCodeById.size() != requiredPropertyIds.size()) {
        throw FileException(
            "Jumlah data properti pada file " + filename +
            " harus " + to_string(requiredPropertyIds.size()) + " baris."
        );
    }

    for (int requiredId : requiredPropertyIds) {
        if (propertyCodeById.find(requiredId) == propertyCodeById.end()) {
            throw FileException(
                "ID properti wajib tidak ditemukan pada file " +
                filename + ": " + to_string(requiredId)
            );
        }
    }
}

void ConfigManager::loadRailroadConfig(const string& filename) {
    const vector<vector<string>> rows = readTokenRows(filename);
    const vector<string> expectedHeader = {"JUMLAH_RAILROAD", "BIAYA_SEWA"};
    requireExactHeader(rows[0], expectedHeader, filename);

    size_t parsedRowCount = 0;
    for (size_t rowIndex = 1; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        requireRowSize(row, expectedHeader.size(), filename, rowIndex + 1);

        const int railroadCount = parseInt(row[0], filename, "JUMLAH_RAILROAD");
        if (railroadRentTable.find(railroadCount) != railroadRentTable.end()) {
            throw FileException(
                "Jumlah railroad duplikat pada file " + filename + ": " + row[0]
            );
        }

        railroadRentTable[railroadCount] = parseInt(row[1], filename, "BIAYA_SEWA");
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data railroad valid pada file " + filename);
    }

    if (railroadRentTable.size() != 4) {
        throw FileException("File " + filename + " harus memuat sewa untuk 1 sampai 4 railroad.");
    }

    for (int count = 1; count <= 4; count++) {
        if (railroadRentTable.find(count) == railroadRentTable.end()) {
            throw FileException(
                "Jumlah railroad wajib tidak ditemukan pada file " +
                filename + ": " + to_string(count)
            );
        }
    }
}

void ConfigManager::loadUtilityConfig(const string& filename) {
    const vector<vector<string>> rows = readTokenRows(filename);
    const vector<string> expectedHeader = {"JUMLAH_UTILITY", "FAKTOR_PENGALI"};
    requireExactHeader(rows[0], expectedHeader, filename);

    size_t parsedRowCount = 0;
    for (size_t rowIndex = 1; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        requireRowSize(row, expectedHeader.size(), filename, rowIndex + 1);

        const int utilityCount = parseInt(row[0], filename, "JUMLAH_UTILITY");
        if (utilityMultiplierTable.find(utilityCount) != utilityMultiplierTable.end()) {
            throw FileException(
                "Jumlah utility duplikat pada file " + filename + ": " + row[0]
            );
        }

        utilityMultiplierTable[utilityCount] = parseInt(
            row[1],
            filename,
            "FAKTOR_PENGALI"
        );
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data utility valid pada file " + filename);
    }

    if (utilityMultiplierTable.size() != 2) {
        throw FileException("File " + filename + " harus memuat faktor untuk 1 sampai 2 utility.");
    }

    for (int count = 1; count <= 2; count++) {
        if (utilityMultiplierTable.find(count) == utilityMultiplierTable.end()) {
            throw FileException(
                "Jumlah utility wajib tidak ditemukan pada file " +
                filename + ": " + to_string(count)
            );
        }
    }
}

void ConfigManager::loadTaxConfig(const string& filename) {
    const map<string, int> values = parseScalarConfig(
        filename,
        {"PPH_FLAT", "PPH_PERSENTASE", "PBM_FLAT"}
    );

    pphFlat = getValueByAliases(values, {"PPH_FLAT"}, filename);
    pphPercent = getValueByAliases(values, {"PPH_PERSENTASE", "PPH_PERCENTAGE"}, filename);
    pbmFlat = getValueByAliases(values, {"PBM_FLAT"}, filename);
}

void ConfigManager::loadActionTileConfig(const string& filename) {
    const vector<vector<string>> rows = readTokenRows(filename);
    const vector<string> expectedHeader = {"ID", "KODE", "NAMA", "JENIS_PETAK", "WARNA"};
    const vector<int> requiredActionIds = {1, 3, 5, 8, 11, 18, 21, 23, 31, 34, 37, 39};

    requireExactHeader(rows[0], expectedHeader, filename);

    size_t parsedRowCount = 0;
    for (size_t rowIndex = 1; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        requireRowSize(row, expectedHeader.size(), filename, rowIndex + 1);

        const int id = parseInt(row[0], filename, "ID");
        const string& code = row[1];
        const string& name = row[2];
        const string& tileType = row[3];
        const string& color = row[4];

        if (code.empty()) {
            throw FileException("Kode petak aksi kosong pada file " + filename);
        }
        if (name.empty()) {
            throw FileException("Nama petak aksi kosong pada file " + filename);
        }
        if (tileType.empty()) {
            throw FileException("Jenis petak aksi kosong pada file " + filename);
        }
        const string normalizedType = toUpperCopy(tileType);
        if (
            normalizedType != "SPESIAL" &&
            normalizedType != "KARTU" &&
            normalizedType != "PAJAK" &&
            normalizedType != "FESTIVAL"
        ) {
            throw FileException(
                "Jenis petak aksi tidak valid pada file " +
                filename + " baris " + to_string(rowIndex + 1) + ": " + tileType
            );
        }
        if (actionTileConfigs.find(id) != actionTileConfigs.end()) {
            throw FileException("ID petak aksi duplikat pada file " + filename + ": " + row[0]);
        }

        actionTileConfigs[id] = ActionTileConfig(id, code, name, tileType, color);
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data petak aksi valid pada file " + filename);
    }

    if (actionTileConfigs.size() != requiredActionIds.size()) {
        throw FileException(
            "Jumlah data petak aksi pada file " + filename +
            " harus " + to_string(requiredActionIds.size()) + " baris."
        );
    }

    for (int requiredId : requiredActionIds) {
        if (actionTileConfigs.find(requiredId) == actionTileConfigs.end()) {
            throw FileException(
                "ID petak aksi wajib tidak ditemukan pada file " +
                filename + ": " + to_string(requiredId)
            );
        }
    }
}

void ConfigManager::loadSpecialConfig(const string& filename) {
    const map<string, int> values = parseScalarConfig(
        filename,
        {"GO_SALARY", "JAIL_FINE"}
    );

    goSalary = getValueByAliases(values, {"GO_SALARY"}, filename);
    jailFine = getValueByAliases(values, {"JAIL_FINE"}, filename);
}

void ConfigManager::loadMiscConfig(const string& filename) {
    const map<string, int> values = parseScalarConfig(
        filename,
        {"MAX_TURN", "SALDO_AWAL"}
    );

    maxTurn = getValueByAliases(values, {"MAX_TURN"}, filename);
    initialBalance = getValueByAliases(values, {"SALDO_AWAL"}, filename);
}

const map<int, ConfigManager::ActionTileConfig>& ConfigManager::getActionTileConfigs() const {
    return actionTileConfigs;
}

const ConfigManager::ActionTileConfig& ConfigManager::getActionTileConfigById(int id) const {
    return actionTileConfigs.at(id);
}

bool ConfigManager::hasActionTileId(int id) const {
    return actionTileConfigs.find(id) != actionTileConfigs.end();
}

const map<string, ConfigManager::PropertyConfig>& ConfigManager::getPropertyConfigs() const {
    return propertyConfigs;
}

const ConfigManager::PropertyConfig& ConfigManager::getPropertyConfig(const string& code) const {
    return propertyConfigs.at(code);
}

const map<int, string>& ConfigManager::getPropertyCodeByIdMap() const {
    return propertyCodeById;
}

const string& ConfigManager::getPropertyCodeById(int id) const {
    return propertyCodeById.at(id);
}

const ConfigManager::PropertyConfig& ConfigManager::getPropertyConfigById(int id) const {
    return getPropertyConfig(getPropertyCodeById(id));
}

bool ConfigManager::hasPropertyId(int id) const {
    return propertyCodeById.find(id) != propertyCodeById.end();
}

const map<int, int>& ConfigManager::getRailroadRentTable() const {
    return railroadRentTable;
}

int ConfigManager::getRailroadRent(int railroadCount) const {
    return railroadRentTable.at(railroadCount);
}

const map<int, int>& ConfigManager::getUtilityMultiplierTable() const {
    return utilityMultiplierTable;
}

int ConfigManager::getUtilityMultiplier(int utilityCount) const {
    return utilityMultiplierTable.at(utilityCount);
}

int ConfigManager::getPphFlat() const {
    return pphFlat;
}

int ConfigManager::getPphPercent() const {
    return pphPercent;
}

int ConfigManager::getPbmFlat() const {
    return pbmFlat;
}

int ConfigManager::getGoSalary() const {
    return goSalary;
}

int ConfigManager::getJailFine() const {
    return jailFine;
}

int ConfigManager::getMaxTurn() const {
    return maxTurn;
}

int ConfigManager::getInitialBalance() const {
    return initialBalance;
}
