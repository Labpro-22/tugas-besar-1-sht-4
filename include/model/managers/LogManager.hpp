#pragma once

#include <string>
#include <vector>

using namespace std;

class LogManager {
public:
    class LogEntry {
    private:
        int turnNumber;
        string username;
        string actionType;
        string detail;

    public:
        LogEntry();
        LogEntry(int turnNumber, const string& username, const string& actionType, const string& detail);
        LogEntry(const LogEntry& other);
        ~LogEntry();
        LogEntry& operator=(const LogEntry& other);

        string format() const;
    };

private:
    vector<LogEntry> logs;

public:
    LogManager();
    LogManager(const vector<LogEntry>& logs);
    LogManager(const LogManager& other);
    ~LogManager();
    LogManager& operator=(const LogManager& other);

    void addLog(int turnNumber, const string& username, const string& actionType, const string& detail);
    const vector<LogEntry>& getLogs() const;
    vector<LogEntry> getRecentLogs(int n) const;
};