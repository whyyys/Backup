/**
 * Project Untitled
 */

#include "FileInfo.h"
#include<iostream>
/**
 * FileInfo implementation
 */

FileInfo::FileInfo(fs::path filepath_)
{
    filepath.assign(filepath_);
    memset(&fileheader, 0, sizeof(fileheader));
    // 获取文件元数据
    lstat(filepath.string().c_str(), &(fileheader.metadata));
    strcpy(fileheader.name, filepath.string().c_str());
    if (GetFileType(fileheader) == FILE_TYPE_SYMBOLIC_LINK)
    {
        char link_buf[MAX_PATH_LEN] = {0};
        readlink(filepath.string().c_str(), link_buf, MAX_PATH_LEN);
        strcpy(fileheader.linkfile, link_buf);
    }
}

FileInfo::FileInfo(FileHeader fileheader_)
{
    this->fileheader = fileheader_;
    filepath.assign(fileheader.name);
    unsigned int major_num, minor_num;
    // 硬链接处理
    if (IsHardLink())
    {
        if (fs::exists(fileheader.name))
        {
            fs::remove_all(fileheader.name);
        }
        return;
    }
    if (GetFileType() == FILE_TYPE_CHARACTER_DEVICE || GetFileType() == FILE_TYPE_BLOCK_DEVICE)
    {
        // 获取设备号
        dev_t dev_id = fileheader.metadata.st_rdev;
        major_num = major(dev_id); // 获取主设备号
        minor_num = minor(dev_id); // 获取次设备号
    }
    // 创建多级目录
    switch (GetFileType())
    {
    case FILE_TYPE_NORMAL:
        // 创建父目录
        if (!fs::exists(filepath.parent_path())){
        fs::create_directories(filepath.parent_path());}
        break;
    case FILE_TYPE_DIRECTORY:
        if (!fs::exists(filepath)){
        fs::create_directories(filepath);}
        break;
    case FILE_TYPE_SYMBOLIC_LINK:
        // 需要先删除文件
        if (fs::exists(fileheader.name)){
            fs::remove_all(fileheader.name);
        }
        break;
    case FILE_TYPE_FIFO:
        // 需要先删除文件
        if (fs::exists(fileheader.name)){
            fs::remove_all(fileheader.name);
        }
        mkfifo(fileheader.name, fileheader.metadata.st_mode);
        break;
    // 创建块设备和字符设备文件
    case FILE_TYPE_CHARACTER_DEVICE:
        // 创建一个模拟的字符设备
        mknod(filepath.c_str(), S_IFCHR | 0666, makedev(major_num, minor_num)); // 设备号100,0
        break;
    case FILE_TYPE_BLOCK_DEVICE:
        // 创建一个模拟的块设备
        mknod(filepath.c_str(), S_IFBLK | 0666, makedev(major_num, minor_num)); // 设备号8,0
        break;
    default:
        break;
    }
}

FileInfo::~FileInfo() {}

bool FileInfo::OpenFile(std::ios_base::openmode mode_)
{
    this->open(filepath.string(), mode_);
    return is_open();
}

bool FileInfo::IsHardLink()
{
    return (fileheader.metadata.st_nlink > 1);
}

FileHeader FileInfo::ReadFileHeader()
{
    FileHeader tmpheader;
    this->read((char *)&tmpheader, FILE_HEADER_SIZE);
    return tmpheader;
}
BackupInfo FileInfo::ReadBackupInfo()
{
    BackupInfo tmpinfo;
    // 读取 BACKUP_INFO_SIZE 字节的数据到 tmpinfo
    this->read((char *)&tmpinfo, BACKUP_INFO_SIZE);
    return tmpinfo;  // 返回读取到的数据副本
}
void FileInfo::WriteBackupInfo(BackupInfo info)
{
    this->write((char *)&info, BACKUP_INFO_SIZE);
}

FileType FileInfo::GetFileType()
{
    return FileInfo::GetFileType(this->fileheader);
}

FileType FileInfo::GetFileType(const FileHeader &file_header_)
{
    switch (file_header_.metadata.st_mode & S_IFMT)
    {
    case S_IFREG:
        return FILE_TYPE_NORMAL; // 普通文件
        break;
    case S_IFDIR:
        return FILE_TYPE_DIRECTORY; // 目录
        break;
    case S_IFLNK:
        return FILE_TYPE_SYMBOLIC_LINK; // 符号链接
        break;
    case S_IFIFO:
        return FILE_TYPE_FIFO; // 管道文件（FIFO）
        break;
    case S_IFBLK:
        return FILE_TYPE_BLOCK_DEVICE; // 块设备文件
        break;
    case S_IFCHR:
        return FILE_TYPE_CHARACTER_DEVICE; // 字符设备文件
        break;
    default:
        return FILE_TYPE_OTHER; // 其他类型的文件
        break;
    }
}

size_t FileInfo::GetFileSize()
{
    return fileheader.metadata.st_size;
}

FileHeader FileInfo::GetFileHeader()
{
    return fileheader;
}

void FileInfo::RestoreMetadata()
{
    struct stat *metadata = &(fileheader.metadata);

    // 还原文件权限信息
    chmod(fileheader.name, metadata->st_mode);

    // 还原文件的用户和组
    lchown(fileheader.name, metadata->st_uid, metadata->st_gid);

    // 还原时间戳(访问时间 修改时间)
    timespec tim[2] = {metadata->st_atim, metadata->st_mtim};
    utimensat(AT_FDCWD, fileheader.name, tim, AT_SYMLINK_NOFOLLOW);
}

bool FileInfo::CalculateFileChecksum()
{
    std::string file_path = filepath.string();
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    uint32_t checksum = 0;
    char buffer[1024];

    // 逐块读取文件并计算校验和
    while (file.read(buffer, sizeof(buffer)))
    {
        size_t bytes_read = file.gcount();
        for (size_t i = 0; i < bytes_read; ++i)
        {
            checksum += static_cast<uint32_t>(buffer[i]);
        }
    }

    // 处理最后不足1024字节的数据
    size_t bytes_read = file.gcount();
    for (size_t i = 0; i < bytes_read; ++i)
    {
        checksum += static_cast<uint32_t>(buffer[i]);
    }

    // file.close();

    // 计算校验码的负值并写到文件末尾
    uint32_t checksum_to_write = -checksum; // 校验码是负的校验和
    std::ofstream outfile(file_path, std::ios::binary | std::ios::app);
    if (!outfile.is_open())
    {
        return false;
    }

    // 将校验和追加到文件末尾
    outfile.write(reinterpret_cast<const char *>(&checksum_to_write), sizeof(checksum_to_write));
    // outfile.close();
    return true;
}
