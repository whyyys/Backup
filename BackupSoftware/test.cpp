#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct fileinfo{
    fs::path filepath;
};

class File : public std::fstream
{
private:
    fs::path filepath;     // 文件路径
public:
    File(fs::path filepath_);
    ~File();
    fileinfo copypath();
    bool OpenFile(std::ios_base::openmode mode_);
};

bool File::OpenFile(std::ios_base::openmode mode_)
{
    this->open(filepath.string(), mode_);
    return is_open();
}

fileinfo File::copypath()
{
    fileinfo tmpinfo;
    // 读取 BACKUP_INFO_SIZE 字节的数据到 tmpinfo
    this->read((char *)&tmpinfo, sizeof(fileinfo));
    return tmpinfo;  // 返回读取到的数据副本
}

File::File(fs::path filepath_)
{
    filepath.assign(filepath_);
}

File::~File()
{}

int main() {
    // 随便一个文件路径
    fs::path pack_path = "/home/why/Backup/BackupSoftware/test.txt";
    File bak_file(pack_path);
    if (!bak_file.OpenFile(std::ios::in | std::ios::binary))
        return -1;
    // 读取信息到fs::path变量里就会报错，注释这一句就不会报错
    // 读取备份信息
    fileinfo info = bak_file.copypath();

    
    bak_file.close();
    std::cout<<"success"<<std::endl;
    return 0;
}
