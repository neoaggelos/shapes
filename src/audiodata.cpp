#include "main.h"

void
AudioData::loadWAV(string filename)
{
	string full = getAssetsDir("wav") + filename + ".wav";

	WAV * wav = new WAV;
	hasAudio &= SDL_LoadWAV(full.c_str(), &(wav->spec), &(wav->data), &(wav->len)) != NULL;

	if (hasAudio)
		sounds[filename] = wav;
}

AudioData::AudioData()
{
	hasAudio = SDL_InitSubSystem(SDL_INIT_AUDIO) != -1;

	loadWAV("bleep");
}

AudioData::~AudioData()
{
	typedef map<string, WAV*>::iterator iter;
	if (hasAudio) {
		SDL_PauseAudio(1);
		SDL_CloseAudio();
		for (iter i = sounds.begin(); i != sounds.end(); i++) {
			SDL_FreeWAV((*i).second->data);
		}
	}

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
		
}

void
AudioData::play(string sound)
{
	if (!hasAudio)
		return;

	if (sound == lastSound) {
		SDL_QueueAudio(1, sounds[sound]->data, sounds[sound]->len);
	}
	else {
		SDL_CloseAudio();

		if (SDL_OpenAudio(&(sounds[sound]->spec), NULL) == 0) {
			SDL_QueueAudio(1, sounds[sound]->data, sounds[sound]->len);
		}

		lastSound = sound;
		SDL_PauseAudio(0);
	}
}