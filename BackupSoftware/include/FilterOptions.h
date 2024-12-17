/**
 * Project Untitled
 */


#ifndef INCLUDE_FILTEROPTIONS_H
#define INCLUDE_FILTEROPTIONS_H

#include <ctime>
#include <string>
#include <list>
#include <regex>
#include "FileInfo.h"

// 根据路径/类型/名字/时间筛选备份文件
#define FILTER_FILE_PATH 1
#define FILTER_FILE_NAME 2
#define FILTER_FILE_TYPE 4
#define FILTER_FILE_ACCESS_TIME 8
#define FILTER_FILE_MODIFY_TIME 16
#define FILTER_FILE_CHANGE_TIME 32

class FilterOptions {
private:
    unsigned char type;
    std::string reg_path; //过滤文件路径的正则表达式
    std::string reg_name; //过滤文件名的正则表达式
    FileType file_type; //过滤文件类型

    //过滤文件时间
    time_t atime_start, atime_end;
    time_t mtime_start, mtime_end;
    time_t ctime_start, ctime_end;

public: 
    FilterOptions();
    ~FilterOptions();

    void SetPathFilter(std::string reg_path_);
    void SetNameFilter(std::string reg_name_);
    void SetFileType(FileType file_type_);
    void SetAccessTime(time_t atime_start_, time_t atime_end_);
    void SetModifyTime(time_t mtime_start_, time_t mtime_end_);
    void SetChangeTime(time_t ctime_start_, time_t ctime_end_);

    // 判定文件是否满足给定的过滤规则
    bool Check(const FileHeader &file_header_);
};

#endif //INCLUDE_FILTEROPTIONS_H