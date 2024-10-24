#include "LogEntry.h"
/**
 * Project Untitled
 */


#ifndef _LOGMANAGER_H
#define _LOGMANAGER_H

class LogManager {
public: 
    
/**
 * @param logEntry
 */
bool writeLog(LogEntry logEntry);
    
std::vector<LogEntry> getLogs();
private: 
    std::vector<LogEntry> logEntries;
};

#endif //_LOGMANAGER_H