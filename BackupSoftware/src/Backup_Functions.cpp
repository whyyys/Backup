// Backup_Functions.cpp
#include "Backup_Functions.h"
#include "Packer.h"
#include "Compression.h"
#include "Encryption.h"
#include "FilterOptions.h"

BackupFunctions::BackupFunctions(const fs::path &src_path_, const fs::path &dst_path, const fs::path &rst_path_, const fs::path &bkf,
                                std::string comment_, std::string password_){
    source_path.assign(src_path_);
    destination_path.assign(dst_path);
    restore_path.assign(rst_path_);
    backupfile.assign(bkf);
    password = password_;
    comment = comment_;
}
BackupFunctions::~BackupFunctions(){
}

void BackupFunctions::SetMod(unsigned char mod_){
    mod = mod_;
}
// void BackupFunctions::SetFilter(const FilterOptions &filteroptions_){
//     filter = filteroptions_;
// }

// 获取文件备份信息
bool BackupFunctions::GetBackupInfo(const fs::path &file_path_, BackupInformation &info_) {
    std::ifstream file(file_path_, std::ios::binary | std::ios::ate);  // 打开文件（以二进制模式打开，并定位到文件末尾）
    if (!file.is_open()) return false;
    // 获取文件大小
    std::streampos file_size = file.tellg();
    // 计算 BackupInfo 结构体的大小
    size_t backup_info_size = sizeof(BackupInformation);
    // 确保文件有至少一个结构体
    if (file_size < backup_info_size) return false;
    // 计算最后一个 BackupInfo 结构体的位置
    size_t last_struct_pos = static_cast<size_t>(file_size) - backup_info_size;
    // 定位到最后一个结构体的位置
    file.seekg(last_struct_pos, std::ios::beg);
    // 读取最后一个 BackupInfo 结构体
    file.read(reinterpret_cast<char*>(&info_), backup_info_size);
    // 关闭文件
    file.close();
    return true;
}
// 获取输出信息
std::vector<std::string> BackupFunctions::Getoutinfo(){
    return outinfo;
}

