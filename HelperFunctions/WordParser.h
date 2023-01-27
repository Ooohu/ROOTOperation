#include <vector>
#include <string>
#include <iostream>

//Remove special characters in a string
std::string CreateSafeName( std::string in_name){
  std::string safe_name = in_name;
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ' '), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '('), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ')'), safe_name.end());
  safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '\\'), safe_name.end());
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

  return safe_name;
}
