#ifndef _audiodata_h
#define _audiodata_h

struct WAV {
	SDL_AudioSpec spec;
	Uint8 *data;
	Uint32 len;
};

class AudioData {
private:
	map<std::string, WAV*> sounds;
	bool hasAudio;
	string lastSound;

	void loadWAV(string name);

public:
	AudioData();
	~AudioData();

	void play(string name);
};

#endif /* _audiodata_h */