#include <gtest/gtest.h>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Packer.h"
#include "Compression.h"
#include "FilterOptions.h"
namespace fs = std::filesystem;

bool check_file_content(const fs::path& path1, const fs::path& path2);
// 测试类，确保在每个测试前后正确清理文件系统
class BackupTest : public ::testing::Test {
protected:
    void SetUp() override {
    // 设置目录和文件路径
    root_path = "/home/why/Backup/BackupSoftware/tests/AFolder";
    dst_path = "/home/why/Backup/BackupSoftware/tests/dst_file";
    pakfile = "/home/why/Backup/BackupSoftware/tests/dst_file/AFolder.pak";
    cmpfile = "/home/why/Backup/BackupSoftware/tests/dst_file/AFolder.pak.cps";
    restore_path = "/home/why/Backup/BackupSoftware/tests/restore";

    // 初始化 FilterOptions 对象
    filter = FilterOptions(); // 使用默认构造函数初始化
    }

    // 在每个测试结束后执行
    void TearDown() override {
        // 清理临时目录
        fs::remove_all(dst_path);
        fs::remove_all(restore_path);
    }

    // 需要测试的路径和 FilterOptions
    std::filesystem::path root_path;
    std::filesystem::path dst_path;
    std::filesystem::path pakfile;
    std::filesystem::path cmpfile;
    std::filesystem::path restore_path;
    FilterOptions filter;
};

// 测试打解包功能
TEST_F(BackupTest, PackUnpackTest) {
    Packer packer(root_path, dst_path, filter);
    
    // 首先执行打包
    EXPECT_TRUE(packer.Pack()) << "Packing failed!";
    
    // 检查打包后的文件是否存在
    EXPECT_TRUE(fs::exists(dst_path)) << "Pack file does not exist after packing!";
    
    // 执行解包
    Packer packer1(restore_path, pakfile, filter);
    EXPECT_TRUE(packer1.Unpack()) << "Unpacking failed!";
    
    // 检查文件是否恢复到原始位置
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "regular_file1.txt")) << "regular_file1.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "regular_file2.txt")) << "regular_file2.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/inside_file.txt")) << "inside_file.txt not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/subfolder/documents.tar")) << "documents.tar not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "folder/subfolder/documents.tar.gz")) << "documents.tar.gz not unpacked!";
    
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
    
    // // 检查设备文件是否恢复
    // EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "block_device_file")) << "block_device_file not unpacked!";
    // EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "char_device_file")) << "char_device_file not unpacked!";
    EXPECT_TRUE(fs::exists(restore_path / root_path.filename() / "my_fifo")) << "my_fifo not unpacked!";
    
    std::cout << "Unpack test completed successfully!" << std::endl;
}

// 测试压缩解压功能
TEST_F(BackupTest, compressDecompressTest) {
    Packer packer(root_path, dst_path, filter);
    
    // 首先执行打包
    EXPECT_TRUE(packer.Pack()) << "Packing failed!";
    
    // 检查打包后的文件是否存在
    EXPECT_TRUE(fs::exists(dst_path)) << "Pack file does not exist after packing!";

    Compression compressor(pakfile);

    // 执行文件压缩
    EXPECT_TRUE(compressor.compress()) << "Compression failed!";

    // 检查压缩后的文件是否存在
    EXPECT_TRUE(fs::exists(cmpfile)) << "Compressed file does not exist after compression!";

    // 对打包文件改名以便后续比较
    fs::path bakfile = pakfile;
    bakfile.replace_extension("bak");
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

// 辅助函数：比较文件内容
bool check_file_content(const fs::path& path1, const fs::path& path2) {
    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1 || !file2) return false;

    // 比较文件内容
    return std::equal(std::istreambuf_iterator<char>(file1), std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(file2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);// 初始化 Google Test 
    return RUN_ALL_TESTS();  // 运行所有测试用例
}