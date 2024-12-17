/**
 * Project Untitled
 */


#ifndef INCLUDE_ENCRYPTION_H
#define INCLUDE_ENCRYPTION_H

#include "FileInfo.h"
#include <openssl/aes.h>
#include <openssl/md5.h>

#define FILE_SUFFIX_ENCRYPT ".ept"

class Encryption {
private:
    unsigned char key[MD5_DIGEST_LENGTH];
    fs::path file_path;

public:
    Encryption(std::string file_path_, std::string password_);
    ~Encryption();
    bool encrypt();
    
    /**
     * @brief 解密
     * @return 0成功 -1密码错误 -2文件错误
     */
    int decrypt();
};

#endif //INCLUDE_ENCRYPTION_H