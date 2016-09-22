#include "main.h"

/*
add 1 every time something breaking happens in settings code.
protects from crashes when newer game versions load older version settings
*/
const int CURRENT_SETTINGS_VERSION = 5;

bool
Settings::isOK(SDLU_IniHandler* h)
{
	const int num = 6;
	const char *check_strings[] = { "difficulty", "moveRightKey", "moveLeftKey", "changeShapeUpKey", "changeShapeDownKey", "theme", "lastName", "settings_version" };
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
		settings_version = to_int(SDLU_GetIniProperty(settings, NULL, "settings_version"));
	}

	if (settings)
		SDLU_DestroyIni(settings);
}

Settings::~Settings()
{
	if (savingSettings) {
		SDLU_IniHandler *h = SDLU_CreateIni();
		SDLU_SetIniProperty(&h, NULL, "settings_version", to_string(settings_version).c_str());
		SDLU_SetIniProperty(&h, NULL, "moveRightKey", to_string((int)moveRightKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "moveLeftKey", to_string((int)moveLeftKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "changeShapeUpKey", to_string((int)changeShapeUpKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "changeShapeDownKey", to_string((int)changeShapeDownKey).c_str());
		SDLU_SetIniProperty(&h, NULL, "difficulty", to_string((int)difficulty).c_str());
		SDLU_SetIniProperty(&h, NULL, "theme", theme.c_str());
		SDLU_SetIniProperty(&h, NULL, "lastName", lastName.c_str());

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
	settings_version = CURRENT_SETTINGS_VERSION;
}

static SDLU_Button* new_button(RenderData *data, const char *title, int x, int y, int w = -1, int h = -1, int fontsize = 15)
{
	SDLU_Button *ret = SDLU_CreateButton(data->getWindow(), title, SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(ret, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(ret, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(ret, SDLU_HOVER_CALLBACK, on_hover, NULL);

	((SDLU_Styles*)ret->content)->font_size = fontsize;
	SDLU_SetButtonGeometry(ret, x, y, w, h);

	return ret;
}

static SDLU_ComboBox* new_cbox(RenderData *data, int x, int y, int w = -1, int h = -1)
{
	SDLU_ComboBox* ret = SDLU_CreateComboBox(data->getWindow());

	SDLU_SetComboBoxGeometry(ret, x, y, w, h);
	ret->styles->font_size = 15;

	return ret;
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

#ifndef __ANDROID__
static void
right_callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*static_cast<SettingsMenuAction*>(action) = ReadRightKey;
}

static void
left_callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*static_cast<SettingsMenuAction*>(action) = ReadLeftKey;
}

static void
shapeup_callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*static_cast<SettingsMenuAction*>(action) = ReadShapeUpKey;
}

static void
shapedown_callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*static_cast<SettingsMenuAction*>(action) = ReadShapeDownKey;
}
#endif /* __ANDROID__ */

static void
reset_callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*static_cast<SettingsMenuAction*>(action) = Reset;
}

static void
back_callback(void *_this, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*static_cast<SettingsMenuAction*>(action) = BackToMenu;
}

void
Settings::openMenu()
{
#ifndef __ANDROID__
	SDLU_Button *shapeUpButton, *shapeDownButton, *rightButton, *leftButton;
#endif /* __ANDROID__ */
	SDLU_Button *resetButton, *backButton;
	SDLU_ComboBox *diffBox, *themeBox;
	RenderData* data = gSuper->getRenderData();
	SettingsMenuAction action;

#ifndef __ANDROID__
	rightButton = new_button(data, "Change", 350, 350, 85, 25);
	SDLU_SetButtonCallback(rightButton, SDLU_PRESS_CALLBACK, right_callback, &action);

	leftButton = new_button(data, "Change", 350, 400, 85, 25);
	SDLU_SetButtonCallback(leftButton, SDLU_PRESS_CALLBACK, left_callback, &action);

	shapeUpButton = new_button(data, "Change", 350, 450, 85, 25);
	SDLU_SetButtonCallback(shapeUpButton, SDLU_PRESS_CALLBACK, shapeup_callback, &action);

	shapeDownButton = new_button(data, "Change", 350, 500, 85, 25);
	SDLU_SetButtonCallback(shapeDownButton, SDLU_PRESS_CALLBACK, shapedown_callback, &action);
#endif /* __ANDROID__ */

	resetButton = new_button(data, "Reset To Default", 40, 560, 180, 35, 18);
	SDLU_SetButtonCallback(resetButton, SDLU_PRESS_CALLBACK, reset_callback, &action);

	backButton = new_button(data, "Back To Menu", 260, 560, 180, 35, 18);
	SDLU_SetButtonCallback(backButton, SDLU_PRESS_CALLBACK, back_callback, &action);
	SDLU_SetButtonHotkey(backButton, SDL_SCANCODE_AC_BACK);

	themeBox = new_cbox(data, 350, 130, 85, 25);
	SDLU_AddComboBoxItem(&themeBox, "Red");
	SDLU_AddComboBoxItem(&themeBox, "Cats");
	SDLU_AddComboBoxItem(&themeBox, "Blue");
	SDLU_AddComboBoxItem(&themeBox, "Old");
	SDLU_SetComboBoxActiveItem(themeBox, theme.c_str());

	diffBox = new_cbox(data, 350, 240, 85, 25);
	SDLU_AddComboBoxItem(&diffBox, "Easy");
	SDLU_AddComboBoxItem(&diffBox, "Medium");
	SDLU_AddComboBoxItem(&diffBox, "Hard");
	SDLU_SetComboBoxActiveIndex(diffBox, gSuper->getSettings()->difficulty);

	SDL_Event event;
	action = None;
	while (action != BackToMenu && action != Quit) {
		action = None;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gSuper->finish();
			}
			else if (event.type == SDLU_COMBOBOX_OPENED) {
				//				no need to know which combo box was opened, just play the sound
				//				Uint32 cbox_id = static_cast<Uint32>(event.user.code);
				gSuper->getAudioData()->play("bleep");
			}
			else if (event.type == SDLU_COMBOBOX_CHANGED) {
				Uint32 cbox_id = static_cast<Uint32>(event.user.code);
				if (cbox_id == diffBox->id) {
					gSuper->getAudioData()->play("bleep");
					DifficultyLevel new_diff = static_cast<DifficultyLevel>(diffBox->current_index);
					gSuper->getSettings()->difficulty = new_diff;
				}
				else if (cbox_id == themeBox->id) {
					gSuper->getAudioData()->play("bleep");
					gSuper->getSettings()->theme = SDL_strdup(themeBox->current);
					gSuper->getRenderData()->reloadTexture(gSuper->getSettings()->theme);
				}
			}

			if (action == Reset) {
				gSuper->getSettings()->reset();
				SDLU_SetComboBoxActiveIndex(diffBox, gSuper->getSettings()->difficulty);
				SDLU_SetComboBoxActiveItem(themeBox, gSuper->getSettings()->theme.c_str());
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
					SDL_Delay(10);
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
		gSuper->getTextRenderer()->write(20, "Theme", 5, 70);
		gSuper->getTextRenderer()->write(20, "Difficulty", 5, 180);
		gSuper->getTextRenderer()->write(20, "Controls", 5, 290);

		SDL_RenderDrawLine(target, 180, 50, 300, 50);
		SDL_RenderDrawLine(target, 5, 105, 475, 105);
		SDL_RenderDrawLine(target, 5, 215, 475, 215);
		SDL_RenderDrawLine(target, 5, 325, 475, 325);

		SDL_SetRenderDrawColor(target, 0xaa, 0xaa, 0xaa, 0xff);

		gSuper->getTextRenderer()->write(18, "Choose Theme", 15, 130);
		gSuper->getTextRenderer()->write(18, themeBox->current, { 0, 130, 480, 100 }, Center);

		gSuper->getTextRenderer()->write(18, "Choose Difficulty", 15, 240);
		gSuper->getTextRenderer()->write(18, diffBox->current, { 0, 240, 480, 100 }, Center);

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
		SDLU_RenderComboBox(themeBox);
		SDLU_RenderComboBox(diffBox);

		SDL_RenderPresent(target);

		SDL_Delay(10);
	}

#ifndef __ANDROID__
	SDLU_DestroyButton(shapeUpButton);
	SDLU_DestroyButton(shapeDownButton);
	SDLU_DestroyButton(rightButton);
	SDLU_DestroyButton(leftButton);
#endif /* __ANDROID__ */
	SDLU_DestroyButton(backButton);
	SDLU_DestroyButton(resetButton);

	SDLU_DestroyComboBox(themeBox);
	SDLU_DestroyComboBox(diffBox);

	if (action == Quit) {
		gSuper->finish();
	}
}