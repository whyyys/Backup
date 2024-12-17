# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
project(BACKUPSOFTWARE)

# 设置C++标准为C++17，并且为强制要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加当前项目的include目录，以便编译器能找到头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

# 查找 src 目录下的所有 .cpp 文件
file(GLOB_RECURSE BACKUP_SOURCES "src/*.cpp")

# 创建 backup_functions 库并添加源文件
add_library(backup_functions ${BACKUP_SOURCES})

# 查找Google Test库，确保系统已安装GTest
find_package(GTest REQUIRED)

# 添加GTest的include目录
include_directories(${GTEST_INCLUDE_DIRS})

# 添加测试源文件，生成一个名为runTests的可执行文件
add_executable(runTests 
    tests/test_packer.cpp
    # tests/test_compression.cpp
    # tests/test_encryption.cpp
)

# 链接静态库, gtest库和pthread库到可执行文件runTests
# gtest框架在实现上使用了多线程（pthread）来管理测试并发执行
target_link_libraries(runTests 
    ${GTEST_LIBRARIES} 
    pthread 
    backup_functions
)

# 启用测试功能
enable_testing()

# 添加一个名为runTests的测试
add_test(NAME runTests COMMAND runTests)