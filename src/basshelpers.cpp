// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>

// vekamp
#include "utils.hpp"

// libs and lib helpers
#include "basshelpers.hpp"

namespace BASSHelpers
{
    std::string GetVersionStr()
    {
        std::string hexVer = UlongToHex(BASS_GetVersion());
        
        // example format = 0x02041100 = 2.4.11.
        std::string verNums[] = {
            hexVer.substr(2,2),
            hexVer.substr(4,2),
            hexVer.substr(6,2),
            hexVer.substr(8,2),
        };

        for(int i = 0; i < std::size(verNums); i++)
        {
            std::string str = verNums[i];
            
            if(str[0] == '0')
            {
                str = str.substr(1,1);
            }

            verNums[i] = str;
        }

        std::string verString = verNums[0] + '.' + verNums [1] + '.' + verNums[2] + '.' + verNums[3];

        return verString;
    }

    void BASSError(const char *text)
    {
        printf("BASS Error(%d): %s\n", BASS_ErrorGetCode(), text);
        BASS_Free();
        exit(0);
    }
}