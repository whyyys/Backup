/**
 * Project Untitled
 */

#ifndef INCLUDE_BACKUPMANAGER_H
#define INCLUDE_BACKUPMANAGER_H

#include <iostream>
#include "FileInfo.h"
#include "FilterOptions.h"
#include "Compression.h"
#include "Encryption.h"
#include "Packer.h"

class BackupManager
{
private:
    fs::path src_path; // 待备份文件目录
    fs::path dst_path; // 存放路径

    BackupInfo info;      // 备份文件信息
    FilterOptions filter; // 过滤条件

    std::vector<std::string> outinfo; // 输出信息

public:
    BackupManager(std::string src_path_, std::string dst_path_);
    ~BackupManager();

    // 备份
    bool CreateBackup(std::string password = nullptr);
    void SetComment(std::string comment_);
    void SetMod(unsigned char mod_);
    void SetFilter(const FilterOptions &filteroptions_);
    // 恢复
    bool RestoreBackup(std::string password = nullptr);
    int GetBackupInfo();
    unsigned char GetBackupMode();
    static int GetBackupInfo(std::string file_path_, BackupInfo &info_);
    static bool VerifyFileChecksum(const std::string& file_path);
    // 获取输出信息
    std::vector<std::string> Getoutinfo();
};

#endif // INCLUDE_BACKUPMANAGER_H