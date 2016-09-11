#ifndef _audiodata_h
#define _audiodata_h

class AudioData {
private:
	map<string, Mix_Chunk*> sounds;
	int channel;
	bool hasAudio;

	void loadWAV(string name);

public:
	AudioData();
	~AudioData();

	void play(string name);
};

#endif /* _audiodata_h */