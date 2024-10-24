/**
 * Project Untitled
 */


#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

class Encryption {
public: 
    
/**
 * @param sourcePath
 * @param destinationPath
 * @param key
 */
bool encrypt(std::string sourcePath, std::string destinationPath, std::string key);
    
/**
 * @param sourcePath
 * @param destinationPath
 * @param key
 */
bool decrypt(std::string sourcePath, std::string destinationPath, std::string key);
};

#endif //_ENCRYPTION_H