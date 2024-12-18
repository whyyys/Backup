/**
 * Project Untitled
 */

#ifndef INCLUDE_FILEINFO_H
#define INCLUDE_FILEINFO_H

#include <ctime>
#include <string>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <filesystem>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <iostream>
namespace fs = std::filesystem;

#define MAX_PATH_LEN 256 // 最大路径长度
#define BUFFER_SIZE 4096 // 缓冲区大小
#define COMMENT_SIZE 255 // 备份信息长度

// 文件类型
#define FILE_TYPE_NORMAL 1
#define FILE_TYPE_HARD_LINK 2
#define FILE_TYPE_SYMBOLIC_LINK 4
#define FILE_TYPE_DIRECTORY 8
#define FILE_TYPE_FIFO 16
#define FILE_TYPE_BLOCK_DEVICE 32
#define FILE_TYPE_CHARACTER_DEVICE 64
#define FILE_TYPE_OTHER 128

// 压缩加密
#define MOD_COMPRESS 1
#define MOD_ENCRYPT 2

typedef unsigned char FileType;

struct BackupInfo
{
    time_t timestamp;               // 时间戳
    char backup_path[MAX_PATH_LEN]; // 备份路径
    char comment[COMMENT_SIZE];     // 描述信息
    unsigned char mod;              // 压缩加密选项
    uint32_t checksum;              //校验和
};

#define BACKUP_INFO_SIZE (sizeof(BackupInfo))

struct FileHeader
{
    char name[MAX_PATH_LEN]; // 文件名
    struct stat metadata;

    // 硬链接文件只有其链接目标在备份文件范围内才有效
    char linkfile[MAX_PATH_LEN];
};

#define FILE_HEADER_SIZE (sizeof(FileHeader))

class FileInfo : public std::fstream
{
private:
    FileHeader fileheader; // 文件元数据
    fs::path filepath;     // 文件路径

public:
    // 传入文件元数据创建文件
    FileInfo(FileHeader fileheader_);
    // 根据路径解析文件
    FileInfo(fs::path filepath_);
    ~FileInfo();

    bool OpenFile(std::ios_base::openmode mode_);
    bool IsHardLink();

    // 读取文件头信息
    FileHeader ReadFileHeader();
    // 读写文件备份信息
    BackupInfo ReadBackupInfo();
    void WriteBackupInfo(BackupInfo info);
    // 读取文件类型信息
    FileType GetFileType();
    static FileType GetFileType(const FileHeader &file_header_);
    size_t GetFileSize();
    FileHeader GetFileHeader();
    bool CalculateFileChecksum();
    void RestoreMetadata();
};

#endif // INCLUDE_FILEINFO_H