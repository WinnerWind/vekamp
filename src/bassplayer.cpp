// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>

// vekamp
#include "utils.hpp"

// libs and lib helpers
#include "bassplayer.hpp"

namespace BASS
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

    void BASSError(const char *text, bool isFatal)
    {
        printf("BASS Error(%d): %s\n", BASS_ErrorGetCode(), text);
        
        if(isFatal)
        {
            BASS_Free();
            exit(0);
        }
    }

    // BASSPlayer Class.
    float   BASSPlayer::volume = 1.0;
    int     BASSPlayer::deviceIdx = -1; // -1 = Default Device.
    DWORD   BASSPlayer::curChannel = 0;
    bool    BASSPlayer::restartChannel = FALSE;

    void BASSPlayer::Init()
    {
        //Print out version.
        printf("Using BASS Version %s\n", BASS::GetVersionStr().c_str());
        
        // Check the correct BASS version was loaded.
        if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
            printf("An incorrect version of BASS was loaded.\n");
            exit(0);
        }

        // Attempts initialisation on default device. 
        if(!BASS_Init(deviceIdx, 48000, 0, 0, NULL))
            BASSError("Couldn't init device. (Incorrect deviceIdx?)\n");
    }

    void BASSPlayer::Destroy()
    {
        if(!BASS_Free())
            BASSError("Couldn't free BASS. (BASS not initialised?)\n", false);
        else
            printf("Bass Freed\n");
    }

    void BASSPlayer::StartFilePlayback(const char fPath[])
    {
        if(!BASS_ChannelStop(curChannel))
            BASSError("Couldn't stop channel. (No channel set?)\n", false);
        
        if(!BASS_ChannelFree(curChannel))
            BASSError("Couldn't free channel. (No channel set?)\n", false);

        curChannel = BASS_StreamCreateFile(FALSE, fPath, 0, 0, BASS_SAMPLE_FLOAT);
        BASS_ChannelSetAttribute(curChannel, BASS_ATTRIB_VOL, volume);
    }

    void BASSPlayer::StartPausePlayback()
    {
        if (BASS_ChannelIsActive(curChannel) == BASS_ACTIVE_STOPPED
		|| BASS_ChannelIsActive(curChannel) == BASS_ACTIVE_PAUSED)
        {
            if(BASS_ChannelPlay(curChannel, restartChannel))
            {
                printf("Playing Audio...\n");
                restartChannel = FALSE;
            }
            else
                BASS::BASSError("Couldn't play file. (Incorrect path?)\n", FALSE);
        }
        else if(BASS_ChannelIsActive(curChannel) == BASS_ACTIVE_PLAYING)
        {
            if(BASS_ChannelPause(curChannel))
            {
                printf("Pausing Playback.\n");
            }
            else
                BASS::BASSError("Couldn't pause. (No channel set?)\n", FALSE);
        }
    }

    void BASSPlayer::StopPlayback()
    {
        if(BASS_ChannelStop(curChannel))
        {
            printf("Stopping Playback.\n");
            restartChannel = TRUE;
        }
        else
            BASS::BASSError("Couldn't stop. (No channel set?)\n", FALSE);
    }

    // Setters & Getters
    void BASSPlayer::SetVolume(float vol)
    {
        volume = vol;
        if(BASS_ChannelSetAttribute(curChannel, BASS_ATTRIB_VOL, vol))
	        printf("Set Volume to: %f\n", vol);    
        else
            BASS::BASSError("Couldn't set volume. (No channel set?)\n", false);
    }

    float BASSPlayer::GetVolume() {return volume;}

}