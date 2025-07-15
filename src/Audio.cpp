#include <cstdlib>
#include <iostream>
#include "Audio.h"

#define NUM_WAVEFORMS 5

Audio::Audio()
{
    bool success = true;
    int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
    if( result < 0 )
    {
        std::cerr << "Unable to open audio: " << SDL_GetError() << "\n";
        success = false;
    }

    result = Mix_AllocateChannels(4);
    if( result < 0 )
    {
        std::cerr << "Unable to allocate mixing channels: " << SDL_GetError() << "\n";
        success = false;
    }

    if (success)
    {
        for( int i = 0; i < NUM_WAVEFORMS; i++ )
        {
            std::string fileName = "res/audio/pickupCoin_" + std::to_string(i + 1) + ".wav";
            Mix_Chunk* chunk = Mix_LoadWAV(fileName.c_str());
            if( chunk == nullptr )
            {
                std::cerr << "Unable to load wave file: " << fileName.c_str() << "\n";
            }
            else
            {
                samples.push_back(chunk);
            }
        }
    }
    mute = !success;
}

Audio::~Audio()
{
    if (samples.size() > 0)
    {
        for(int i = 0; i < samples.size(); i++)
        {
            if (samples[i] != nullptr)
            {
                Mix_FreeChunk(samples[i]);
                samples[i] = nullptr;
            }
        }
    }
    Mix_CloseAudio();
}

void Audio::RandomBeep()
{
    if (samples.size() > 0 && !mute)
    {
        int index = std::rand() % samples.size();
        if (samples[index] != nullptr)
        {
            Mix_PlayChannel(-1, samples[index], 0);
        }
    }
}