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

        int getTurnNumber() const;
        const string& getUsername() const;
        const string& getActionType() const;
        const string& getDetail() const;
        string format() const;
        int getTurnNumber() const;
        const string& getUsername() const;
        const string& getActionType() const;
        const string& getDetail() const;
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
    void clear();
    const vector<LogEntry>& getLogs() const;
    vector<LogEntry> getRecentLogs(int n) const;
};