// 备份和解备份函数
bool BackupFunctions::CreateBackup(){
    unsigned char flag = 0;
    fs::path dstfile = destination_path / source_path.filename();
    fs::path pakfile = dstfile; pakfile += FILE_SUFFIX_PACK;
    fs::path cpsfile = pakfile; cpsfile += FILE_SUFFIX_COMPRESS;
    fs::path eptfile = cpsfile; eptfile += FILE_SUFFIX_ENCRYPT;
    // 判断路径是否存在
    if (!fs::exists(source_path))
    {
        outinfo.push_back("Error: No such file or directory: " + source_path.string());
        return false;
    }

    // 打包
    outinfo.push_back("PACKING...");
    FilterOptions filter;
    Packer packer(source_path, destination_path, filter);
    if (!packer.Pack())
    {
        outinfo.push_back("Error: Failed to pack file.");
        return false;
    }
    dstfile = pakfile;
    // 压缩
    if (mod & MOD_COMPRESS)
    {
        outinfo.push_back("COMORESSING...");
        Compression compression(pakfile);
        if (!compression.compress())
        {
            outinfo.push_back("Error: Failed to compress file.");
            return false;
        }
        fs::remove_all(pakfile);
        dstfile = cpsfile;
        flag |= MOD_COMPRESS;
    }

    // 加密
    if (mod & MOD_ENCRYPT)
    {
        outinfo.push_back("ENCRYPTING...");
        Encryption encryption(cpsfile, password);
        if (!encryption.encrypt())
        {
            outinfo.push_back("Error: Failed to encrypt file.");
            return false;
        }
        fs::remove_all(cpsfile);
        dstfile = eptfile;
        flag |= MOD_ENCRYPT;
    }

    BackupInformation nowinfo = {
        time(NULL),                // 当前时间戳
        {0},                       // 确保 backup_path 初始化为空字符
        {0},                       // 确保 comment 初始化为空字符
        0,                         // 压缩加密选项
        0                          // 校验码
    };
    // 安全地拷贝路径和评论
    memcpy(nowinfo.comment, comment.c_str(), std::min(sizeof(nowinfo.comment), comment.length()));
    memcpy(nowinfo.backup_path, dstfile.string().c_str(), std::min(sizeof(nowinfo.backup_path), dstfile.string().length()));
    nowinfo.mod = flag;  // 压缩加密选项

    //生成文件校验码
    std::string file_path = dstfile.string();
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) outinfo.push_back("Calculate filechecksum error: can't open file.");

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

    file.close();

    // 计算校验码
    nowinfo.checksum = checksum;

    // 保存备份信息
    std::ofstream outfile(file_path, std::ios::binary | std::ios::app);
    if (!outfile.is_open()) outinfo.push_back("Calculate filechecksum error: can't open file.");

    // 将校验和追加到文件末尾
    outfile.write(reinterpret_cast<const char *>(&nowinfo), BACKUP_INFO_SIZE);
    outfile.close();

    outinfo.push_back("Backup succeed!");
    return true;
}
bool BackupFunctions::RestoreBackup(){
    fs::path dofile(backupfile);
    // 判断路径是否存在
    if (!fs::exists(restore_path))
    {
        outinfo.push_back("Error: No such file or directory: " + restore_path.string());
        return false;
    }
    // 判断文件是否存在
    if (!fs::exists(dofile))
    {
        outinfo.push_back("Error: No such file or directory: " + dofile.string());
        return false;
    }
    BackupInformation info = {0};
    GetBackupInfo(dofile, info);
    fs::path file_path(dofile);
    // 校验文件并删除文件末尾的结构体待解包结束后重新写入
    // 打开文件，定位到文件末尾
    std::ifstream file_(file_path, std::ios::binary | std::ios::ate);
    if (!file_)
    {
        outinfo.push_back("Check filechecksum error: can't open file");
        return false;
    }
    // 获取文件大小
    std::streampos file_size = file_.tellg();
    file_.close();
    // BackupInfo 结构体的大小
    size_t backup_info_size = sizeof(BackupInformation);
    // 如果文件大小小于一个结构体的大小，说明没有足够的数据删除
    if (file_size < backup_info_size)
    {
        outinfo.push_back("Check filechecksum error: file is smaller than a BackupInfo structure!");
        return false;
    }
    // 打开文件进行写入，使用文件截断操作
    fs::resize_file(file_path, static_cast<size_t>(file_size) - backup_info_size);

    //计算文件校验码
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) outinfo.push_back("Calculate filechecksum error: can't open file.");

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

    file.close();

    // 校验文件
    if(info.checksum != checksum)
    {
        outinfo.push_back("Error: Check filechecksum error!");
        return false;
    }

    // 解密
    if (info.mod & MOD_ENCRYPT)
    {
        outinfo.push_back("DECRYPTING...");
        Encryption encryption(dofile, password);
        int status = encryption.decrypt();
        if (status == -2)
        {
            outinfo.push_back("Error: Failed to decrypt file.");
            return false;
        }
        else if (status == -1)
        {
            outinfo.push_back("Error: Wrong password!");
            file_path.assign(backupfile);
            std::ofstream append_file(file_path, std::ios::binary | std::ios::app);
            if (!append_file)
            {
                outinfo.push_back("Restore filechecksum error: file is smaller than a BackupInformation structure!");
                return false;
            }
            append_file.write(reinterpret_cast<const char*>(&info), sizeof(BackupInformation));
            append_file.close();
            return false;
        }
        fs::remove_all(dofile);
        dofile.replace_extension("");
    }

    // 解压
    if (info.mod & MOD_COMPRESS)
    {
        outinfo.push_back("DECOMPRESSING...");
        Compression compression(dofile);
        if (!compression.decompress())
        {
            outinfo.push_back("Error: Failed to decompress file.");
            return false;
        }
        if (info.mod & MOD_ENCRYPT)
            fs::remove_all(dofile);
        dofile.replace_extension("");
    }

    // 解包
    outinfo.push_back("UNPACKING...");
    FilterOptions filter;
    Packer packer(restore_path, dofile, filter);
    if (!packer.Unpack())
    {
        outinfo.push_back("Error: Failed to unpack file.");
        return false;
    }
    if (info.mod & MOD_COMPRESS)
        fs::remove_all(dofile);

    outinfo.push_back("RestorBackup succeed!");

    // 重新写入info
    file_path.assign(backupfile);
    std::ofstream append_file(file_path, std::ios::binary | std::ios::app);
    if (!append_file)
    {
        outinfo.push_back("Restore filechecksum error: file is smaller than a BackupInformation structure!");
        return false;
    }
    append_file.write(reinterpret_cast<const char*>(&info), sizeof(BackupInformation));
    append_file.close();

    return true;
}