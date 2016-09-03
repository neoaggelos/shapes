#include "main.h"

static SDLU_Button* new_button(RenderData *data, const char *title, int x, int y, int w = -1, int h = -1)
{
	SDLU_Button *ret = SDLU_CreateButton(data->getWindow(), title, SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(ret, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(ret, SDLU_HOVER_ACTION, SDLU_HOVER_BG);

	SDLU_SetButtonGeometry(ret, x, y, w, h);

	return ret;
}

static SDLU_ComboBox* new_cbox(RenderData *data, int x, int y, int w = -1, int h = -1)
{
	SDLU_ComboBox* ret = SDLU_CreateComboBox(data->getWindow());

	SDLU_SetComboBoxGeometry(ret, x, y, w, h);

	return ret;
}

SettingsMenu::SettingsMenu(Super *super)
{
	parent = super;

	RenderData* data = parent->getRenderData();
	
	rightButton = new_button(data, "Change", 350, 120, 85, 35);
	leftButton = new_button(data, "Change", 350, 170, 85, 35);

	shapeButton = new_button(data, "Change", 350, 220, 85, 35);
	
	resetButton = new_button(data, "Reset to defaults", 140, 480, 200, 35);
	backButton = new_button(data, "Back to Menu", 140, 530, 200, 35);

	diffBox = new_cbox(data, 350, 380, 85, 35);
	SDLU_AddComboBoxItem(&diffBox, "Easy");
	SDLU_AddComboBoxItem(&diffBox, "Medium");
	SDLU_AddComboBoxItem(&diffBox, "Hard");

	SDLU_SetComboBoxActiveItem(diffBox, parent->getSettings()->difficulty);

	mustClose = false;
}

SettingsMenu::~SettingsMenu()
{
	SDLU_DestroyButton(shapeButton);
	SDLU_DestroyButton(rightButton);
	SDLU_DestroyButton(leftButton);
	SDLU_DestroyButton(backButton);
	SDLU_DestroyButton(resetButton);

	SDLU_DestroyComboBox(diffBox);
}

void
SettingsMenu::handleEvent(SDL_Event event)
{
	if (event.type == SDL_QUIT)
		parent->finish();
	else if (event.type == SDLU_BUTTON_PRESS) {
		Uint32 button_id = static_cast<Uint32>(event.user.code);
		if (button_id == backButton->id)
			mustClose = true;
		else if (button_id == resetButton->id) {
			parent->getSettings()->reset();
			SDLU_SetComboBoxActiveItem(diffBox, parent->getSettings()->difficulty);
		}
		else {
			event.type = SDL_LASTEVENT;
			SDL_Scancode newKey = SDL_SCANCODE_UNKNOWN;

			while (newKey == SDL_SCANCODE_UNKNOWN) {
				if (SDL_PollEvent(&event)) {
					if (event.type == SDL_KEYDOWN) {
						newKey = event.key.keysym.scancode;
					}
					else if (event.type == SDL_QUIT) {
						parent->finish();
					}
				}
				//SDL_Delay(10);
			}

			if (newKey != SDL_SCANCODE_ESCAPE) {
				if (button_id == shapeButton->id)
					parent->getSettings()->changeShapeKey = newKey;
				else if (button_id == leftButton->id)
					parent->getSettings()->moveLeftKey = newKey;
				else if (button_id == rightButton->id)
					parent->getSettings()->moveRightKey = newKey;
			}
		}
	}
	else if (event.type == SDLU_COMBOBOX_TEXTCHANGED) {
		Uint32 cbox_id = static_cast<Uint32>(event.user.code);
		if (cbox_id == diffBox->id) {
			DifficultyLevel new_diff = static_cast<DifficultyLevel>(diffBox->current_index);
			parent->getSettings()->difficulty = new_diff;
		}
	}
}

void
SettingsMenu::render()
{
	Settings *s = parent->getSettings();
	SDL_Renderer *target = parent->getRenderData()->getRenderer();
	SDL_SetRenderDrawColor(target, 0, 0, 0, 0);
	SDL_RenderClear(target);

	SDL_SetRenderDrawColor(target, 0xff, 0xff, 0xff, 0xff);
	SDLU_SetFontSize(SDLU_TEXT_SIZE_MEDIUM);
	SDLU_RenderText(target, SDLU_ALIGN_CENTER, 5, "SETTINGS");
	SDLU_RenderText(target, 5, 70, "Controls");
	SDLU_RenderText(target, 5, 330, "Difficulty");

	SDL_RenderDrawLine(target, 180, 40, 300, 40);
	SDL_RenderDrawLine(target, 5, 105, 475, 105);
	SDL_RenderDrawLine(target, 5, 365, 475, 365);

	SDLU_SetFontSize(18);
	SDL_SetRenderDrawColor(target, 0xaa, 0xaa, 0xaa, 0xff);
	SDLU_RenderText(target, 15, 130, "Move Right");
	SDLU_RenderText(target, SDLU_ALIGN_CENTER, 130, "%s", SDL_GetScancodeName(s->moveRightKey));
	SDLU_RenderText(target, 15, 180, "Move Left");
	SDLU_RenderText(target, SDLU_ALIGN_CENTER, 180, "%s", SDL_GetScancodeName(s->moveLeftKey));
	SDLU_RenderText(target, 15, 230, "Change Shape");
	SDLU_RenderText(target, SDLU_ALIGN_CENTER, 230, "%s", SDL_GetScancodeName(s->changeShapeKey));
	SDLU_RenderText(target, 15, 390, "Choose Difficulty");
	SDLU_RenderText(target, SDLU_ALIGN_CENTER, 390, "%s", diffBox->current);

	SDLU_RenderButton(shapeButton);
	SDLU_RenderButton(leftButton);
	SDLU_RenderButton(rightButton);
	SDLU_RenderButton(backButton);
	SDLU_RenderButton(resetButton);
	SDLU_RenderComboBox(diffBox);

	SDL_RenderPresent(target);
}

void
SettingsMenu::run()
{
	SDL_Event event;
	while (!isDone()) {
		if (SDL_PollEvent(&event)) {
			handleEvent(event);

			render();
		}

		//SDL_Delay(4);
	}
}

bool
SettingsMenu::isDone()
{
	return mustClose;
}