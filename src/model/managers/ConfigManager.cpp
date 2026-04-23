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

    if (
        rows.size() >= 2 &&
        hasHeaderFields(rows[0], expectedFields) &&
        rows[1].size() >= rows[0].size()
    ) {
        const map<string, size_t> headerIndex = buildHeaderIndex(rows[0]);
        for (const string& field : expectedFields) {
            const size_t valueIndex = headerIndex.at(field);
            if (valueIndex >= rows[1].size()) {
                throw FileException(
                    "Nilai untuk field '" + field +
                    "' tidak ditemukan pada file " + filename
                );
            }
            values[field] = parseInt(rows[1][valueIndex], filename, field);
        }
        return values;
    }

    for (const vector<string>& row : rows) {
        if (row.size() < 2) {
            continue;
        }

        const string key = toUpperCopy(row[0]);
        if (find(expectedFields.begin(), expectedFields.end(), key) == expectedFields.end()) {
            continue;
        }

        values[key] = parseInt(row[1], filename, key);
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
    int purchasePrice,
    int mortgageValue,
    const string& colorGroup,
    int houseBuildCost,
    int hotelBuildCost,
    const vector<int>& rentLevels
)
    : code(code),
      name(name),
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

    size_t idIndex = 0;
    size_t codeIndex = 1;
    size_t nameIndex = 2;
    size_t colorIndex = 4;
    size_t purchasePriceIndex = 5;
    size_t mortgageValueIndex = 6;
    size_t houseBuildCostIndex = 7;
    size_t hotelBuildCostIndex = 8;
    size_t rentStartIndex = 9;
    size_t dataStartIndex = 0;

    if (hasHeaderFields(rows[0], {
        "ID", "KODE", "NAMA", "WARNA", "HARGA_LAHAN",
        "NILAI_GADAI", "UPG_RUMAH", "UPG_HT"
    })) {
        const map<string, size_t> headerIndex = buildHeaderIndex(rows[0]);
        idIndex = headerIndex.at("ID");
        codeIndex = headerIndex.at("KODE");
        nameIndex = headerIndex.at("NAMA");
        colorIndex = headerIndex.at("WARNA");
        purchasePriceIndex = headerIndex.at("HARGA_LAHAN");
        mortgageValueIndex = headerIndex.at("NILAI_GADAI");
        houseBuildCostIndex = headerIndex.at("UPG_RUMAH");
        hotelBuildCostIndex = headerIndex.at("UPG_HT");
        rentStartIndex = hotelBuildCostIndex + 1;
        dataStartIndex = 1;
    }

    const size_t requiredIndex = max({
        idIndex,
        codeIndex,
        nameIndex,
        colorIndex,
        purchasePriceIndex,
        mortgageValueIndex,
        houseBuildCostIndex,
        hotelBuildCostIndex
    });

    size_t parsedRowCount = 0;
    for (size_t rowIndex = dataStartIndex; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        if (row.size() <= requiredIndex) {
            continue;
        }

        const string& idToken = row[idIndex];
        const string& purchasePriceToken = row[purchasePriceIndex];
        const string& mortgageToken = row[mortgageValueIndex];
        const string& houseCostToken = row[houseBuildCostIndex];
        const string& hotelCostToken = row[hotelBuildCostIndex];

        if (
            !isIntegerToken(idToken) ||
            !isIntegerToken(purchasePriceToken) ||
            !isIntegerToken(mortgageToken) ||
            !isIntegerToken(houseCostToken) ||
            !isIntegerToken(hotelCostToken)
        ) {
            continue;
        }

        const int id = parseInt(idToken, filename, "ID");
        const string code = row[codeIndex];

        if (code.empty()) {
            throw FileException("Kode properti kosong pada file " + filename);
        }
        if (propertyConfigs.find(code) != propertyConfigs.end()) {
            throw FileException("Kode properti duplikat pada file " + filename + ": " + code);
        }
        if (propertyCodeById.find(id) != propertyCodeById.end()) {
            throw FileException("ID properti duplikat pada file " + filename + ": " + idToken);
        }

        vector<int> rentLevels;
        for (size_t i = rentStartIndex; i < row.size(); i++) {
            if (!isIntegerToken(row[i])) {
                continue;
            }
            rentLevels.push_back(parseInt(row[i], filename, "RENT_LEVEL"));
        }

        propertyConfigs[code] = PropertyConfig(
            code,
            row[nameIndex],
            parseInt(purchasePriceToken, filename, "HARGA_LAHAN"),
            parseInt(mortgageToken, filename, "NILAI_GADAI"),
            row[colorIndex],
            parseInt(houseCostToken, filename, "UPG_RUMAH"),
            parseInt(hotelCostToken, filename, "UPG_HT"),
            rentLevels
        );

        propertyCodeById[id] = code;
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data properti valid pada file " + filename);
    }
}

