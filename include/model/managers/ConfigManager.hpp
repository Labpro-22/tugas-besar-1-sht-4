#pragma once

#include <map>
#include <string>
#include <vector>

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
        vector<int> rentLevels;

    public:
        PropertyConfig();
        PropertyConfig(
            const string& code,
            const string& name,
            int purchasePrice,
            int mortgageValue,
            const string& colorGroup,
            int houseBuildCost,
            int hotelBuildCost,
            const vector<int>& rentLevels
        );
        PropertyConfig(const PropertyConfig& other);
        ~PropertyConfig();
        PropertyConfig& operator=(const PropertyConfig& other);

        const string& getCode() const;
        const string& getName() const;
        int getPurchasePrice() const;
        int getMortgageValue() const;
        const string& getColorGroup() const;
        int getHouseBuildCost() const;
        int getHotelBuildCost() const;
        const vector<int>& getRentLevels() const;
    };

private:
    static constexpr const char* PROPERTY_CONFIG_PATH = "config/property.txt";
    static constexpr const char* RAILROAD_CONFIG_PATH = "config/railroad.txt";
    static constexpr const char* UTILITY_CONFIG_PATH = "config/utility.txt";
    static constexpr const char* TAX_CONFIG_PATH = "config/tax.txt";
    static constexpr const char* SPECIAL_CONFIG_PATH = "config/special.txt";
    static constexpr const char* MISC_CONFIG_PATH = "config/misc.txt";

    map<string, PropertyConfig> propertyConfigs;
    map<int, string> propertyCodeById;
    map<int, int> railroadRentTable;
    map<int, int> utilityMultiplierTable;
    int pphFlat;
    int pphPercent;
    int pbmFlat;
    int goSalary;
    int jailFine;
    int maxTurn;
    int initialBalance;

    string trim(const string& value) const;
    string toUpperCopy(string value) const;
    string stripInlineComment(const string& line) const;
    vector<string> splitWhitespace(const string& line) const;
    vector<vector<string>> readTokenRows(const string& filename) const;
    int parseInt(const string& token, const string& filename, const string& fieldName) const;
    bool isIntegerToken(const string& token) const;
    map<string, size_t> buildHeaderIndex(const vector<string>& headerRow) const;
    bool hasHeaderFields(const vector<string>& row, const vector<string>& requiredFields) const;
    int getValueByAliases(const map<string, int>& values, const vector<string>& aliases, const string& filename) const;
    map<string, int> parseScalarConfig(const string& filename, const vector<string>& expectedFields) const;

public:
    ConfigManager();
    ConfigManager(
        const map<string, PropertyConfig>& propertyConfigs,
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

    const map<string, PropertyConfig>& getPropertyConfigs() const;
    const PropertyConfig& getPropertyConfig(const string& code) const;

    const map<int, string>& getPropertyCodeByIdMap() const;
    const string& getPropertyCodeById(int id) const;
    const PropertyConfig& getPropertyConfigById(int id) const;
    bool hasPropertyId(int id) const;

    const map<int, int>& getRailroadRentTable() const;
    int getRailroadRent(int railroadCount) const;
    const map<int, int>& getUtilityMultiplierTable() const;
    int getUtilityMultiplier(int utilityCount) const;
    int getPphFlat() const;
    int getPphPercent() const;
    int getPbmFlat() const;
    int getGoSalary() const;
    int getJailFine() const;
    int getMaxTurn() const;
    int getInitialBalance() const;
};
