#include <string>
#include "FilterOptions.h"
/**
 * Project Untitled
 */


#ifndef _CREATEBACKUP_H
#define _CREATEBACKUP_H

class CreateBackup {
public: 
    
/**
 * @param sourcePath
 * @param destinationPath
 * @param filters
 * @param isCompressed
 * @param compressionType
 * @param isEncrypted
 * @param encryptionType
 * @param encryptionKey
 */
CreateBackup(std::string sourcePath, std::string destinationPath, FilterOptions filters, bool isCompressed, std::string compressionType, bool isEncrypted, std::string encryptionType, std::string encryptionKey);
};

#endif //_CREATEBACKUP_H