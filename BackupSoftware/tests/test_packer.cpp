#include <gtest/gtest.h>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Packer.h"
#include "Compression.h"
#include "Encryption.h"
#include "FilterOptions.h"
#include "Backup_Functions.h"
namespace fs = std::filesystem;

bool check_file_content(const fs::path& path1, const fs::path& path2);
std::string randomPassword(int length);

// 测试类，确保在测试前后正确清理文件系统
class BackupTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
    }

    // 在测试结束后执行
    static void TearDownTestSuite() {
        // 清理临时目录
        fs::remove_all(fs::canonical(fs::current_path() / "../tests") / "dst_file");
        fs::remove_all(fs::canonical(fs::current_path() / "../tests") / "restore");
    }
    void SetUp() override{
        cur = fs::current_path();
        // 拼接 '../test' 路径
        fs::path new_path = cur / "../tests";
        // 获取绝对路径并规范化，消除 '..'
        fs::path absolute_path = fs::canonical(new_path);
        // 设置目录和文件路径
        root_path = absolute_path / "AFolder";
        dst_path = absolute_path / "dst_file";
        pakfile = absolute_path / "dst_file/AFolder.pak";
        cmpfile = absolute_path / "dst_file/AFolder.pak.cps";
        eptfile = absolute_path / "dst_file/AFolder.pak.cps.ept";
        restore_path = absolute_path / "restore";

        // 初始化 FilterOptions 对象
        filter_regex_ = ".*ignore.*";
        
        // 创建解包文件夹
        fs::create_directory(restore_path);
    }
    void TearDown() override{
        fs::current_path(cur);
    }

    // 需要测试的路径和 FilterOptions
    std::filesystem::path root_path;
    std::filesystem::path dst_path;
    std::filesystem::path pakfile;
    std::filesystem::path cmpfile;
    std::filesystem::path eptfile;
    std::filesystem::path restore_path;
    std::filesystem::path cur;
    FilterOptions filter;
    std::string filter_regex_;
};

// 测试打解包功能
TEST_F(BackupTest, PackUnpackTest) {
    Packer packer(root_path, dst_path, filter);
    
    // 首先执行打包
    EXPECT_TRUE(packer.Pack()) << "Packing failed!";
    
    // 检查打包后的文件是否存在
    EXPECT_TRUE(fs::exists(pakfile)) << "Pack file does not exist after packing!";
    
    // 执行解包
    Packer packer1(restore_path, pakfile, filter);
    EXPECT_TRUE(packer1.Unpack()) << "Unpacking failed!";
    // 检查文件是否恢复到原始位置
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "regular_file1.txt")) << "regular_file1.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "regular_file2.txt")) << "regular_file2.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/inside_file.txt")) << "inside_file.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/subfolder")) << "subfolder not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file")) << "random_large_file not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar")) << "random_large_file.tar not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar.gz")) << "random_large_file.tar.gz not unpacked!";

    // 检查软链接是否正确恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "symlink_file_1.txt")) << "symlink_file_1.txt not unpacked!";
    EXPECT_TRUE(fs::is_symlink(restore_path / root_path.filename() / "symlink_file_1.txt")) << "symlink_file_1.txt is not a symlink!";
    EXPECT_EQ(fs::read_symlink(restore_path / root_path.filename() / "symlink_file_1.txt"),
            "regular_file1.txt") << "symlink_file_1.txt does not point to the correct target!";

    // 检查硬链接是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "hardlink_file_2.txt")) << "hardlink_file_2.txt not unpacked!";
    EXPECT_EQ(fs::file_size(restore_path / root_path.filename() / "hardlink_file_2.txt"),
            fs::file_size(restore_path / root_path.filename() / "regular_file2.txt")) << "hardlink_file_2.txt does not point to the same file as regular_file2.txt!";

    // 检查管道文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "my_fifo")) << "my_fifo not unpacked!";
    EXPECT_TRUE(fs::is_fifo(restore_path / root_path.filename() / "my_fifo")) << "my_fifo is not a FIFO!";

    // 检查块设备文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "block_device_file")) << "block_device_file not unpacked!";
    EXPECT_TRUE(fs::is_block_file(restore_path / root_path.filename() / "block_device_file")) << "block_device_file is not a block device!";

    // 检查字符设备文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "char_device_file")) << "char_device_file not unpacked!";
    EXPECT_TRUE(fs::is_character_file(restore_path / root_path.filename() / "char_device_file")) << "char_device_file is not a character device!";

    // 检查压缩包文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar")) << "random_large_file.tar not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar.gz")) << "random_large_file.tar.gz not unpacked!";

    // 输出验证完成的信息
    std::cout << "Unpack test completed successfully!" << std::endl;
}

