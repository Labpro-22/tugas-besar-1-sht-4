#include "model/managers/LogManager.hpp"

#include <iomanip>
#include <sstream>

using namespace std;

LogManager::LogEntry::LogEntry()
    : turnNumber(0),
      username(""),
      actionType(""),
      detail("")
{}

LogManager::LogEntry::LogEntry(int turnNumber, const string& username, const string& actionType, const string& detail)
    : turnNumber(turnNumber), username(username), actionType(actionType), detail(detail) {}

LogManager::LogEntry::LogEntry(const LogEntry& other)
    : turnNumber(other.turnNumber),
      username(other.username),
      actionType(other.actionType),
      detail(other.detail) {}

LogManager::LogEntry::~LogEntry() {}

LogManager::LogEntry& LogManager::LogEntry::operator=(const LogEntry& other) {
    if (this != &other) {
        turnNumber = other.turnNumber;
        username = other.username;
        actionType = other.actionType;
        detail = other.detail;
    }
    return *this;
}

int LogManager::LogEntry::getTurnNumber() const { return turnNumber; }
const string& LogManager::LogEntry::getUsername() const { return username; }
const string& LogManager::LogEntry::getActionType() const { return actionType; }
const string& LogManager::LogEntry::getDetail() const { return detail; }

string LogManager::LogEntry::format() const {
    ostringstream stream;
    stream << "[Turn " << turnNumber << "] "
           << username << " | "
           << left << setw(8) << actionType << " | "
           << detail;
    return stream.str();
}

LogManager::LogManager()
    : logs()
{}

LogManager::LogManager(const vector<LogEntry>& logs) : logs(logs) {}

LogManager::LogManager(const LogManager& other) : logs(other.logs) {}

LogManager::~LogManager() {}

LogManager& LogManager::operator=(const LogManager& other) {
    if (this != &other) {
        logs = other.logs;
    }
    return *this;
}

void LogManager::addLog(int turnNumber, const string& username, const string& actionType, const string& detail) {
    logs.push_back(LogEntry(turnNumber, username, actionType, detail));
}

void LogManager::clear() {
    logs.clear();
}

const vector<LogManager::LogEntry>& LogManager::getLogs() const {
    return logs;
}

vector<LogManager::LogEntry> LogManager::getRecentLogs(int n) const {
    if (n <= 0 || logs.empty()) {
        return {};
    }

    if (n >= static_cast<int>(logs.size())) {
        return logs;
    }

    return vector<LogEntry>(logs.end() - n, logs.end());
}
