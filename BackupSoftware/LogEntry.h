#include <ctime>
#include <string>
/**
 * Project Untitled
 */


#ifndef _LOGENTRY_H
#define _LOGENTRY_H

class LogEntry {
public: 
    std::time_t operationTime;
    bool operationType;
    std::string sourcePath;
    std::string destinationPath;
    bool isCompressed;
    std::string compressionMethod;
    bool isEncrypted;
    std::string encryptionMethod;
    bool status;
    
LogEntry();
};

#endif //_LOGENTRY_H