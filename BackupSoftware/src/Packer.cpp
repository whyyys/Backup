#include "Packer.h"

void Packer::DfsFile(FileInfo &bak_file, fs::path cur_path)
{
    FileInfo file(cur_path);
    FileHeader fileheader = file.GetFileHeader();
    // 过滤规则判断
    bool file_status = filter.Check(fileheader);
    if(!file_status) return;

    // 硬链接 第二次检测到同一个inode时视为硬链接
    if(file.IsHardLink())
    {
        if(inode_table.count(fileheader.metadata.st_ino))
        {
            // 指向文件已经被打包，后续恢复连接即可
            strcpy(fileheader.linkfile, inode_table[fileheader.metadata.st_ino].c_str());
            bak_file.write((const char *)&fileheader, sizeof(fileheader));
            return;
        }
        else{
            fileheader.metadata.st_nlink = 1;
            inode_table[fileheader.metadata.st_ino] = cur_path.string();
        }
    }
        
    char buf[BUFFER_SIZE] = {0};
    switch (file.GetFileType())
    {
    case FILE_TYPE_DIRECTORY:
        bak_file.write((const char *)&fileheader, sizeof(fileheader));
        for (const auto &entry : fs::directory_iterator(cur_path))
        {
            DfsFile(bak_file, entry.path());
        }
        break;
    case FILE_TYPE_NORMAL:
        bak_file.write((const char *)&fileheader, sizeof(fileheader));

        file.OpenFile(std::ios::in | std::ios::binary);

        while (file.peek() != EOF)
        {
            file.read(buf, sizeof(buf));
            bak_file.write(buf, file.gcount());
        }
        file.close();
        break;
    case FILE_TYPE_SYMBOLIC_LINK:
        bak_file.write((const char *)&fileheader, sizeof(fileheader));
        break;
    case FILE_TYPE_FIFO:
        bak_file.write((const char *)&fileheader, sizeof(fileheader));
        break;
    case FILE_TYPE_CHARACTER_DEVICE:
        bak_file.write((const char *)&fileheader, sizeof(fileheader));
        break;
    case FILE_TYPE_BLOCK_DEVICE:
        bak_file.write((const char *)&fileheader, sizeof(fileheader));
        break;
    default:
        break;
    }
}

Packer::Packer(std::string root_path_, std::string pack_path_, const FilterOptions &filter_){
    root_path.assign(root_path_);
    pack_path.assign(pack_path_);
    filter = filter_;
}

Packer::~Packer(){

}
/// @brief 打包
/// @param root_path 打包文件/文件夹
/// @param pack_path  打包到的路径
/// @return 解包成功返回true
bool Packer::Pack()
{
    pack_path /= (root_path.filename().string() + FILE_SUFFIX_PACK);
    FileHeader bak_file_header = {0};
    strcpy(bak_file_header.name, pack_path.string().c_str());
    // 打包为普通文件
    bak_file_header.metadata.st_mode = S_IFREG;
    bak_file_header.metadata.st_nlink = 1;

    // 创建打包文件
    FileInfo bak_file(bak_file_header);
    if (!bak_file.OpenFile(std::ios::out | std::ios::binary | std::ios::trunc))
        return false;

    BackupInfo info = {
        time(NULL),                // 当前时间戳
        {0},                       // 确保 backup_path 初始化为空字符
        {0},                       // 确保 comment 初始化为空字符
        0,                         // 压缩加密选项
        0                          // 校验码
    };

    // 安全地拷贝路径和评论
    strncpy(info.backup_path, root_path.string().c_str(), MAX_PATH_LEN - 1);
    info.backup_path[MAX_PATH_LEN - 1] = '\0';  // 确保路径字符串终止符

    strncpy(info.comment, "Pack from why.", COMMENT_SIZE - 1);
    info.comment[COMMENT_SIZE - 1] = '\0';  // 确保评论字符串终止符

    info.mod = 0;  // 压缩加密选项，假设为 0

    info.checksum = 0; // 这里不进行文件校验

    bak_file.WriteBackupInfo(info); 

    // 切换工作目录
    fs::current_path(root_path.parent_path());
    // 深度优先遍历目录树
    DfsFile(bak_file, fs::relative(root_path, root_path.parent_path()));

    bak_file.close();
    return true;
}

/// @brief 解包
/// @param root_path 还原路径
/// @param pack_path  打包文件所在路径
/// @return 解包成功返回true
bool Packer::Unpack(){
    // 检查文件扩展名是否为 .pak
    if (pack_path.extension() != FILE_SUFFIX_PACK)
        return false;

    FileInfo bak_file(pack_path);
    if (!bak_file.OpenFile(std::ios::in | std::ios::binary))
        return false;

    // 读取备份信息
    BackupInfo info = bak_file.ReadBackupInfo();

    // 使用指定路径解包
    fs::create_directories(root_path);
    fs::current_path(root_path);

    char buf[BUFFER_SIZE] = {0};
    FileHeader fileheader = {0};
    while (bak_file.peek() != EOF)
    {
        // 读文件头
        fileheader = bak_file.ReadFileHeader();
        // 创建文件对象
        FileInfo file(fileheader);
        // 链接文件最后处理
        if ((fileheader.metadata.st_mode & S_IFMT) == S_IFLNK)
        {
            symbollinklist.push_back(fileheader);
            continue;
        }
        if (fileheader.metadata.st_nlink > 1)
        {
            hardlinklist.push_back(fileheader);
            continue;
        }
        
        // 只有普通文件需要复制文件内容到新文件中
        if (file.GetFileType() == FILE_TYPE_NORMAL)
        {
            if(!file.OpenFile(std::ios::out | std::ios::binary | std::ios::trunc))return false;
            size_t file_size = file.GetFileSize();
            while (file_size >= BUFFER_SIZE)
            {
                bak_file.read(buf, BUFFER_SIZE);
                file.write(buf, BUFFER_SIZE);
                file_size -= BUFFER_SIZE;
            }
            if (file_size)
            {
                bak_file.read(buf, file_size);
                file.write(buf, file_size);
            }
            file.close();
        }
        // 恢复文件时间戳
        file.RestoreMetadata();
    }
        // 恢复链接文件
        for (const auto& hardlinkheader : hardlinklist) 
    {
        fs::create_hard_link(hardlinkheader.linkfile, hardlinkheader.name);
    }
    for (const auto& symbollinkheader : symbollinklist) 
    {
        fs::create_symlink(symbollinkheader.linkfile, symbollinkheader.name);
    }

    bak_file.close();
    return true;
}