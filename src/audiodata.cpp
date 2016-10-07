#include "main.h"

const int CHANNELS = 8;

void
AudioData::loadWAV(string name)
{
    string file = getAssetsDir("wav") + name + ".wav";

    sounds[name] = Mix_LoadWAV(file.c_str());
    hasAudio = sounds[name] != NULL;
}

AudioData::AudioData()
{
    hasAudio = SDL_InitSubSystem(SDL_INIT_AUDIO) != -1;

    if (hasAudio)
        hasAudio = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0;

    Mix_AllocateChannels(CHANNELS);

    loadWAV("bleep");
    loadWAV("whoosh");

    channel = 0;
}

AudioData::~AudioData()
{
    typedef map<string, Mix_Chunk*>::iterator iter;
    if (hasAudio) {
        Mix_CloseAudio();
        for (iter i = sounds.begin(); i != sounds.end(); i++) {
            Mix_FreeChunk((*i).second);
        }
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void
AudioData::play(string sound)
{
    if (!hasAudio || !gSuper->getSettings()->soundEnabled)
        return;

    Mix_PlayChannel(channel, sounds[sound], 0);
    Mix_Resume(channel);

    channel = ++channel == CHANNELS ? 0 : channel;
}
