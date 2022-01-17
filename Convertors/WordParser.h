#include <vector>
#include <string>
#include <iostream>

//Remove special characters in a string
std::string CreateSaveName( std::string in_name){
  std::string save_name = name;
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ' '), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '('), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ')'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '\'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '/'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '['), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ']'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '+'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '-'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '*'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '.'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ','), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '|'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '&'), safe_name.end());

  return names;
}
