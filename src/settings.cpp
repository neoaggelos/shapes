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
		changeShapeUpKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "changeShapeUpKey")));
		changeShapeDownKey = static_cast<SDL_Scancode>(StringToInt(SDLU_GetIniProperty(settings, NULL, "changeShapeDownKey")));
		difficulty = static_cast<DifficultyLevel>(StringToInt(SDLU_GetIniProperty(settings, NULL, "difficulty")));
		theme = SDLU_GetIniProperty(settings, NULL, "theme");
		lastName = SDLU_GetIniProperty(settings, NULL, "lastName");
		settings_version = StringToInt(SDLU_GetIniProperty(settings, NULL, "settings_version"));
	}

	if (settings)
		SDLU_DestroyIni(settings);
}

Settings::~Settings()
{
	if (savingSettings) {
		SDLU_IniHandler *h = SDLU_CreateIni();
		SDLU_SetIniProperty(&h, NULL, "settings_version", IntToString(settings_version));
		SDLU_SetIniProperty(&h, NULL, "moveRightKey", IntToString(moveRightKey));
		SDLU_SetIniProperty(&h, NULL, "moveLeftKey", IntToString(moveLeftKey));
		SDLU_SetIniProperty(&h, NULL, "changeShapeUpKey", IntToString(changeShapeUpKey));
		SDLU_SetIniProperty(&h, NULL, "changeShapeDownKey", IntToString(changeShapeDownKey));
		SDLU_SetIniProperty(&h, NULL, "difficulty", IntToString(difficulty));
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

static SDLU_Button* new_button(RenderData *data, const char *title, int x, int y, int w = -1, int h = -1)
{
	SDLU_Button *ret = SDLU_CreateButton(data->getWindow(), title, SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(ret, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(ret, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(ret, SDLU_HOVER_CALLBACK, on_hover, NULL);

	((SDLU_Styles*)ret->content)->font_size = 18;
	SDLU_SetButtonGeometry(ret, x, y, w, h);

	return ret;
}

static SDLU_ComboBox* new_cbox(RenderData *data, int x, int y, int w = -1, int h = -1)
{
	SDLU_ComboBox* ret = SDLU_CreateComboBox(data->getWindow());

	SDLU_SetComboBoxGeometry(ret, x, y, w, h);
	ret->styles->font_size = 18;

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
	SDLU_Button *shapeUpButton, *shapeDownButton, *rightButton, *leftButton, *resetButton, *backButton;
	SDLU_ComboBox *diffBox, *themeBox;
	RenderData* data = gSuper->getRenderData();
	SettingsMenuAction action;

	rightButton = new_button(data, "Change", 350, 125, 85, 35);
	SDLU_SetButtonCallback(rightButton, SDLU_PRESS_CALLBACK, right_callback, &action);
	
	leftButton = new_button(data, "Change", 350, 175, 85, 35);
	SDLU_SetButtonCallback(leftButton, SDLU_PRESS_CALLBACK, left_callback, &action);

	shapeUpButton = new_button(data, "Change", 350, 225, 85, 35);
	SDLU_SetButtonCallback(shapeUpButton, SDLU_PRESS_CALLBACK, shapeup_callback, &action);

	shapeDownButton = new_button(data, "Change", 350, 275, 85, 35);
	SDLU_SetButtonCallback(shapeDownButton, SDLU_PRESS_CALLBACK, shapedown_callback, &action);

	resetButton = new_button(data, "Reset To Default", 40, 560, 180, 35);
	SDLU_SetButtonCallback(resetButton, SDLU_PRESS_CALLBACK, reset_callback, &action);

	backButton = new_button(data, "Back To Menu", 260, 560, 180, 35);
	SDLU_SetButtonCallback(backButton, SDLU_PRESS_CALLBACK, back_callback, &action);

	themeBox = new_cbox(data, 350, 385, 85, 35);
	SDLU_AddComboBoxItem(&themeBox, "Red");
	SDLU_AddComboBoxItem(&themeBox, "Cats");
	SDLU_AddComboBoxItem(&themeBox, "Blue");
	SDLU_SetComboBoxActiveItem(themeBox, theme.c_str());

	diffBox = new_cbox(data, 350, 495, 85, 35);
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
			
			if (action >= ReadKeysStart && action <= ReadKeysEnd) {
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
			else if (action == Reset) {
				gSuper->getSettings()->reset();
				SDLU_SetComboBoxActiveIndex(diffBox, gSuper->getSettings()->difficulty);
				SDLU_SetComboBoxActiveItem(themeBox, gSuper->getSettings()->theme.c_str());
			}
		}

		Settings *s = gSuper->getSettings();
		SDL_Renderer *target = gSuper->getRenderData()->getRenderer();
		SDL_SetRenderDrawColor(target, 0, 0, 0, 0);
		SDL_RenderClear(target);

		SDL_SetRenderDrawColor(target, 0xff, 0xff, 0xff, 0xff);
		SDLU_SetFontSize(SDLU_TEXT_SIZE_MEDIUM);
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 15, "SETTINGS");
		SDLU_RenderText(target, 5, 70, "Controls");
		SDLU_RenderText(target, 5, 330, "Theme");
		SDLU_RenderText(target, 5, 440, "Difficulty");

		SDL_RenderDrawLine(target, 180, 50, 300, 50);
		SDL_RenderDrawLine(target, 5, 105, 475, 105);
		SDL_RenderDrawLine(target, 5, 365, 475, 365);
		SDL_RenderDrawLine(target, 5, 475, 475, 475);

		SDLU_SetFontSize(18);
		SDL_SetRenderDrawColor(target, 0xaa, 0xaa, 0xaa, 0xff);

		SDLU_RenderText(target, 15, 130, "Move Right");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 130, "%s", SDL_GetScancodeName(s->moveRightKey));
		SDLU_RenderText(target, 15, 180, "Move Left");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 180, "%s", SDL_GetScancodeName(s->moveLeftKey));
		SDLU_RenderText(target, 15, 230, "Change Shape Up");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 230, "%s", SDL_GetScancodeName(s->changeShapeUpKey));
		SDLU_RenderText(target, 15, 280, "Change Shape Down");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 280, "%s", SDL_GetScancodeName(s->changeShapeDownKey));

		SDLU_RenderText(target, 15, 390, "Choose Theme");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 390, "%s", themeBox->current);

		SDLU_RenderText(target, 15, 500, "Choose Difficulty");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 500, "%s", diffBox->current);

		SDLU_RenderButton(shapeUpButton);
		SDLU_RenderButton(shapeDownButton);
		SDLU_RenderButton(leftButton);
		SDLU_RenderButton(rightButton);
		SDLU_RenderButton(backButton);
		SDLU_RenderButton(resetButton);
		SDLU_RenderComboBox(themeBox);
		SDLU_RenderComboBox(diffBox);

		SDL_RenderPresent(target);

		SDL_Delay(10);
	}

	SDLU_DestroyButton(shapeUpButton);
	SDLU_DestroyButton(shapeDownButton);
	SDLU_DestroyButton(rightButton);
	SDLU_DestroyButton(leftButton);
	SDLU_DestroyButton(backButton);
	SDLU_DestroyButton(resetButton);

	SDLU_DestroyComboBox(themeBox);
	SDLU_DestroyComboBox(diffBox);

	if (action == Quit) {
		gSuper->finish();
	}
}