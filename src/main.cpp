// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <conio.h>

// vekamp
#include "utils.hpp"

// libs and lib helpers
#include "basshelpers.hpp"


int main(int argc, char *argv[])
{
    printf("Using BASS Version %s\n", BASSHelpers::GetVersionStr().c_str());

    // Check the correct BASS version was loaded.
	if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
		printf("An incorrect version of BASS was loaded\n");
		return 0;
	}

	int deviceIdx = -1;
    DWORD BASSChannel;

    printf("Path: %s\n", argv[1]);

    // Attempts initialisation on default device. 
    if(!BASS_Init(deviceIdx, 48000, 0, 0, NULL))
        BASSHelpers::BASSError("Couldn't init device.");

    BASSChannel = BASS_StreamCreateFile(FALSE, argv[1], 0, 0, BASS_SAMPLE_FLOAT);

    if(BASS_ChannelPlay(BASSChannel, FALSE))
        printf("Playing Audio...");
    else
        BASSHelpers::BASSError("Couldn't play file.");

    while(!_kbhit() && BASS_ChannelIsActive(BASSChannel))
    {
        
    }

    BASS_Free();
    return 0;
}