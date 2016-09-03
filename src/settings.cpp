#include "main.h"

bool
Settings::isOK(SDLU_IniHandler* h)
{
	const int num = 4;
	const char *check_strings[] = { "difficulty", "moveRightKey", "moveLeftKey", "changeShapeKey" };
	bool OK = true;

	for (int i = 0; (i < num) && OK; i++) {
		OK = SDLU_GetIniProperty(h, NULL, check_strings[i]) != NULL;
	}

	return OK;
}

Settings::Settings()
{
	char* settingsPath = SDL_GetPrefPath("shapes", "shapes");
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
	}
}

Settings::~Settings()
{
	if (savingSettings) {
		SDLU_IniHandler *h = SDLU_CreateIni();
		SDLU_SetIniProperty(&h, NULL, "moveRightKey", IntToString(moveRightKey));
		SDLU_SetIniProperty(&h, NULL, "moveLeftKey", IntToString(moveLeftKey));
		SDLU_SetIniProperty(&h, NULL, "changeShapeKey", IntToString(changeShapeKey));
		SDLU_SetIniProperty(&h, NULL, "difficulty", IntToString(difficulty));

		SDLU_SaveIni(h, settingsIni.c_str());
	}
}

void
Settings::reset()
{
	moveRightKey = SDL_SCANCODE_RIGHT;
	moveLeftKey = SDL_SCANCODE_LEFT;
	changeShapeKey = SDL_SCANCODE_SPACE;
	difficulty = DifficultyLevel::Medium;
}