#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <vector>
#include <SDL_mixer.h>

class Audio
{
public:
    Audio();
    ~Audio();
    void RandomBeep();
    void Mute() { mute = true; }
    void Unmute() { mute = false; }
    void ToggleMute() { mute = !mute; }
private:
    bool mute;
    std::vector<Mix_Chunk*> samples;
};

#endif