// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// self
#include "utils.hpp"

std::string UlongToHex(unsigned long i)
{
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(unsigned long)*2) 
         << std::hex << i;
  return stream.str();
}

std::string HelloWorld()
{
    return "Hello World";
}