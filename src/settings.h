#ifndef _settings_h
#define _settings_h

#include "main.h"

class Settings {
private:
	string settingsIni;

    bool savingSettings;
	bool isOK(SDLU_IniHandler *h);

public:
    Settings();
    ~Settings();

	DifficultyLevel difficulty;
	SDL_Scancode moveRightKey;
	SDL_Scancode moveLeftKey;
	SDL_Scancode changeShapeUpKey;
	SDL_Scancode changeShapeDownKey;

	void reset();
};

#endif /* _settings_h */