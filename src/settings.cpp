#include "main.h"

static int StringToInt(string s)
{
    int j = 0;
    for (unsigned int i = 0; i < s.length(); i++) {
        j = 10 * j + s[i] - '0';
    }

    return j;
}

static string IntToString(int i)
{
    string ret;
    do {
        ret = static_cast<char>(i % 10 + '0') + ret;
    } while (i /= 10);

    return ret;
}

Settings::Settings()
{
    bool firstTime = false;
    char* settingsPath = SDL_GetPrefPath("shapes", "shapes");

    if (!settingsPath) {
        savingSettings = false;
        /* TODO warning? */
    }

    settingsIni = string(settingsPath) + "settings.ini";

    settings = SDLU_CreateIni();
	firstTime = true;
    if (firstTime) {
        SDLU_SetIniProperty(&settings, "controls", "moveRightKey", IntToString(SDL_SCANCODE_RIGHT).c_str());
        SDLU_SetIniProperty(&settings, "controls", "moveLeftKey", IntToString(SDL_SCANCODE_LEFT).c_str());
        SDLU_SetIniProperty(&settings, "controls", "changeShapeUpKey", IntToString(SDL_SCANCODE_UP).c_str());
        SDLU_SetIniProperty(&settings, "controls", "changeShapeDownKey", IntToString(SDL_SCANCODE_DOWN).c_str());
        SDLU_SetIniProperty(&settings, "options", "difficulty", IntToString(DifficultyLevel::Medium).c_str());
	}
    else {
        /* Make sure configuration is OK */
    }
}

Settings::~Settings()
{
    if (savingSettings) {
        SDLU_SaveIni(settings, settingsIni.c_str());
    }
}

DifficultyLevel
Settings::difficulty()
{
    const char *prop = SDLU_GetIniProperty(settings, "options", "difficulty");
	std::cout << "PROP" << prop << std::endl; //DEBUG
	int r = StringToInt(prop);

	std::cout << "Diff: " << r << std::endl; //DEBUG
    return static_cast<DifficultyLevel>(r);
}

SDL_Scancode
Settings::moveRightKey()
{
    int r = StringToInt(SDLU_GetIniProperty(settings, "controls", "moveRightKey"));

    return static_cast<SDL_Scancode>(r);
}

SDL_Scancode
Settings::moveLeftKey()
{
    int r = StringToInt(SDLU_GetIniProperty(settings, "controls", "moveLeftKey"));

    return static_cast<SDL_Scancode>(r);
}

SDL_Scancode
Settings::changeShapeUpKey()
{
    int r = StringToInt(SDLU_GetIniProperty(settings, "controls", "changeShapeUpKey"));

    return static_cast<SDL_Scancode>(r);
}

SDL_Scancode
Settings::changeShapeDownKey()
{
    int r = StringToInt(SDLU_GetIniProperty(settings, "controls", "changeShapeDownKey"));

    return static_cast<SDL_Scancode>(r);
}