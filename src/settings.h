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

    int settings_version;

    DifficultyLevel difficulty;
    SDL_Scancode moveRightKey;
    SDL_Scancode moveLeftKey;
    SDL_Scancode changeShapeUpKey;
    SDL_Scancode changeShapeDownKey;

    bool soundEnabled;
    bool firstRun;

    string theme;
    string lastName;

    void reset();
    void openMenu();
};

#endif /* _settings_h */
