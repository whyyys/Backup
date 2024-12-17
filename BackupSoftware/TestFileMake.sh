#!/bin/bash

# 设置工作目录
WORKDIR="/home/why/Backup/BackupSoftware/tests/AFolder"
mkdir -p $WORKDIR

# 进入工作目录
cd $WORKDIR

# 1. 创建普通文件
echo "This is a regular file." > regular_file1.txt
echo "This is another regular file." > regular_file2.txt
mkdir folder
echo "This is a text file inside a folder." > folder/inside_file.txt

# 2. 创建符号链接
ln -s regular_file1.txt symlink_file_1.txt

# 3. 创建硬链接
ln regular_file2.txt hardlink_file_2.txt

# 4. 创建管道文件 (FIFO)
mkfifo my_fifo

# 5. 创建字符设备文件
# 使用 mknod 创建字符设备文件 (模拟)
mknod char_device_file c 100 0

# 6. 创建块设备文件
# 使用 mknod 创建块设备文件 (模拟)
mknod block_device_file b 8 0

# 7. 创建目录
mkdir -p folder/subfolder

# 创建大文件tar包 gz包
tar -cPf folder/subfolder/documents.tar /home/why/Backup/Insruction/
tar -czPf folder/subfolder/documents.tar.gz /home/why/Backup/Insruction/

# 输出提示信息
echo "各种文件已成功创建："
echo "普通文件：regular_file.txt, file1.txt"
echo "符号链接：symlink_file.txt, symlink_file2.txt"
echo "硬链接：hardlink_file.txt, hardlink_file1.txt"
echo "管道文件：my_fifo"
echo "字符设备文件：char_device_file"
echo "块设备文件：block_device_file"
echo "目录：folder, folder/subfolder"
echo "大文件：folder/subfolder/documents.tar, folder/subfolder/documents.tar.gz"
