#include<string>
/**
 * Project Untitled
 */


#ifndef _RESTOREBACKUP_H
#define _RESTOREBACKUP_H

class RestoreBackup {
public: 
    
/**
 * @param backupFilePath
 * @param restorePath
 * @param isDecompressed
 * @param isDecrypted
 * @param decryptionKey
 */
RestoreBackup(std::string backupFilePath, std::string restorePath, bool isDecompressed, bool isDecrypted, std::string decryptionKey);
};

#endif //_RESTOREBACKUP_H