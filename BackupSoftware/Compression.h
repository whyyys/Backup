/**
 * Project Untitled
 */


#ifndef _COMPRESSION_H
#define _COMPRESSION_H

class Compression {
public: 
    
/**
 * @param sourcePath
 * @param destinationPath
 * @param compressionType
 */
bool compress(std::string sourcePath, std::string destinationPath, std::string compressionType);
    
/**
 * @param sourcePath
 * @param destinationPath
 * @param compressionType
 */
bool decompress(std::string sourcePath, std::string destinationPath, std::string compressionType);
};

#endif //_COMPRESSION_H