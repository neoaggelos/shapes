#ifndef _settings_h
#define _settings_h

#include "main.h"

class Settings {
private:
    SDLU_IniHandler *settings;
    string settingsIni;

    bool savingSettings;

    void setProperty(string section, string prop, int val);

public:
    Settings();

    ~Settings();
    
    DifficultyLevel difficulty();
    SDL_Scancode moveRightKey();
    SDL_Scancode moveLeftKey();
    SDL_Scancode changeShapeUpKey();
    SDL_Scancode changeShapeDownKey();

};

#endif /* _settings_h */