// 测试压缩解压功能
TEST_F(BackupTest, CompressDecompressTest) {
    Compression compressor(pakfile);

    // 执行文件压缩
    EXPECT_TRUE(compressor.compress()) << "Compression failed!";

    // 检查压缩后的文件是否存在
    EXPECT_TRUE(fs::exists(cmpfile)) << "Compressed file does not exist after compression!";

    // 对打包文件改名以便后续比较
    fs::path bakfile = pakfile;
    bakfile.replace_extension("pbak");
    if (fs::exists(bakfile)) {
        fs::remove(bakfile);  // 删除现有的 bak 文件
    }
    try {
        fs::rename(pakfile, bakfile);
    } catch (const std::exception& e) {
        std::cerr << "Error renaming file: " << e.what() << std::endl;
        FAIL() << "File renaming failed!";
    }

    // 执行解压
    Compression decompressor(cmpfile);
    EXPECT_TRUE(decompressor.decompress()) << "Decompression failed!";

    // 检查解压后的文件是否恢复到原始文件
    EXPECT_TRUE(fs::exists(pakfile)) << "Decompressed file does not exist!";
    
    // 检查文件内容是否一致，假设 `check_file_content()` 是一个用来比较文件内容的函数
    EXPECT_TRUE(check_file_content(pakfile, bakfile)) << "Decompressed file content mismatch!";

    std::cout << "Compression and decompression test completed successfully!" << std::endl;
}

// 测试加密解密功能
TEST_F(BackupTest, EncryptDecryptTest) {
    std::string password = randomPassword(8);
    Encryption encryption(cmpfile, password);

    // 执行文件加密
    EXPECT_TRUE(encryption.encrypt()) << "Encryption failed!";

    // 检查加密后的文件是否存在
    EXPECT_TRUE(fs::exists(eptfile)) << "Encrypted file does not exist after Encryption!";

    // 对压缩文件改名以便后续比较
    fs::path bakfile = cmpfile;
    bakfile.replace_extension("cbak");
    if (fs::exists(bakfile)) {
        fs::remove(bakfile);  // 删除现有的 bak 文件
    }
    try {
        fs::rename(cmpfile, bakfile);
    } catch (const std::exception& e) {
        std::cerr << "Error renaming file: " << e.what() << std::endl;
        FAIL() << "File renaming failed!";
    }

    // 执行解密
    Encryption decryption(eptfile, password);
    EXPECT_EQ(decryption.decrypt(), 0) << "Decryption failed!";

    // 检查解密后的文件是否恢复到原始文件
    EXPECT_TRUE(fs::exists(cmpfile)) << "Decrypted file does not exist!";
    
    // 检查文件内容是否一致，假设 `check_file_content()` 是一个用来比较文件内容的函数
    EXPECT_TRUE(check_file_content(cmpfile, bakfile)) << "Decrypted file content mismatch!";

    // 多次解密检验是否破坏文件
    fs::remove(cmpfile);
    // 执行解密
    Encryption decryption_1(eptfile, password);
    EXPECT_EQ(decryption_1.decrypt(), 0) << "Decryption failed!";

    // 检查解密后的文件是否恢复到原始文件
    EXPECT_TRUE(fs::exists(cmpfile)) << "Decrypted file does not exist!";
    
    // 检查文件内容是否一致，假设 `check_file_content()` 是一个用来比较文件内容的函数
    EXPECT_TRUE(check_file_content(cmpfile, bakfile)) << "Decrypted file content mismatch!";

    std::cout << "Eecryption and decryption test completed successfully!" << std::endl;
}

