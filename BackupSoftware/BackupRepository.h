#include <string>
#include <vector>
#include "FileInfo.h"

/**
 * Project Untitled
 */


#ifndef _BACKUPREPOSITORY_H
#define _BACKUPREPOSITORY_H

class BackupRepository {
public: 
    std::string repositoryPath;
    std::vector<FileInfo> backupRecords;
    std::vector<bool> compressionEnabled;
    std::string compressionType;
    std::vector<bool> encryptionEnabled;
    std::string encryptionKey;
    
BackupRepository();
};

#endif //_BACKUPREPOSITORY_H