// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// vekamp
#include "utils.hpp"

// libs and lib helpers
#include "basshelpers.hpp"


int main(int argc, char *argv[])
{
    std::cout << "Using BASS Version " << BASSHelpers::GetVersionStr() << std::endl;

    // check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
		std::cout << "An incorrect version of BASS was loaded";
		return 0;
	}

    std::cout << HelloWorld();

    return 0;
}