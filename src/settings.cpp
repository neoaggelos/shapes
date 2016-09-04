#include "main.h"

const int CURRENT_SETTINGS_VERSION = 3;

bool
Settings::isOK(SDLU_IniHandler* h)
{
	const int num = 6;
	const char *check_strings[] = { "difficulty", "moveRightKey", "moveLeftKey", "changeShapeKey", "theme", "settings_version" };
	bool OK = true;

	for (int i = 0; (i < num) && OK; i++) {
		OK = SDLU_GetIniProperty(h, NULL, check_strings[i]) != NULL;
	}

	if (OK)
		OK = StringToInt(SDLU_GetIniProperty(h, NULL, "settings_version")) >= CURRENT_SETTINGS_VERSION;

	return OK;
}

Settings::Settings()
{
	const char* settingsPath = getSettingsDir();
	savingSettings = settingsPath != NULL;

	settingsIni = string(settingsPath) + "settings.ini";

	SDLU_IniHandler *settings = SDLU_LoadIni(settingsIni.c_str());

	if (!settings || !isOK(settings)) {
		reset();
	}
	else {
		moveRightKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "moveRightKey")));
		moveLeftKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "moveLeftKey")));
		changeShapeKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "changeShapeKey")));
		difficulty = static_cast<DifficultyLevel>(StringToInt(SDLU_GetIniProperty(settings, NULL, "difficulty")));
		theme = SDLU_GetIniProperty(settings, NULL, "theme");
		settings_version = StringToInt(SDLU_GetIniProperty(settings, NULL, "settings_version"));
	}
}

Settings::~Settings()
{
	if (savingSettings) {
		SDLU_IniHandler *h = SDLU_CreateIni();
		SDLU_SetIniProperty(&h, NULL, "settings_version", IntToString(settings_version));
		SDLU_SetIniProperty(&h, NULL, "moveRightKey", IntToString(moveRightKey));
		SDLU_SetIniProperty(&h, NULL, "moveLeftKey", IntToString(moveLeftKey));
		SDLU_SetIniProperty(&h, NULL, "changeShapeKey", IntToString(changeShapeKey));
		SDLU_SetIniProperty(&h, NULL, "difficulty", IntToString(difficulty));
		SDLU_SetIniProperty(&h, NULL, "theme", theme.c_str());

		SDLU_SaveIni(h, settingsIni.c_str());
	}
}

void
Settings::reset()
{
	moveRightKey = SDL_SCANCODE_RIGHT;
	moveLeftKey = SDL_SCANCODE_LEFT;
	changeShapeKey = SDL_SCANCODE_SPACE;
	difficulty = Medium;
	theme = "Red";
	settings_version = CURRENT_SETTINGS_VERSION;
}