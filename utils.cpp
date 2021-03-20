#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <regex>


bool OpenFile(std::ifstream& logFile, char* logFilename)
{
  logFile.open(logFilename, std::ios::in);

  if (logFile.is_open())
  {
    std::cout << "'" << logFilename << "' file opened successfully\n";
    return true;
  }
  else
  {
    std::cout << "Cannot open "<< logFilename << "\n";
    return false;
  }
}

// Basically a wrapper to check if the regex is valid
// If throws - then regex syntax isn't valid
bool isRegexValid(const std::string &regexString)
{
    try
    {
        std::regex re(regexString);
    }
    catch (const std::regex_error&)
    {
        return false;
    }
    return true;
}
