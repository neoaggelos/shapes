#include "main.h"

/*
add 1 every time something breaking happens in settings code.
protects from crashes when newer game versions load older version settings
*/
const int CURRENT_SETTINGS_VERSION = 6;

bool
Settings::isOK(SDLU_IniHandler* h)
{
	const int num = 6;
	const char *check_strings[] = { "difficulty", "moveRightKey", "moveLeftKey", "changeShapeUpKey", "changeShapeDownKey", "theme", "lastName", "soundEnabled", "settings_version" };
	bool OK = true;

	for (int i = 0; (i < num) && OK; i++) {
		OK = SDLU_GetIniProperty(h, NULL, check_strings[i]) != NULL;
	}

	if (OK)
		OK = to_int(SDLU_GetIniProperty(h, NULL, "settings_version")) >= CURRENT_SETTINGS_VERSION;

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
		moveRightKey = static_cast<SDL_Scancode>(to_int(SDLU_GetIniProperty(settings, NULL, "moveRightKey")));
		moveLeftKey = static_cast<SDL_Scancode>(to_int(SDLU_GetIniProperty(settings, NULL, "moveLeftKey")));
		changeShapeUpKey = static_cast<SDL_Scancode>(to_int(SDLU_GetIniProperty(settings, NULL, "changeShapeUpKey")));
		changeShapeDownKey = static_cast<SDL_Scancode>(to_int(SDLU_GetIniProperty(settings, NULL, "changeShapeDownKey")));
		difficulty = static_cast<DifficultyLevel>(to_int(SDLU_GetIniProperty(settings, NULL, "difficulty")));
		theme = SDLU_GetIniProperty(settings, NULL, "theme");
		lastName = SDLU_GetIniProperty(settings, NULL, "lastName");
		soundEnabled = to_int(SDLU_GetIniProperty(settings, NULL, "soundEnabled")) == 0 ? false : true;
		settings_version = to_int(SDLU_GetIniProperty(settings, NULL, "settings_version"));
	}

	if (settings)
		SDLU_DestroyIni(settings);
}

