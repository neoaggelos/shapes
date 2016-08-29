#include "main.h"

static int StringToInt(string s)
{
	int j = 0;
	for (unsigned int i = 0; i < s.length(); i++) {
		j = 10 * j + s[i] - '0';
	}

	return j;
}

static char* IntToString(int i)
{
	string ret;
	do {
		ret = static_cast<char>(i % 10 + '0') + ret;
	} while (i /= 10);

	return SDL_strdup(ret.c_str());
}

bool
Settings::isOK(SDLU_IniHandler* h)
{
	const int num = 5;
	const char *check_strings[] = { "difficulty", "moveRightKey", "moveLeftKey", "changeShapeUpKey", "changeShapeDownKey" };
	bool OK = true;

	for (int i = 0; (i < num) && OK; i++) {
		OK = SDLU_GetIniProperty(h, NULL, check_strings[i]) != NULL;
	}

	return OK;
}

Settings::Settings()
{
	char* settingsPath = SDL_GetPrefPath("shapes", "shapes");

	if (!settingsPath) {
		savingSettings = false;
		/* TODO warning? */
	}

	settingsIni = string(settingsPath) + "settings.ini";

	SDLU_IniHandler *settings = SDLU_LoadIni(settingsIni.c_str());

	if (!settings || !isOK(settings)) {
		reset();
	}
	else {
		moveRightKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "moveRightKey")));
		moveLeftKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "moveLeftKey")));
		changeShapeUpKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "changeShapeUpKey")));
		changeShapeDownKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "changeShapeDownKey")));
		difficulty = static_cast<DifficultyLevel>(StringToInt(SDLU_GetIniProperty(settings, NULL, "difficulty")));
	}
}

Settings::~Settings()
{
	if (savingSettings) {
		SDLU_IniHandler *h = SDLU_CreateIni();
		SDLU_SetIniProperty(&h, NULL, "moveRightKey", IntToString(moveRightKey));
		SDLU_SetIniProperty(&h, NULL, "moveLeftKey", IntToString(moveLeftKey));
		SDLU_SetIniProperty(&h, NULL, "changeShapeUpKey", IntToString(changeShapeUpKey));
		SDLU_SetIniProperty(&h, NULL, "changeShapeDownKey", IntToString(changeShapeDownKey));
		SDLU_SetIniProperty(&h, NULL, "difficulty", IntToString(difficulty));

		SDLU_SaveIni(h, settingsIni.c_str());
	}
}

void
Settings::reset()
{
	moveRightKey = SDL_SCANCODE_RIGHT;
	moveLeftKey = SDL_SCANCODE_LEFT;
	changeShapeUpKey = SDL_SCANCODE_UP;
	changeShapeDownKey = SDL_SCANCODE_DOWN;
	difficulty = DifficultyLevel::Medium;
}