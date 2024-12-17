/**
 * Project Untitled
 */

#include "BackupManager.h"

/**
 * BackupManager implementation
 */

BackupManager::BackupManager(std::string src_path_, std::string dst_path_)
{
    if (src_path_.length() == 0)
    {
        src_path = ".";
    }
    src_path = fs::absolute(src_path_);
    dst_path = fs::absolute(dst_path_);
    memset(&info, 0, sizeof(info));
    info.mod = MOD_COMPRESS | MOD_ENCRYPT;
    info.timestamp = std::time(nullptr);
    memcpy(info.backup_path, src_path_.c_str(), std::min(src_path_.length(), sizeof(info.backup_path)));
}

BackupManager::~BackupManager() {}

bool BackupManager::CreateBackup(std::string password)
{
    fs::path ori_dst_path = dst_path;
    // 判断路径是否存在
    if (!fs::exists(src_path))
    {
        outinfo.push_back("Error: No such file or directory: " + src_path.string());
        return false;
    }
    // 判断文件名是否符合要求
    std::string name = src_path.filename();
    std::regex reg("^[.]*[\\w]+[\\w.-]*$");
    if (!std::regex_match(name, reg))
    {
        outinfo.push_back("Error: Incvalid file name: " + name);
        return false;
    }

    // 打包
    outinfo.push_back("PACKING...");
    Packer packer(src_path, dst_path, filter);
    if (!packer.Pack())
    {
        outinfo.push_back("Error: Failed to pack file.");
        return false;
    }
    dst_path = ori_dst_path / FILE_SUFFIX_PACK;

    // 压缩
    if (info.mod & MOD_COMPRESS)
    {
        outinfo.push_back("COMORESSING...");
        Compression compression(dst_path);
        if (!compression.compress())
        {
            outinfo.push_back("Error: Failed to compress file.");
            return false;
        }
        fs::remove_all(dst_path);
        dst_path = ori_dst_path / FILE_SUFFIX_COMPRESS;
    }

    // 加密
    if (info.mod & MOD_ENCRYPT)
    {
        outinfo.push_back("ENCRYPTING...");
        Encryption encryption(dst_path, password);
        if (!encryption.encrypt())
        {
            outinfo.push_back("Error: Failed to encrypt file.");
            return false;
        }
        fs::remove_all(dst_path);
        dst_path = ori_dst_path / FILE_SUFFIX_ENCRYPT;
    }

    // 保存备份信息
    FileInfo file(dst_path);
    file.OpenFile(std::ios::in | std::ios::out | std::ios::binary);
    file.WriteBackupInfo(info);
    file.close();

    outinfo.push_back("Backup succeed!");
    return true;
}

bool BackupManager::RestoreBackup(std::string password)
{
    // 判断路径是否存在
    if (!fs::exists(dst_path))
    {
        outinfo.push_back("Error: No such file or directory: " + dst_path.string());
        return false;
    }

    // 解密
    if (info.mod & MOD_ENCRYPT)
    {
        outinfo.push_back("DECRYPTING...");
        Encryption encryption(dst_path, password);
        int status = encryption.decrypt();
        if (status == -2)
        {
            outinfo.push_back("Error: Failed to decrypt file.");
            return false;
        }
        else if (status == -1)
        {
            outinfo.push_back("Error: Wrong password!");
            return false;
        }
        dst_path.replace_extension("tdc");
    }

    // 解压
    if (info.mod & MOD_COMPRESS)
    {
        outinfo.push_back("DECOMPRESSING...");
        Compression compression(dst_path);
        if (!compression.decompress())
        {
            outinfo.push_back("Error: Failed to decompress file.");
            return false;
        }
        if (info.mod & MOD_ENCRYPT)
            fs::remove_all(dst_path);
        dst_path.replace_extension("tdp");
    }

    // 解包
    outinfo.push_back("UNPACKING...");
    Packer packer(src_path, dst_path, filter);
    if (!packer.Unpack())
    {
        outinfo.push_back("Error: Failed to unpack file.");
        return false;
    }
    if (info.mod & MOD_COMPRESS)
        fs::remove_all(dst_path);

    outinfo.push_back("RestorBackup succeed!");
    return true;
}

std::vector<std::string> BackupManager::Getoutinfo()
{
    return outinfo;
}

void BackupManager::SetComment(std::string comment_)
{
    memset(&(info.comment), 0, sizeof(info.comment));
    memcpy(info.comment, comment_.c_str(), std::min(comment_.length(), sizeof(info.comment)));
}

void BackupManager::SetMod(unsigned char mod_)
{
    info.mod = mod_;
}

void BackupManager::SetFilter(const FilterOptions &filteroptions_)
{
    filter = filteroptions_;
}

int BackupManager::GetBackupInfo()
{
    return BackupManager::GetBackupInfo(this->dst_path, this->info);
}

unsigned char BackupManager::GetBackupMode()
{
    return info.mod;
}

int BackupManager::GetBackupInfo(std::string file_path_, BackupInfo &info_)
{
    FileInfo file(file_path_);
    if (!file.OpenFile(std::ios::in | std::ios::binary))  return -2; // 读取文件失败返回值
    info_ = file.ReadBackupInfo();
    file.close();
    if (!VerifyFileChecksum(file_path_)) // 文件校验失败
    {
        return -1; // 校验错误返回值
    }
        
        return 0; // 成功返回值
}    
bool BackupManager::VerifyFileChecksum(const std::string& file_path) {
    // 打开文件读取内容并计算校验和
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for checksum calculation: " << file_path << std::endl;
        return false;
    }

    uint32_t checksum = 0;
    char buffer[1024];

    // 逐块读取文件并计算校验和
    while (file.read(buffer, sizeof(buffer))) {
        size_t bytes_read = file.gcount();
        for (size_t i = 0; i < bytes_read; ++i) {
            checksum += static_cast<uint32_t>(buffer[i]);
        }
    }

    // 处理最后不足1024字节的数据
    size_t bytes_read = file.gcount();
    for (size_t i = 0; i < bytes_read; ++i) {
        checksum += static_cast<uint32_t>(buffer[i]);
    }

    file.close();

    // 检查文件是否正确
    if (checksum == 0) {
        std::cout << "File checksum is valid." << std::endl;
        return true;
    } else {
        std::cerr << "File checksum is invalid." << std::endl;
        return false;
    }
}
