#include "model/managers/ConfigManager.hpp"

using namespace std;

ConfigManager::PropertyConfig::PropertyConfig() {}

ConfigManager::PropertyConfig::PropertyConfig(
    const string& code,
    const string& name,
    int purchasePrice,
    int mortgageValue,
    const string& colorGroup,
    int houseBuildCost,
    int hotelBuildCost
) {}

ConfigManager::PropertyConfig::PropertyConfig(const ConfigManager::PropertyConfig& other) {}

ConfigManager::PropertyConfig::~PropertyConfig() {}

ConfigManager::PropertyConfig& ConfigManager::PropertyConfig::operator=(
    const ConfigManager::PropertyConfig& other
) {
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

ConfigManager::ConfigManager() {}

ConfigManager::ConfigManager(
    const map<string, ConfigManager::PropertyConfig>& propertyConfigs,
    const map<int, int>& railroadRentTable,
    const map<int, int>& utilityMultiplierTable,
    int pphFlat,
    int pphPercent,
    int pbmFlat,
    int goSalary,
    int jailFine,
    int maxTurn,
    int initialBalance
) {}

ConfigManager::ConfigManager(const ConfigManager& other) {}

ConfigManager::~ConfigManager() {}

ConfigManager& ConfigManager::operator=(const ConfigManager& other) {
    return *this;
}

void ConfigManager::loadAllConfigs() {}

void ConfigManager::loadPropertyConfig(const string& filename) {}

void ConfigManager::loadRailroadConfig(const string& filename) {}

void ConfigManager::loadUtilityConfig(const string& filename) {}

void ConfigManager::loadTaxConfig(const string& filename) {}

void ConfigManager::loadSpecialConfig(const string& filename) {}

void ConfigManager::loadMiscConfig(const string& filename) {}

const map<string, ConfigManager::PropertyConfig>& ConfigManager::getPropertyConfigs() const {
    return propertyConfigs;
}

const ConfigManager::PropertyConfig& ConfigManager::getPropertyConfig(const string& code) const {
    return propertyConfigs.at(code);
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