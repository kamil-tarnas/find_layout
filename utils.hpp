#ifndef UTILS_HPP
#define UTILS_HPP


#include <utility>
#include <iostream>


bool OpenFile(std::ifstream& logFile, char* logFilename);

bool isRegexValid(const std::string &regexString);

template <class firstType, class secondType>
bool SortBySecondElem(const std::pair<firstType, secondType> &a, const std::pair<firstType, secondType> &b)
{
    return (a.second > b.second);
}


#endif //#ifndef UTILS_HPP
