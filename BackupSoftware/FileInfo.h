#include <string>
#include <ctime>
/**
 * Project Untitled
 */


#ifndef _FILEINFO_H
#define _FILEINFO_H

class FileInfo {
public: 
    std::string fileName;
    std::string filePath;
    long fileSize;
    std::time_t creationTime;
    std::time_t lastModified;
    std::time_t lastAccessed ;
    uint16_t filePermissions;
    std::string fileType;
    bool isHidden;
    
FileInfo();
};

#endif //_FILEINFO_H