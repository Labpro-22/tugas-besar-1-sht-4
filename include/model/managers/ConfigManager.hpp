#pragma once

#include <map>
#include <string>

using namespace std;

class ConfigManager {
public:
    class PropertyConfig {
    private:
        string code;
        string name;
        int purchasePrice;
        int mortgageValue;
        string colorGroup;
        int houseBuildCost;
        int hotelBuildCost;

    public:
        PropertyConfig();
        PropertyConfig(
            const string& code,
            const string& name,
            int purchasePrice,
            int mortgageValue,
            const string& colorGroup,
            int houseBuildCost,
            int hotelBuildCost
        );
        PropertyConfig(const PropertyConfig& other);
        ~PropertyConfig();
        PropertyConfig& operator=(const PropertyConfig& other);
    };

private:
    map<string, PropertyConfig> propertyConfigs;
    map<int, int> railroadRentTable;
    map<int, int> utilityMultiplierTable;
    int pphFlat;
    int pphPercent;
    int pbmFlat;
    int goSalary;
    int jailFine;
    int maxTurn;
    int initialBalance;

public:
    ConfigManager();
    ConfigManager(
        const map<string, PropertyConfig>& propertyConfigs,
        const map<int, int>& railroadRentTable,
        const map<int, int>& utilityMultiplierTable,
        int pphFlat,
        int pphPercent,
        int pbmFlat,
        int goSalary,
        int jailFine,
        int maxTurn,
        int initialBalance
    );
    ConfigManager(const ConfigManager& other);
    ~ConfigManager();
    ConfigManager& operator=(const ConfigManager& other);

    void loadAllConfigs();
    void loadPropertyConfig(const string& filename);
    void loadRailroadConfig(const string& filename);
    void loadUtilityConfig(const string& filename);
    void loadTaxConfig(const string& filename);
    void loadSpecialConfig(const string& filename);
    void loadMiscConfig(const string& filename);
};