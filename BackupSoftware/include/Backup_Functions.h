// Backup_Functions.h
#ifndef BACKUP_FUNCTIONS_H
#define BACKUP_FUNCTIONS_H

#include <string>
#include "Packer.h"
#include "Compression.h"
#include "Encryption.h"
#include "FilterOptions.h"

#ifdef __cplusplus
extern "C" {
#endif

// 定义打包和解包接口
class BackupFunctions {
private:
    // 打包用路径
    fs::path source_path; // 待备份文件目录
    fs::path destination_path; // 存放路径
    // 解包用路径
    fs::path restore_path; // 解包存放路径
    fs::path backupfile; // 解包文件位置
    std::string password; // 密码

    BackupInfo backupinformation;      // 备份文件信息
    FilterOptions filter; // 过滤条件

    std::string comment; //评论信息
    std::vector<std::string> outinfo; // 输出信息
public:
    BackupFunctions(const fs::path &src_path_, const fs::path &dst_path, const fs::path &rst_path_, const fs::path &bkf,
                 std::string comment_, std::string password_ = nullptr);
    ~BackupFunctions();

    void SetBackupInfo(const BackupInfo &backupinformation_);
    void SetFilter(const FilterOptions &filteroptions_);

    // 获取文件备份信息并验证校验码
    static bool GetBackupInfo(const fs::path &file_path_, BackupInfo &info_);
    // 获取输出信息
    std::vector<std::string> Getoutinfo();

    // 备份和解备份函数
    bool CreateBackup();
    bool RestoreBackup();
};

#ifdef __cplusplus
}
#endif

#endif  // BACKUP_FUNCTIONS_H

