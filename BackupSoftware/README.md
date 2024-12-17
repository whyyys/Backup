### 测试文件的目录树结构
由shell脚本./TestFileMake.sh生成
```shell
./tests/AFolder
├── block_device_file           # 块设备文件
├── char_device_file            # 字符设备文件
├── folder 
│   ├── inside_file.txt
│   └── subfolder
│       ├── documents.tar       # tar包文件
│       └── documents.tar.gz    # 压缩包文件
├── hardlink_file_2.txt         # 硬链接
├── my_fifo                     # 管道文件
├── regular_file1.txt
├── regular_file2.txt
└── symlink_file_1.txt -> regular_file1.txt # 软连接
```