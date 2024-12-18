### 测试文件的目录树结构
由shell脚本./TestFileMake.sh生成
```shell
./tests/AFolder
├── .ignore_folder              # 被过滤的文件夹
│   └── hidden_file.txt
├── block_device_file           # 块设备文件
├── char_device_file            # 字符设备文件
├── folder
│   ├── inside_file.txt
│   └── subfolder
├── hardlink_file_2.txt         # 硬链接
├── ignore.txt                  # 被过滤的普通文件
├── large_folder
│   ├── random_large_file       # 大文件
│   ├── random_large_file.tar   # tar包文件
│   └── random_large_file.tar.gz    # 压缩包文件
├── my_fifo                     # 管道文件
├── regular_file1.txt
├── regular_file2.txt
└── symlink_file_1.txt -> regular_file1.txt # 软连接
```