// 测试整体封装功能
TEST_F(BackupTest, TotalTest) {
    // 清理临时目录
    fs::remove_all("../tests/dst_file");
    fs::remove_all("../tests/restore/AFolder");

    std::string comment = "Test all functions.";
    std::string password = randomPassword(8);
    BackupFunctions task(root_path, dst_path, "", "", comment, password);

    // 设置信息
    task.SetFilter(FILTER_FILE_NAME | FILTER_FILE_TYPE, filter_regex_, FILE_TYPE_SYMBOLIC_LINK, 0, 0, 0, 0, 0, 0);
    task.SetMod(MOD_COMPRESS | MOD_ENCRYPT);
    
    // 首先执行打包
    EXPECT_TRUE(task.CreateBackup()) << "Operate failed!";

    //输出过程信息
    std::cout << "------Pack information------" << std::endl;
    std::vector<std::string> outinfo = task.Getoutinfo();
    for (const auto& str : outinfo) {
        std::cout << str << std::endl;
    }

    // 检查打包后的文件是否存在
    EXPECT_TRUE(fs::exists(eptfile)) << "File does not exist after test all functions!";

    // 测试错误密码
    // 执行解包
    BackupFunctions task_4("", "", restore_path, eptfile, "", "adsfs");
    EXPECT_FALSE(task_4.RestoreBackup()) << "Unpacking failed!";

    //输出过程信息
    std::cout << "-----Unpack information------" << std::endl;
    std::vector<std::string> outinfo_4 = task_4.Getoutinfo();
    for (const auto& str : outinfo_4) {
        std::cout << str << std::endl;
    }

    // 执行解包
    BackupFunctions task_2("", "", restore_path, eptfile, "", password);
    EXPECT_TRUE(task_2.RestoreBackup()) << "Unpacking failed!";

    //输出过程信息
    std::cout << "-----Unpack information------" << std::endl;
    std::vector<std::string> outinfo_2 = task_2.Getoutinfo();
    for (const auto& str : outinfo_2) {
        std::cout << str << std::endl;
    }
    //再次解包查看是否损坏源文件
    fs::remove_all("./AFolder");
    // 执行解包
    BackupFunctions task_3("", "", restore_path, eptfile, "", password);
    EXPECT_TRUE(task_3.RestoreBackup()) << "Unpacking failed!";

    //输出过程信息
    std::cout << "-----Unpack information------" << std::endl;
    std::vector<std::string> outinfo_3 = task_3.Getoutinfo();
    for (const auto& str : outinfo_3) {
        std::cout << str << std::endl;
    }
    // 检查文件是否恢复到原始位置
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "regular_file1.txt")) << "regular_file1.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "regular_file2.txt")) << "regular_file2.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/inside_file.txt")) << "inside_file.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/subfolder")) << "subfolder not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file")) << "random_large_file not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar")) << "random_large_file.tar not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar.gz")) << "random_large_file.tar.gz not unpacked!";

    // 检查过滤器是否设置正确
    EXPECT_FALSE(fs::exists(restore_path / root_path.filename() / "ignore.txt")) << "ignore.txt not ignore!";
    EXPECT_FALSE(fs::exists(restore_path / root_path.filename() / ".ignore_folder")) << ".ignore_folder not ignore!";

    // 检查软链接是否正确恢复
    // EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "symlink_file_1.txt")) << "symlink_file_1.txt not unpacked!";
    // EXPECT_TRUE(fs::is_symlink(restore_path / root_path.filename() / "symlink_file_1.txt")) << "symlink_file_1.txt is not a symlink!";
    // EXPECT_EQ(fs::read_symlink(restore_path / root_path.filename() / "symlink_file_1.txt"),
    //         "regular_file1.txt") << "symlink_file_1.txt does not point to the correct target!";
    EXPECT_FALSE(fs::exists(restore_path / root_path.filename() / "symlink_file_1.txt")) << "symlink_file_1.txt unpacked!";

    // 检查硬链接是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "hardlink_file_2.txt")) << "hardlink_file_2.txt not unpacked!";
    EXPECT_EQ(fs::file_size(restore_path / root_path.filename() / "hardlink_file_2.txt"),
            fs::file_size(restore_path / root_path.filename() / "regular_file2.txt")) << "hardlink_file_2.txt does not point to the same file as regular_file2.txt!";

    // 检查管道文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "my_fifo")) << "my_fifo not unpacked!";
    EXPECT_TRUE(fs::is_fifo(restore_path / root_path.filename() / "my_fifo")) << "my_fifo is not a FIFO!";

    // 检查块设备文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "block_device_file")) << "block_device_file not unpacked!";
    EXPECT_TRUE(fs::is_block_file(restore_path / root_path.filename() / "block_device_file")) << "block_device_file is not a block device!";

    // 检查字符设备文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "char_device_file")) << "char_device_file not unpacked!";
    EXPECT_TRUE(fs::is_character_file(restore_path / root_path.filename() / "char_device_file")) << "char_device_file is not a character device!";

    // 检查压缩包文件是否恢复
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar")) << "random_large_file.tar not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "large_folder/random_large_file.tar.gz")) << "random_large_file.tar.gz not unpacked!";

    // 输出验证完成的信息
    std::cout << "Unpack test completed successfully!" << std::endl;
}

// 辅助函数：比较文件内容
bool check_file_content(const fs::path& path1, const fs::path& path2) {
    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1 || !file2) return false;

    // 比较文件内容
    return std::equal(std::istreambuf_iterator<char>(file1), std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(file2));
}
// 辅助函数：生成随机密码
std::string randomPassword(int length) {
    const std::string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()-_=+[]{}|;:,.<>?";
    std::srand(std::time(0));
    std::string pwd(length, ' ');
    for (char &c : pwd) c = chars[std::rand() % chars.size()];
    return pwd;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);// 初始化 Google Test 
    return RUN_ALL_TESTS();  // 运行所有测试用例
}