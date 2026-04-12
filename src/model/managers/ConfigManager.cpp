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