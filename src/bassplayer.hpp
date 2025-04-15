#pragma once

#include <bass.h> 

namespace BASS 
{
    std::string GetVersionStr();
    void BASSError(const char *text, bool isFatal = true);

    class BASSPlayer {
        public:
            // Variables

            // Functions
            static void Init();
            static void Destroy();
            static void StartFilePlayback(const char fPath[]);
            static void StartPausePlayback();
            static void StopPlayback();
            static void StartScroll();
            static void EndScroll();
            static void SetPos(double pos);
            static double GetTrackProgressSecs();
            static std::string GetTrackProgressStr(double pos);

            // Getters & setters.
            static void SetVolume(float vol);
            static float GetVolume();
            static QWORD GetTrackLen();
            static double GetTrackLenSecs();
            static const char *GetTrackLenStr();
            static bool IsPlaying();
        
        private:
            // Varibales
            static DWORD curChannel;
            static QWORD trackLen;
            static std::string trackLenStr;
            static bool isPlaying;
            
            static int deviceIdx;
            static bool restartChannel;
            static float volume;

            // Functions
    };
}