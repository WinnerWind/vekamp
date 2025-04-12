#pragma once

#include <bass.h> 

namespace BASSHelpers 
{
    std::string GetVersionStr();
    void BASSError(const char *text);
}