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
            
            // Getters & setters.
            static void SetVolume(float vol);
            static float GetVolume();
        private:
            // Varibales
            static DWORD curChannel;
            
            static int deviceIdx;
            static bool restartChannel;
            static float volume;

            // Functions
    };
}