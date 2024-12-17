#ifndef INCLUDE_PACKER_H
#define INCLUDE_PACKER_H

#include "FilterOptions.h"
#include <unordered_map>
#define FILE_SUFFIX_PACK ".pak"

class Packer
{
private:
    fs::path root_path; // 需要备份或恢复的文件路径
    fs::path pack_path;  // 打包文件的路径
    std::unordered_map<ino_t, std::string> inode_table;
    std::vector<FileHeader> hardlinklist;
    std::vector<FileHeader> symbollinklist;

    FilterOptions filter;
    void DfsFile(FileInfo &bak_file, fs::path cur_path);

public:
    Packer(std::string root_path_, std::string pack_path_, const FilterOptions &filter_);
    ~Packer();

    // 打包到bak_path
    bool Pack();

    /// @brief 解包
    /// @return 解包成功返回true
    bool Unpack();
};

#endif // INCLUDE_PACKER_H_
