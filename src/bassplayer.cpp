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
    float   	BASSPlayer::volume 			= 1.0;
    int     	BASSPlayer::deviceIdx 		= -1; // -1 = Default Device.
    DWORD   	BASSPlayer::curChannel 		= -1;
    QWORD   	BASSPlayer::trackLen 		= -1;
    std::string BASSPlayer::trackLenStr 	= "0:00";
    bool    	BASSPlayer::restartChannel 	= FALSE;
    bool    	BASSPlayer::isPlaying 		= FALSE;

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
            BASSError("Couldn't init device. (Incorrect deviceIdx?)");
    }

    void BASSPlayer::Destroy()
    {
        if(!BASS_Free())
            BASSError("Couldn't free BASS. (BASS not initialised?)", false);
        else
            printf("Bass Freed\n");
    }

    void BASSPlayer::StartFilePlayback(const char fPath[])
    {
        if(!BASS_ChannelStop(curChannel))
            BASSError("Couldn't stop channel. (No channel set?)", false);
        
        if(!BASS_ChannelFree(curChannel))
            BASSError("Couldn't free channel. (No channel set?)", false);

        curChannel = BASS_StreamCreateFile(FALSE, fPath, 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_PRESCAN);
        BASS_ChannelSetAttribute(curChannel, BASS_ATTRIB_VOL, volume);

	    printf("Playing back path: %s\n", fPath);

        trackLen = BASS_ChannelGetLength(curChannel, BASS_POS_BYTE);
        if(trackLen != -1)
        {
            double totalSecs = BASS_ChannelBytes2Seconds(curChannel, trackLen);
            int mins = (int)(totalSecs / 60.0);
            int secs = (int)totalSecs % 60;
            
			char lenStr[10];
			std::snprintf(lenStr, sizeof(lenStr), "%d:%02d", mins, secs);
			trackLenStr = lenStr;

            printf("Track length: %s (%f seconds)\n", trackLenStr.c_str(), totalSecs);
        } 
        else
        {
			BASSError("Could not get track length. (Invalid file?)", false);
			trackLenStr = "0:00";
		}

		isPlaying = false;
    }

    void BASSPlayer::StartPausePlayback()
    {
        if (BASS_ChannelIsActive(curChannel) == BASS_ACTIVE_STOPPED
		|| BASS_ChannelIsActive(curChannel) == BASS_ACTIVE_PAUSED)
        {
            if(BASS_ChannelPlay(curChannel, FALSE))
            {
                printf("Playing Audio...\n");
                restartChannel = FALSE;
            }
            else
                BASS::BASSError("Couldn't play file. (Incorrect path?)", FALSE);

			isPlaying = true;
        }
        else if(BASS_ChannelIsActive(curChannel) == BASS_ACTIVE_PLAYING)
        {
            if(BASS_ChannelPause(curChannel))
            {
                printf("Pausing Playback.\n");
            }
            else
                BASS::BASSError("Couldn't pause. (No channel set?)", FALSE);

			isPlaying = false;
        }
    }

    void BASSPlayer::StopPlayback()
    {
        if(BASS_ChannelStop(curChannel))
        {
            printf("Stopping Playback.\n");
			BASS_ChannelSetPosition(curChannel, 0, BASS_POS_BYTE);
		}
        else
            BASS::BASSError("Couldn't stop. (No channel set?)", FALSE);

		isPlaying = false;
    }

	void BASSPlayer::StartScroll()
	{
		if(isPlaying)
		{
			if(BASS_ChannelPause(curChannel))
            {
                printf("Pausing Playback.\n");
            }
            else
                BASS::BASSError("Couldn't pause. (No channel set?)", FALSE);
		}
	}

	void BASSPlayer::EndScroll()
	{
		if(isPlaying)
		{
			if(BASS_ChannelPlay(curChannel, restartChannel))
            {
                printf("Playing Audio...\n");
                restartChannel = FALSE;
            }
            else
                BASS::BASSError("Couldn't play file. (Incorrect path?)", FALSE);
		}
	}

	void BASSPlayer::SetPos(double pos)
	{
		QWORD bytePos = BASS_ChannelSeconds2Bytes(curChannel, pos);
		BASS_ChannelSetPosition(curChannel, bytePos, BASS_POS_BYTE);
	}

	double BASSPlayer::GetTrackProgressSecs()	
	{
		return BASS_ChannelBytes2Seconds(
			curChannel, 
			BASS_ChannelGetPosition(curChannel, BASS_POS_BYTE)
		);
	}

	std::string BASSPlayer::GetTrackProgressStr(double pos)
	{
		const char *total = GetTrackLenStr();
		
		char posStr[10];
		char finalVal[25];
		
		if(pos != -1)
        {
            int mins = (int)(pos / 60.0);
            int secs = (int)pos % 60;
            
			std::snprintf(posStr, sizeof(posStr), "%d:%02d", mins, secs);
        } 
        else
        {
			std::snprintf(posStr, sizeof(posStr), "0:00");
		}

		std::snprintf(finalVal, 25, "%s / %s", posStr, total);
		std::string returnVal = finalVal;
		return returnVal;
	}


    // Setters & Getters
    void BASSPlayer::SetVolume(float vol)
    {
        volume = vol;
        printf("Set Volume to: %f\n", vol);    

        BASS_ChannelSetAttribute(curChannel, BASS_ATTRIB_VOL, vol);
    }

	// One liner Setters/Getters
    float BASSPlayer::GetVolume() 				{return volume;}
	QWORD BASSPlayer::GetTrackLen() 			{return trackLen;}
	double BASSPlayer::GetTrackLenSecs()		{return BASS_ChannelBytes2Seconds(curChannel, trackLen);}
	const char *BASSPlayer::GetTrackLenStr() 	{return trackLenStr.c_str();}
	bool BASSPlayer::IsPlaying()				{return isPlaying;}

}