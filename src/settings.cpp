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

Settings::Settings(Super *super)
{
	parent = super;

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
		SDLU_SetIniProperty(&h, NULL, "changeShapeKey", IntToString(changeShapeKey));
		SDLU_SetIniProperty(&h, NULL, "difficulty", IntToString(difficulty));
		SDLU_SetIniProperty(&h, NULL, "theme", theme.c_str());

		SDLU_SaveIni(h, settingsIni.c_str());
		SDLU_DestroyIni(h);
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

static SDLU_Button* new_button(RenderData *data, const char *title, int x, int y, int w = -1, int h = -1)
{
	SDLU_Button *ret = SDLU_CreateButton(data->getWindow(), title, SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(ret, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(ret, SDLU_HOVER_ACTION, SDLU_HOVER_BG);

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
	ReadShapeKey = 3,
	ReadKeysEnd = 4,
	Reset,
	BackToMenu,
	Quit,
	None
};

static void
right_callback(void *_this, void *action)
{
	*static_cast<SettingsMenuAction*>(action) = ReadRightKey;
}

static void
left_callback(void *_this, void *action)
{
	*static_cast<SettingsMenuAction*>(action) = ReadLeftKey;
}

static void
shape_callback(void *_this, void *action)
{
	*static_cast<SettingsMenuAction*>(action) = ReadShapeKey;
}

static void
reset_callback(void *_this, void *action)
{
	*static_cast<SettingsMenuAction*>(action) = Reset;
}

static void
back_callback(void *_this, void *action)
{
	*static_cast<SettingsMenuAction*>(action) = BackToMenu;
}

void
Settings::openMenu()
{
	SDLU_Button *shapeButton, *rightButton, *leftButton, *resetButton, *backButton;
	SDLU_ComboBox *diffBox, *themeBox;
	RenderData* data = parent->getRenderData();
	SettingsMenuAction action;

	rightButton = new_button(data, "Change", 350, 125, 85, 35);
	SDLU_SetButtonCallback(rightButton, SDLU_PRESS_CALLBACK, right_callback, &action);
	
	leftButton = new_button(data, "Change", 350, 175, 85, 35);
	SDLU_SetButtonCallback(leftButton, SDLU_PRESS_CALLBACK, left_callback, &action);

	shapeButton = new_button(data, "Change", 350, 225, 85, 35);
	SDLU_SetButtonCallback(shapeButton, SDLU_PRESS_CALLBACK, shape_callback, &action);

	resetButton = new_button(data, "Reset To Default", 40, 560, 180, 35);
	SDLU_SetButtonCallback(resetButton, SDLU_PRESS_CALLBACK, reset_callback, &action);

	backButton = new_button(data, "Back To Menu", 260, 560, 180, 35);
	SDLU_SetButtonCallback(backButton, SDLU_PRESS_CALLBACK, back_callback, &action);

	themeBox = new_cbox(data, 350, 385, 85, 35);

	SDLU_Directory *dir = SDLU_OpenDirectory(getAssetsDir().c_str(), SDL_TRUE);
	SDLU_FileInfo *info;

	while ((info = SDLU_NextFileWithExtension(dir, "bmp")))
	{
		std::string name = string(info->filename).substr(0, SDL_strlen(info->filename) - 4);
		SDLU_AddComboBoxItem(&themeBox, SDL_strdup(name.c_str()));
	}
	SDLU_SetComboBoxActiveItem(themeBox, parent->getSettings()->theme.c_str());
	SDLU_CloseDirectory(dir);

	diffBox = new_cbox(data, 350, 495, 85, 35);
	SDLU_AddComboBoxItem(&diffBox, "Easy");
	SDLU_AddComboBoxItem(&diffBox, "Medium");
	SDLU_AddComboBoxItem(&diffBox, "Hard");
	SDLU_SetComboBoxActiveIndex(diffBox, parent->getSettings()->difficulty);

	SDL_Event event;
	action = None;
	while (action != BackToMenu && action != Quit) {
		action = None;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				parent->finish();
			}
			else if (event.type == SDLU_COMBOBOX_TEXTCHANGED) {
				Uint32 cbox_id = static_cast<Uint32>(event.user.code);
				if (cbox_id == diffBox->id) {
					DifficultyLevel new_diff = static_cast<DifficultyLevel>(diffBox->current_index);
					parent->getSettings()->difficulty = new_diff;
				}
				else if (cbox_id == themeBox->id) {
					parent->getSettings()->theme = SDL_strdup(themeBox->current);
					parent->getRenderData()->reloadTexture(parent);
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
						parent->getSettings()->moveRightKey = newKey;
					else if (action == ReadLeftKey)
						parent->getSettings()->moveLeftKey = newKey;
					else if (action == ReadShapeKey)
						parent->getSettings()->changeShapeKey = newKey;
				}
			}
			else if (action == Reset) {
				parent->getSettings()->reset();
				SDLU_SetComboBoxActiveIndex(diffBox, parent->getSettings()->difficulty);
				SDLU_SetComboBoxActiveItem(themeBox, parent->getSettings()->theme.c_str());
			}
		}

		Settings *s = parent->getSettings();
		SDL_Renderer *target = parent->getRenderData()->getRenderer();
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
		SDLU_RenderText(target, 15, 230, "Change Shape");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 230, "%s", SDL_GetScancodeName(s->changeShapeKey));
		SDLU_RenderText(target, 15, 280, "Pause Game");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 280, "Escape");

		SDLU_RenderText(target, 15, 390, "Choose Theme");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 390, "%s", parent->getSettings()->theme.c_str());

		SDLU_RenderText(target, 15, 500, "Choose Difficulty");
		SDLU_RenderText(target, SDLU_ALIGN_CENTER, 500, "%s", diffBox->current);

		SDLU_RenderButton(shapeButton);
		SDLU_RenderButton(leftButton);
		SDLU_RenderButton(rightButton);
		SDLU_RenderButton(backButton);
		SDLU_RenderButton(resetButton);
		SDLU_RenderComboBox(themeBox);
		SDLU_RenderComboBox(diffBox);

		SDL_RenderPresent(target);

		SDL_Delay(10);
	}

	SDLU_DestroyButton(shapeButton);
	SDLU_DestroyButton(rightButton);
	SDLU_DestroyButton(leftButton);
	SDLU_DestroyButton(backButton);
	SDLU_DestroyButton(resetButton);

	SDLU_DestroyComboBox(themeBox);
	SDLU_DestroyComboBox(diffBox);

	if (action == Quit) {
		parent->finish();
	}
}