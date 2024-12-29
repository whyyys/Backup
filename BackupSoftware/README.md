# BACKUP_SOFTWARE BACK-END
## 概述
该项目提供了一款备份解决方案，具备文件加密、压缩和备份管理等功能。

此目录为备份软件后端文件。

它支持操作如读取备份信息、加密/解密文件，以及管理各种文件类型。核心功能使用 C++ 实现，利用 AES 加密来保护备份文件的安全性。项目使用 CMake 构建，以确保跨平台兼容性。

## 项目结构
``` shell
.
├── CMakeLists.txt          # CMake 构建配置文件
├── README.md               # 项目概述和文档
├── TestFileMake.sh         # 创建测试文件的脚本
├── include                 # 项目头文件目录
│   ├── Backup_Functions.h  # 备份功能的头文件
│   ├── Compression.h       # 压缩工具的头文件
│   ├── Encryption.h        # 加密功能的头文件
│   ├── FileInfo.h          # 文件信息的头文件
│   ├── FilterOptions.h     # 过滤选项的头文件
│   └── Packer.h            # 打包功能的头文件
├── src                     # 源代码目录
│   ├── Backup_Functions.cpp # 备份功能的源代码
│   ├── Compression.cpp      # 压缩功能的源代码
│   ├── Encryption.cpp       # 加密功能的源代码
│   ├── FileInfo.cpp         # 文件信息处理的源代码
│   ├── FilterOptions.cpp    # 过滤选项的源代码
│   └── Packer.cpp           # 打包功能的源代码
└── tests                   # 测试文件目录
    ├── AFolder             # 测试文件夹，包含各种测试文件
    └── test_packer.cpp     # 备份功能测试代码
```
## 主要功能
### 1. 备份功能
该项目能够创建备份文件，支持压缩和加密。通过调用 BackupFunctions 类中的方法，用户可以设置备份文件的路径和描述信息，进行压缩和加密等操作。
### 2. 打包功能
支持将多个文件打包为一个压缩文件，以便于管理和存储。用户可以选择需要打包的文件，并通过 Packer 类打包成一个压缩包。
### 3. 文件压缩
支持压缩文件，通过 Compression 类提供的接口，用户可以选择压缩选项进行文件压缩，以节省存储空间。
### 4. 文件加密与解密
使用 AES 加密算法对备份文件进行加密与解密。加密过程使用 128 位密钥，支持 CBC 模式。加密文件的后缀为 .ept，解密时可以恢复为原始文件。
### 5. 文件过滤选项
提供多种过滤选项来选择需要备份的文件类型。过滤选项支持文件名、访问时间、修改时间和更改时间的范围设置。

## 依赖
* C++17 或更高版本
* CMake 9.4.0 或更高版本
* OpenSSL库（用于 AES 加密和解密）

## 后端库构建步骤

``` bash
mkdir build
cd ./build

cmake ..
make

# 运行测试
# 因为涉及到创建字符设备和块设备等特殊文件需要提权
sudo ./runTest
```
> 生成后端库文件路径为`./build/libbackup_functions.a`

> 头文件路径为`./include/Backup_Functions.h`

## 使用说明
通过 BackupFunctions 类进行备份，设置备份文件的路径和描述信息，并选择是否加密或压缩。

具体代码细节可参考`tests/test_packer.cpp`






