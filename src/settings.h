#ifndef _settings_h
#define _settings_h

#include "main.h"

class Settings {
private:
	Super *parent;
	string settingsIni;

    bool savingSettings;
	bool isOK(SDLU_IniHandler *h);

public:
    Settings(Super *super);
    ~Settings();

	int settings_version;

	DifficultyLevel difficulty;
	SDL_Scancode moveRightKey;
	SDL_Scancode moveLeftKey;
	SDL_Scancode changeShapeKey;

	string theme;

	void reset();
	void openMenu();
};

#endif /* _settings_h */