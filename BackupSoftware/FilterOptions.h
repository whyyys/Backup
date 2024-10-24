#include <ctime>
#include <string>
#include <list>
/**
 * Project Untitled
 */


#ifndef _FILTEROPTIONS_H
#define _FILTEROPTIONS_H

class FilterOptions {
public: 
    std::list<std::string> fileTypes;
    long minFileSize;
    long maxFileSize;
    std::time_t modifiedAfter;
    std::time_t modifiedBefore;
    std::list<std::string> excludePaths;
    std::string includeRegex;
    std::string excludeRegex;
    
FilterOptions();
};

#endif //_FILTEROPTIONS_H