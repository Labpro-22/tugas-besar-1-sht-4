#include "model/managers/LogManager.hpp"

using namespace std;

LogManager::LogEntry::LogEntry() {}

LogManager::LogEntry::LogEntry(int turnNumber, const string& username, const string& actionType, const string& detail) {}

LogManager::LogEntry::LogEntry(const LogEntry& other) {}

LogManager::LogEntry::~LogEntry() {}

LogManager::LogEntry& LogManager::LogEntry::operator=(const LogEntry& other) {
    return *this;
}

string LogManager::LogEntry::format() const {
    return "";
}

LogManager::LogManager() {}

LogManager::LogManager(const vector<LogEntry>& logs) {}

LogManager::LogManager(const LogManager& other) {}

LogManager::~LogManager() {}

LogManager& LogManager::operator=(const LogManager& other) {
    return *this;
}

void LogManager::addLog(int turnNumber, const string& username, const string& actionType, const string& detail) {}

const vector<LogManager::LogEntry>& LogManager::getLogs() const {
    return logs;
}

vector<LogManager::LogEntry> LogManager::getRecentLogs(int n) const {
    return {};
}