void ConfigManager::loadRailroadConfig(const string& filename) {
    const vector<vector<string>> rows = readTokenRows(filename);

    size_t countIndex = 0;
    size_t rentIndex = 1;
    size_t dataStartIndex = 0;

    if (hasHeaderFields(rows[0], {"JUMLAH_RAILROAD", "BIAYA_SEWA"})) {
        const map<string, size_t> headerIndex = buildHeaderIndex(rows[0]);
        countIndex = headerIndex.at("JUMLAH_RAILROAD");
        rentIndex = headerIndex.at("BIAYA_SEWA");
        dataStartIndex = 1;
    }

    const size_t requiredIndex = max(countIndex, rentIndex);
    size_t parsedRowCount = 0;
    for (size_t rowIndex = dataStartIndex; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        if (row.size() <= requiredIndex) {
            continue;
        }
        if (!isIntegerToken(row[countIndex]) || !isIntegerToken(row[rentIndex])) {
            continue;
        }

        const int railroadCount = parseInt(row[countIndex], filename, "JUMLAH_RAILROAD");
        if (railroadRentTable.find(railroadCount) != railroadRentTable.end()) {
            throw FileException(
                "Jumlah railroad duplikat pada file " + filename + ": " + row[countIndex]
            );
        }

        railroadRentTable[railroadCount] = parseInt(row[rentIndex], filename, "BIAYA_SEWA");
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data railroad valid pada file " + filename);
    }
}

void ConfigManager::loadUtilityConfig(const string& filename) {
    const vector<vector<string>> rows = readTokenRows(filename);

    size_t countIndex = 0;
    size_t multiplierIndex = 1;
    size_t dataStartIndex = 0;

    if (hasHeaderFields(rows[0], {"JUMLAH_UTILITY", "FAKTOR_PENGALI"})) {
        const map<string, size_t> headerIndex = buildHeaderIndex(rows[0]);
        countIndex = headerIndex.at("JUMLAH_UTILITY");
        multiplierIndex = headerIndex.at("FAKTOR_PENGALI");
        dataStartIndex = 1;
    }

    const size_t requiredIndex = max(countIndex, multiplierIndex);
    size_t parsedRowCount = 0;
    for (size_t rowIndex = dataStartIndex; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        if (row.size() <= requiredIndex) {
            continue;
        }
        if (!isIntegerToken(row[countIndex]) || !isIntegerToken(row[multiplierIndex])) {
            continue;
        }

        const int utilityCount = parseInt(row[countIndex], filename, "JUMLAH_UTILITY");
        if (utilityMultiplierTable.find(utilityCount) != utilityMultiplierTable.end()) {
            throw FileException(
                "Jumlah utility duplikat pada file " + filename + ": " + row[countIndex]
            );
        }

        utilityMultiplierTable[utilityCount] = parseInt(
            row[multiplierIndex],
            filename,
            "FAKTOR_PENGALI"
        );
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data utility valid pada file " + filename);
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

    size_t idIndex = 0;
    size_t codeIndex = 1;
    size_t nameIndex = 2;
    size_t typeIndex = 3;
    size_t colorIndex = 4;
    size_t dataStartIndex = 0;

    if (hasHeaderFields(rows[0], {"ID", "KODE", "NAMA", "JENIS_PETAK", "WARNA"})) {
        const map<string, size_t> headerIndex = buildHeaderIndex(rows[0]);
        idIndex = headerIndex.at("ID");
        codeIndex = headerIndex.at("KODE");
        nameIndex = headerIndex.at("NAMA");
        typeIndex = headerIndex.at("JENIS_PETAK");
        colorIndex = headerIndex.at("WARNA");
        dataStartIndex = 1;
    }

    const size_t requiredIndex = max({
        idIndex,
        codeIndex,
        nameIndex,
        typeIndex,
        colorIndex
    });

    size_t parsedRowCount = 0;
    for (size_t rowIndex = dataStartIndex; rowIndex < rows.size(); rowIndex++) {
        const vector<string>& row = rows[rowIndex];
        if (row.size() <= requiredIndex) {
            continue;
        }
        if (!isIntegerToken(row[idIndex])) {
            continue;
        }

        const int id = parseInt(row[idIndex], filename, "ID");
        const string& code = row[codeIndex];
        const string& name = row[nameIndex];
        const string& tileType = row[typeIndex];
        const string& color = row[colorIndex];

        if (code.empty()) {
            throw FileException("Kode petak aksi kosong pada file " + filename);
        }
        if (name.empty()) {
            throw FileException("Nama petak aksi kosong pada file " + filename);
        }
        if (tileType.empty()) {
            throw FileException("Jenis petak aksi kosong pada file " + filename);
        }
        if (actionTileConfigs.find(id) != actionTileConfigs.end()) {
            throw FileException("ID petak aksi duplikat pada file " + filename + ": " + row[idIndex]);
        }

        actionTileConfigs[id] = ActionTileConfig(id, code, name, tileType, color);
        parsedRowCount++;
    }

    if (parsedRowCount == 0) {
        throw FileException("Tidak ada data petak aksi valid pada file " + filename);
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
        {"MAX_TURN", "MAX_TURNS", "SALDO_AWAL", "INITIAL_BALANCE"}
    );

    maxTurn = getValueByAliases(values, {"MAX_TURN", "MAX_TURNS"}, filename);
    initialBalance = getValueByAliases(values, {"SALDO_AWAL", "INITIAL_BALANCE"}, filename);
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
