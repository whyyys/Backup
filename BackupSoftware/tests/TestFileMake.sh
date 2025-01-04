#!/bin/bash

rm -rf tests/AFolder

# 设置工作目录
WORKDIR="./AFolder"
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
mkdir large_folder

# 创建被过滤的文件夹和文件
mkdir .ignore_folder
echo "This is a text file inside ignore folder." > .ignore_folder/hidden_file.txt
echo "This is a ignore file." > ignore.txt

# 创建大文件tar包 gz包

# 设置文件和目录路径
output_dir="./large_folder"      # 输出文件存放目录
random_file="random_large_file"  # 随机文件名
tar_file="random_large_file.tar"  # tar文件名
gz_file="random_large_file.tar.gz" # tar.gz文件名
file_size="2MB"             # 文件大小，这里设置为 100MB（可以根据需要修改）

# 创建输出目录
mkdir -p "$output_dir"

# 创建一个随机的文件 (100MB 大小)
dd if=/dev/urandom of="$output_dir/$random_file" bs=1M count=2

# 打包成 tar 文件
tar -cf "$output_dir/$tar_file" -C "$output_dir" "$random_file"

# 打包成 tar.gz 文件
tar -czf "$output_dir/$gz_file" -C "$output_dir" "$random_file"

# 输出结果
echo "随机文件创建并打包成功!"
echo "原始文件: $output_dir/$random_file"
echo "tar 包: $output_dir/$tar_file"
echo "tar.gz 包: $output_dir/$gz_file"


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
