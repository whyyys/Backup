# BackUP
## Introduce

本项目包含了 **备份应用程序** 和相关软件组件的源代码。该应用程序用于管理备份及相关任务。

## 项目结构

项目结构如下：

### 1. **BackupApplication**

此文件夹包含了由**Qt6**构建的主要的应用程序代码和GUI界面文件。

- `build/`：包含不同的构建配置（例如，调试、发布）。
  - `Desktop_Qt_6_5_3_GCC_64bit-Debug`：调试构建目录。
  - `Desktop_Qt_6_5_3_GCC_64bit-Release`：发布构建目录。
- `include/`：应用程序的头文件。
- `libs/`：应用程序所需的外部库。

### 2. **BackupSoftware**

此文件夹包含了备份软件的核心逻辑和组件。

- `include/`：备份软件的头文件。
- `src/`：备份逻辑和功能的源文件。
- `tests/`：包含用于验证软件功能的测试用例。
  - `AFolder/`：用于测试的文件夹。

### 3. **BackUpApp-x86_64.AppImage**

打包好的**可执行文件**

一般情况下，如果你在Ubuntu 20.04 LTS系统并安装了符合版本的GCC/C++，**直接运行此程序即可**。

#### 如何使用此文件

1. **确保文件具有执行权限**： 如果你下载的 `BackUpApp-x86_64.AppImage` 文件没有执行权限，你需要给它添加执行权限。可以在终端中运行以下命令：

   ```bash
   chmod +x BackUpApp-x86_64.AppImage
   ```

   这将使 `.AppImage` 文件变为可执行文件。

2. **运行 `.AppImage` 文件**： 现在你可以直接运行 `.AppImage` 文件来启动应用程序：

   ```bash
   ./BackUpApp-x86_64.AppImage
   ```

   如果你在文件浏览器中，双击该文件也会启动应用程序。

#### 如果报错

```bash
./BackUpApp-x86_64.AppImage: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version GLIBCXX_3.4.26' not found (required by ./BackUpApp-x86_64.AppImage)
./BackUpApp-x86_64.AppImage: /lib/x86_64-linux-gnu/libc.so.6: version GLIBC_2.28' not found (required by /tmp/.mount_BackUptoYBPb/lib/libQt6Core.so.6)
./BackUpApp-x86_64.AppImage: /lib/x86_64-linux-gnu/libc.so.6: version GLIBC_2.28' not found (required by /tmp/.mount_BackUptoYBPb/lib/libglib-2.0.so.0)
./BackUpApp-x86_64.AppImage: /lib/x86_64-linux-gnu/libc.so.6: version GLIBC_2.28' not found (required by /tmp/.mount_BackUptoYBPb/lib/libsystemd.so.0)
./BackUpApp-x86_64.AppImage: /lib/x86_64-linux-gnu/libc.so.6: version GLIBC_2.30' not found (required by /tmp/.mount_BackUptoYBPb/lib/libsystemd.so.0)
```

错误表明你的系统缺少一些必要的库版本，尤其是 `GLIBC` 和 `libstdc++` 的特定版本。

- **GLIBCXX_3.4.26** 没有找到，需要更新到 `libstdc++.so.6`
- **GLIBC_2.28** 和 **GLIBC_2.30** 没有找到，需要更新到 `libc.so.6`

## 安装和配置

要从头配置项目，请按照以下步骤操作：

1. 克隆仓库：

   ```bash
   git clone https://github.com/whyyys/Backup.git
   ```

2. 安装依赖项：

项目使用 **Qt** 进行开发，确保你已安装 Qt 6.5.3 或更高版本。可以从官方 Qt 网站下载并安装。

项目后端使用C++构建，确保你已安装GCC/G++，并支持C++ 17标准。

3. 构建项目：

要构建项目，请在项目`BackupSoftware`目录下运行以下命令：

```bash
cd BackupSoftware
mkdir build
cd build
cmake ..
make
```

4. 测试

测试用例位于 `BackupSoftware/tests` 目录中。构建项目后，可以通过在`build`目录下运行测试。

```bash
sudo ./runTests
```

5. 前端构建

前端代码位于`BackupApplication`文件夹中，请使用Qt Creator 6 打开`BackupApplication/CMakeLists.txt`文件并进行后续编辑。

6. 库导入

将刚刚后端生成的库及接口文件放到前端文件里，如下：

```bash
cp BackupSoftware/build/libbackup_functions.a BackupApplication/libs
cp BackupSoftware/include/Backup_Functions.h BackupApplication/include
```

7. 构建项目

继续在Qt 6 中完成后续项目的构建以及发布

## 测试文件

运行`TestFileMake.sh`脚本来生成测试文件，注意sudo运行

生成的测试文件目录如下，覆盖到各种文件类型

```bash
.
├── block_device_file
├── char_device_file
├── folder
│   ├── inside_file.txt
│   └── subfolder
├── hardlink_file_2.txt
├── ignore.txt
├── large_folder
│   ├── random_large_file
│   ├── random_large_file.tar
│   └── random_large_file.tar.gz
├── my_fifo
├── regular_file1.txt
├── regular_file2.txt
└── symlink_file_1.txt -> regular_file1.txt
```