Settings::~Settings()
{
	if (savingSettings) {
		SDLU_IniHandler *h = SDLU_CreateIni();
		SDLU_SetIniProperty(&h, NULL, "settings_version", int_to_string(settings_version).c_str());
		SDLU_SetIniProperty(&h, NULL, "moveRightKey", int_to_string((int)moveRightKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "moveLeftKey", int_to_string((int)moveLeftKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "changeShapeUpKey", int_to_string((int)changeShapeUpKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "changeShapeDownKey", int_to_string((int)changeShapeDownKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "difficulty", int_to_string((int)difficulty).c_str());
		SDLU_SetIniProperty(&h, NULL, "theme", theme.c_str());
		SDLU_SetIniProperty(&h, NULL, "lastName", lastName.c_str());
		SDLU_SetIniProperty(&h, NULL, "soundEnabled", int_to_string(static_cast<int>(soundEnabled)).c_str());

		SDLU_SaveIni(h, settingsIni.c_str());
		SDLU_DestroyIni(h);
	}
}

void
Settings::reset()
{
	moveRightKey = SDL_SCANCODE_RIGHT;
	moveLeftKey = SDL_SCANCODE_LEFT;
	changeShapeUpKey = SDL_SCANCODE_UP;
	changeShapeDownKey = SDL_SCANCODE_DOWN;
	difficulty = Medium;
	theme = "Red";
	lastName = "";
	soundEnabled = true;
	settings_version = CURRENT_SETTINGS_VERSION;
}

enum SettingsMenuAction {
	ReadKeysStart = 0,
	ReadRightKey = 1,
	ReadLeftKey = 2,
	ReadShapeUpKey = 3,
	ReadShapeDownKey = 4,
	ReadKeysEnd = 5,
	Reset,
	BackToMenu,
	Quit,
	None
};

static void
callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	const char *name = ((SDLU_Button*)_this)->name;

	if (SDL_strcmp(name, "right") == 0) {
		*static_cast<SettingsMenuAction*>(action) = ReadRightKey;
	}
	else if (SDL_strcmp(name, "left") == 0) {
		*static_cast<SettingsMenuAction*>(action) = ReadLeftKey;
	}
	else if (SDL_strcmp(name, "up") == 0) {
		*static_cast<SettingsMenuAction*>(action) = ReadShapeUpKey;
	}
	else if (SDL_strcmp(name, "down") == 0) {
		*static_cast<SettingsMenuAction*>(action) = ReadShapeDownKey;
	}
	else if (SDL_strcmp(name, "reset") == 0) {
		*static_cast<SettingsMenuAction*>(action) = Reset;
	}
	else if (SDL_strcmp(name, "back") == 0) {
		*static_cast<SettingsMenuAction*>(action) = BackToMenu;
	}
}

void
combo_callback(void *_button, void *_arg)
{
	SDLU_Button *button = (SDLU_Button*)_button;
	gSuper->getAudioData()->play("bleep");
	const char* name = ((SDLU_Button*)_button)->name;

	if (SDL_strcmp(name, "difficulty") == 0) {
		gSuper->getSettings()->difficulty = difficultyLevel(((SDLU_Styles*)(button->content))->title);
	}
	else if (SDL_strncmp(name, "theme", 5) == 0) {
		gSuper->getSettings()->theme = ((SDLU_Styles*)(button->content))->title;
	}
	else if (SDL_strcmp(name, "sound") == 0) {
		gSuper->getSettings()->soundEnabled = SDL_strcmp(((SDLU_Styles*)(button->content))->title, "On") == 0;
	}
}

void
Settings::openMenu()
{
#ifndef __ANDROID__
	SDLU_Button *shapeUpButton, *shapeDownButton, *rightButton, *leftButton;
#endif /* __ANDROID__ */
	SDLU_Button *resetButton, *backButton;
	SDLU_Button *diffButtons[3], *themeButtons[4], *soundButtons[2];
	RenderData* data = gSuper->getRenderData();
	SettingsMenuAction action;

#ifndef __ANDROID__
	rightButton = CreateButton("right", "Change", { 350, 350, 85, 25 }, 15, callback, &action);
	leftButton = CreateButton("left", "Change", { 350, 400, 85, 25 }, 15, callback, &action);
	shapeUpButton = CreateButton("up", "Change", { 350, 450, 85, 25 }, 15, callback, &action);
	shapeDownButton = CreateButton("down", "Change", { 350, 500, 85, 25 }, 15, callback, &action);
#endif /* __ANDROID__ */

	resetButton = CreateButton("reset", "Reset To Defaults", { 40, 560, 180, 35 }, 18, callback, &action);
	backButton = CreateButton("back", "Back To Menu", { 260, 560, 180, 35 }, 18, callback, &action, SDL_SCANCODE_AC_BACK);

	const char* diffs[] = { "Easy", "Medium", "Hard" };
	for (int i = 0; i < 3; i++) {
		int margin = 5;
		int width = (int)((288 - 2 * margin) / 3.0);
		diffButtons[i] = CreateButton("difficulty", diffs[i], { 150 + i * (width+5), 130, width, 25 }, 15, combo_callback, NULL);
	}

	const char* themes[] = { "Red", "Blue", "Cats", "Old" };
	for (int i = 0; i < 4; i++) {
		int margin = 5;
		int width = (int)((288 - 3 * margin) / 4.0);
		themeButtons[i] = CreateButton("theme", themes[i], { 150 + i * (width + 5), 180, width, 25 }, 15, combo_callback, NULL);
	}

	const char* sound[] = { "On", "Off" };
	for (int i = 0; i < 2; i++) {
		int margin = 5;
		int width = (int)((288 - 1 * margin) / 2.0);
		soundButtons[i] = CreateButton("sound", sound[i], { 150 + i * (width + 5), 230, width, 25 }, 15, combo_callback, NULL);
	}
	
	SDL_Event event;
	action = None;
	while (action != BackToMenu && action != Quit) {
		action = None;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gSuper->finish();
			}

			if (action == Reset) {
				gSuper->getSettings()->reset();
				action = None;
			}
#ifndef __ANDROID__
			else if (action >= ReadKeysStart && action <= ReadKeysEnd) {
				/* no point in changing keys for Android */
				event.type = SDL_LASTEVENT;
				SDL_Scancode newKey = SDL_SCANCODE_UNKNOWN;

				while (newKey == SDL_SCANCODE_UNKNOWN) {
					if (SDL_PollEvent(&event)) {
						if (event.type == SDL_QUIT) {
							newKey = SDL_SCANCODE_ESCAPE;
							action = Quit;
						}
						else if (event.type == SDL_KEYDOWN) {
							newKey = event.key.keysym.scancode;
						}
					}
					SDL_Delay(1);
				}

				if (newKey != SDL_SCANCODE_ESCAPE) {
					if (action == ReadRightKey)
						gSuper->getSettings()->moveRightKey = newKey;
					else if (action == ReadLeftKey)
						gSuper->getSettings()->moveLeftKey = newKey;
					else if (action == ReadShapeUpKey)
						gSuper->getSettings()->changeShapeUpKey = newKey;
					else if (action == ReadShapeDownKey)
						gSuper->getSettings()->changeShapeDownKey = newKey;
				}
			}
#endif /* __ANDROID__ */
		}

		Settings *s = gSuper->getSettings();
		SDL_Renderer *target = gSuper->getRenderData()->getRenderer();
		SDL_SetRenderDrawColor(target, 0, 0, 0, 0);
		SDL_RenderClear(target);

		SDL_SetRenderDrawColor(target, 0xff, 0xff, 0xff, 0xff);
		gSuper->getTextRenderer()->write(20, "SETTINGS", { 0, 15, 480, 100 }, Center);
		gSuper->getTextRenderer()->write(20, "General", 5, 70);
		gSuper->getTextRenderer()->write(20, "Controls", 5, 290);

		SDL_RenderDrawLine(target, 180, 50, 300, 50);
		SDL_RenderDrawLine(target, 5, 105, 475, 105);
		SDL_RenderDrawLine(target, 5, 325, 475, 325);

		SDL_SetRenderDrawColor(target, 0xaa, 0xaa, 0xaa, 0xff);

		gSuper->getTextRenderer()->write(18, "Difficulty", 15, 130);
		gSuper->getTextRenderer()->write(18, "Theme", 15, 180);
		gSuper->getTextRenderer()->write(18, "Sound", 15, 230);

		for (int i = 0; i < 3; i++) {
			if (i == gSuper->getSettings()->difficulty - 1)
				((SDLU_Styles*)diffButtons[i]->content)->fill_color = { 30, 30, 30, 0xff };
			else
				((SDLU_Styles*)diffButtons[i]->content)->fill_color = { 100, 100, 100, 0xff };

			SDLU_RenderButton(diffButtons[i]);

		}

		for (int i = 0; i < 4; i++) {
			if (themes[i] == gSuper->getSettings()->theme)
				((SDLU_Styles*)themeButtons[i]->content)->fill_color = { 30, 30, 30, 0xff };
			else
				((SDLU_Styles*)themeButtons[i]->content)->fill_color = { 100, 100, 100, 0xff };

			SDLU_RenderButton(themeButtons[i]);
		}
		
		for (int i = 0; i < 2; i++) {
			if (i == ! gSuper->getSettings()->soundEnabled)
				((SDLU_Styles*)soundButtons[i]->content)->fill_color = { 30, 30, 30, 0xff };
			else
				((SDLU_Styles*)soundButtons[i]->content)->fill_color = { 100, 100, 100, 0xff };

			SDLU_RenderButton(soundButtons[i]);
		}


#ifndef __ANDROID__
		gSuper->getTextRenderer()->write(18, "Move Right", 15, 350);
		gSuper->getTextRenderer()->write(18, SDL_GetScancodeName(s->moveRightKey), { 0, 350, 480, 100 }, Center);
		gSuper->getTextRenderer()->write(18, "Move Left", 15, 400);
		gSuper->getTextRenderer()->write(18, SDL_GetScancodeName(s->moveLeftKey), { 0, 400, 480, 100 }, Center);
		gSuper->getTextRenderer()->write(18, "Change Shape Up", 15, 450);
		gSuper->getTextRenderer()->write(18, SDL_GetScancodeName(s->changeShapeUpKey), { 0, 450, 480, 100 }, Center);
		gSuper->getTextRenderer()->write(18, "Change Shape Down", 15, 500);
		gSuper->getTextRenderer()->write(18, SDL_GetScancodeName(s->changeShapeDownKey), { 0, 500, 480, 100 }, Center);
#else /* if __ANDROID__ */

		const SDL_Rect screenRect = { 30, 350, 120, 160 };
		const SDL_Rect leftRect = { 30, 350, 30, 160 };
		const SDL_Rect rightRect = { 120, 350, 30, 160 };
		const SDL_Rect upRect = { 60, 350, 60, 80 };
		const SDL_Rect downRect = { 60, 430, 60, 80 };

		SDL_SetRenderDrawColor(target, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderDrawRect(target, &screenRect);
		SDL_RenderDrawRect(target, &upRect);
		SDL_RenderDrawRect(target, &downRect);
		SDL_RenderDrawRect(target, &leftRect);
		SDL_RenderDrawRect(target, &rightRect);

		SDL_SetRenderDrawColor(target, 0xaa, 0xaa, 0xaa, 0xff);

		gSuper->getTextRenderer()->write(18, "1", leftRect, Center, Center);
		gSuper->getTextRenderer()->write(18, "2", rightRect, Center, Center);
		gSuper->getTextRenderer()->write(18, "3", upRect, Center, Center);
		gSuper->getTextRenderer()->write(18, "4", downRect, Center, Center);

		gSuper->getTextRenderer()->write(18, "1:", 200, 360);
		gSuper->getTextRenderer()->write(18, "2:", 200, 400);
		gSuper->getTextRenderer()->write(18, "3:", 200, 440);
		gSuper->getTextRenderer()->write(18, "4:", 200, 480);

		gSuper->getTextRenderer()->write(18, "Move Left", { 200, 360, 250, 100 }, Center);
		gSuper->getTextRenderer()->write(18, "Move Right", { 200, 400, 250, 100 }, Center);
		gSuper->getTextRenderer()->write(18, "Change Shape Up", { 200, 440, 250, 100 }, Center);
		gSuper->getTextRenderer()->write(18, "Change Shape Down", { 200, 480, 250, 100 }, Center);
#endif

#ifndef __ANDROID__
		SDLU_RenderButton(shapeUpButton);
		SDLU_RenderButton(shapeDownButton);
		SDLU_RenderButton(leftButton);
		SDLU_RenderButton(rightButton);
#endif /* __ANDROID__ */
		SDLU_RenderButton(backButton);
		SDLU_RenderButton(resetButton);

		SDL_RenderPresent(target);

		SDL_Delay(1);
	}

#ifndef __ANDROID__
	SDLU_DestroyButton(shapeUpButton);
	SDLU_DestroyButton(shapeDownButton);
	SDLU_DestroyButton(rightButton);
	SDLU_DestroyButton(leftButton);
#endif /* __ANDROID__ */
	SDLU_DestroyButton(backButton);
	SDLU_DestroyButton(resetButton);

	for (int i = 0; i < 3; i++) {
		SDLU_DestroyButton(diffButtons[i]);
	}
	for (int i = 0; i < 4; i++) {
		SDLU_DestroyButton(themeButtons[i]);
	}
	for (int i = 0; i < 2; i++) {
		SDLU_DestroyButton(soundButtons[i]);
	}
	if (action == Quit) {
		gSuper->finish();
	}
}