#include "main.h"

Highscores::Highscores(Super *super)
{
	parent = super;

	SDLU_IniHandler *ini;
	const char *scoresPath = getSettingsDir();
	savingScores = scoresPath != NULL;

	scoresIni = string(scoresPath) + "scores.ini";
	ini = SDLU_LoadIni(scoresIni.c_str());

	bool isOK = true;
	for (int diff = 0; diff < 3 && isOK; diff++) {
		for (int i = 0; i < 5 && isOK; i++) {
			char* prop = IntToString(diff * 10 + i);

			const char *name = SDLU_GetIniProperty(ini, "names", prop);
			const char *score = SDLU_GetIniProperty(ini, "scores", prop);

			isOK = name != NULL && score != NULL;
			if (isOK) {
				scores[diff][i] = StringToInt(score);
				names[diff][i] = SDL_strdup(name); //TODO
			}
		}
	}

	if (!isOK) {
		for (int diff = 0; diff < 3; diff++) {
			for (int i = 0; i < 5; i++) {
				scores[diff][i] = 0;
				names[diff][i] = "no name";
			}
		}
	}
}

Highscores::~Highscores()
{
	if (savingScores) {
		SDLU_IniHandler *ini = SDLU_CreateIni();
		
		for (int diff = 0; diff < 3; diff++) {
			for (int i = 0; i < 5; i++) {
				const char *prop = IntToString(diff * 10 + i);
				SDLU_SetIniProperty(&ini, "scores", prop, IntToString(scores[diff][i]));
				SDLU_SetIniProperty(&ini, "names", prop, names[diff][i].c_str());
			}
		}

		SDLU_SaveIni(ini, scoresIni.c_str());
	}
}

int
Highscores::addScore(int diff, int score, string name)
{
	int index = -1;
	
	diff--;
	if (diff < 0 || diff >= 3)
		return -1;

	for (int i = 0; i < 5; i++) {
		if (score > scores[diff][i]) {
			/* shift scores */
			for (int j = 4; j >= i; j--) {
				scores[diff][j] = scores[diff][j - 1];
				names[diff][j] = names[diff][j - 1];
			}

			index = i;
			scores[diff][index] = score;
			names[diff][index] = name;

			break;
		}
	}

	return index;
}

void
Highscores::openMenu(int currentdiff, int currentindex)
{
	SDLU_Button *again_button, *back_button, *right_button, *left_button;
	SDL_Event event;
	int diff = 1;

	RenderData* data = parent->getRenderData();
	again_button = SDLU_CreateButton(data->getWindow(), "I can beat it", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(again_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(again_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(again_button, 140, 480, 200, 40);

	back_button = SDLU_CreateButton(data->getWindow(), "Back To Menu", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(back_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(back_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(back_button, 140, 540, 200, 40);
	
	right_button = SDLU_CreateButton(data->getWindow(), ">", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(right_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(right_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(right_button, 350, 100, 30, 30);
	SDLU_SetButtonHotkey(right_button, SDL_SCANCODE_RIGHT);
	((SDLU_Styles*)right_button->content)->fill_color = SDLU_CreateRGB(SDLU_BLACK_RGB);

	left_button = SDLU_CreateButton(data->getWindow(), "<", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(left_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(left_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(left_button, 100, 100, 30, 30);
	SDLU_SetButtonHotkey(left_button, SDL_SCANCODE_LEFT);
	((SDLU_Styles*)left_button->content)->fill_color = SDLU_CreateRGB(SDLU_BLACK_RGB);

	enum {
		PlayAgain, BackToMenu, Quit, None
	} action = None;

	SDLU_SetFontSize(SDLU_TEXT_SIZE_MEDIUM);
	while (action == None) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				action = Quit;
			else if (event.type == SDLU_BUTTON_PRESS) {
				Uint32 button_id = static_cast<Uint32>(event.user.code);

				if (button_id == again_button->id)
					action = PlayAgain;
				else if (button_id == back_button->id)
					action = BackToMenu;
				else if (button_id == right_button->id && diff < 2)
					diff++;
				else if (button_id == left_button->id && diff > 0)
					diff--;
			}
		}

		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());

		SDLU_RenderButton(again_button);
		SDLU_RenderButton(back_button);
		SDLU_RenderButton(right_button);
		SDLU_RenderButton(left_button);

		SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
		SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 20, "HIGH SCORES");
		SDL_RenderDrawLine(data->getRenderer(), 140, 60, 340, 60);

		string diff_string = "Easy";
		if (diff == 1) diff_string = "Medium";
		if (diff == 2) diff_string = "Hard";
		SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 103, "%s", diff_string.c_str());

		SDL_RenderDrawLine(data->getRenderer(), 130, 100, 350, 100);
		SDL_RenderDrawLine(data->getRenderer(), 130, 129, 350, 129);

		int ypos = 170;
		for (int i = 0; i < 5; i++) {
			if (currentdiff == diff && currentindex == i) {
				SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xff, 0x00, 0xff);
			}
			else if (i == 0) {
				SDL_SetRenderDrawColor(data->getRenderer(), 0xcc, 0xcc, 0xcc, 0xff);
			}
			else {
				SDL_SetRenderDrawColor(data->getRenderer(), 0x88, 0x88, 0x88, 0xff);
			}

			SDLU_RenderText(data->getRenderer(), 100, ypos, "%d.", i+1);
			SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, ypos, "%18s %7d", names[diff][i].c_str(), scores[diff][i]);

			ypos += 60;
		}

		SDL_RenderPresent(data->getRenderer());

		//SDL_Delay(10);
	}

	SDLU_DestroyButton(again_button);
	SDLU_DestroyButton(back_button);

	if (action == Quit)
		parent->finish();
	else if (action == PlayAgain) {
		parent->playGame();
	}
	else if (action == BackToMenu) {
		parent->mainMenu();
	}
}

int
Highscores::getScore(int diff, int index)
{
	diff--;
	if (index < 0 || index >= 5 || diff < 0 || diff >= 3)
		return -1;

	return scores[diff][index];
}