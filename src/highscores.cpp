#include "main.h"

Highscores::Highscores()
{
	SDLU_IniHandler *ini;
	const char *scoresPath = getSettingsDir();
	savingScores = scoresPath != NULL;

	scoresIni = string(scoresPath) + "scores.ini";
	ini = SDLU_LoadIni(scoresIni.c_str());

	bool isOK = true;
	for (int diff = 0; diff < 3 && isOK; diff++) {
		for (int i = 0; i < 5 && isOK; i++) {
			int p = diff * 10 + i;

			const char *name = SDLU_GetIniProperty(ini, "names", to_string(p).c_str());
			const char *score = SDLU_GetIniProperty(ini, "scores", to_string(p).c_str());

			isOK = name != NULL && score != NULL;
			if (isOK) {
				scores[diff][i] = to_int(score);
				names[diff][i] = SDL_strdup(name); //TODO
			}
		}
	}

	if (!isOK) {
		cout << "NOT OK" << endl; //DEBUG
		for (int diff = 0; diff < 3; diff++) {
			for (int i = 0; i < 5; i++) {
				scores[diff][i] = 0;
				names[diff][i] = "no name";
			}
		}
	}
	
	if (ini)
		SDLU_DestroyIni(ini);
}

Highscores::~Highscores()
{
	if (savingScores) {
		SDLU_IniHandler *ini = SDLU_CreateIni();
		
		for (int diff = 0; diff < 3; diff++) {
			for (int i = 0; i < 5; i++) {
				int p = diff * 10 + i;
				SDLU_SetIniProperty(&ini, "scores", to_string(p).c_str(), to_string(scores[diff][i]).c_str());
				SDLU_SetIniProperty(&ini, "names", to_string(p).c_str(), names[diff][i].c_str());
			}
		}

		SDLU_SaveIni(ini, scoresIni.c_str());

		SDLU_DestroyIni(ini);
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
		if (score >= scores[diff][i]) {
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

enum HighscoresMenuAction {
	PlayAgain, BackToMenu, Quit, None
};

static void
again_callback(void *_button, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*(static_cast<HighscoresMenuAction*>(action)) = PlayAgain;
}

static void
back_callback(void *_button, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*(static_cast<HighscoresMenuAction*>(action)) = BackToMenu;
}

static void
right_callback(void *_button, void *_diff)
{
	gSuper->getAudioData()->play("bleep");
	int *diff = static_cast<int*>(_diff);
	if (*diff < 2) *diff += 1;
}

static void
left_callback(void *_button, void *_diff)
{
	gSuper->getAudioData()->play("bleep");
	int *diff = static_cast<int*>(_diff);
	if (*diff > 0) *diff -= 1;
}

void
Highscores::openMenu(int currentdiff, int currentindex)
{
	SDLU_Button *again_button, *back_button, *right_button, *left_button;
	SDL_Event event;
	HighscoresMenuAction action = None;
	int diff = 1;

	RenderData* data = gSuper->getRenderData();
	again_button = SDLU_CreateButton(data->getWindow(), "I can beat it", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(again_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(again_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(again_button, SDLU_PRESS_CALLBACK, again_callback, &action);
	SDLU_SetButtonCallback(again_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(again_button, 140, 480, 200, 40);

	back_button = SDLU_CreateButton(data->getWindow(), "Back To Menu", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(back_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(back_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(back_button, SDLU_PRESS_CALLBACK, back_callback, &action);
	SDLU_SetButtonCallback(back_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(back_button, 140, 540, 200, 40);
	SDLU_SetButtonHotkey(back_button, SDL_SCANCODE_AC_BACK);
	
	right_button = SDLU_CreateButton(data->getWindow(), ">", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(right_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(right_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(right_button, SDLU_PRESS_CALLBACK, right_callback, &diff);
	SDLU_SetButtonCallback(right_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(right_button, 350, 100, 30, 30);
	SDLU_SetButtonHotkey(right_button, SDL_SCANCODE_RIGHT);
	((SDLU_Styles*)right_button->content)->fill_color = SDLU_CreateRGB(SDLU_BLACK_RGB);

	left_button = SDLU_CreateButton(data->getWindow(), "<", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(left_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(left_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(left_button, SDLU_PRESS_CALLBACK, left_callback, &diff);
	SDLU_SetButtonCallback(left_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(left_button, 100, 100, 30, 30);
	SDLU_SetButtonHotkey(left_button, SDL_SCANCODE_LEFT);
	((SDLU_Styles*)left_button->content)->fill_color = SDLU_CreateRGB(SDLU_BLACK_RGB);

	while (action == None) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				action = Quit;
		}

		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());

		SDLU_RenderButton(again_button);
		SDLU_RenderButton(back_button);
		SDLU_RenderButton(right_button);
		SDLU_RenderButton(left_button);

		SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
		gSuper->getTextRenderer()->write(20, "HIGH SCORES", { 0, 20, 480, 100 }, Center);
		SDL_RenderDrawLine(data->getRenderer(), 140, 60, 340, 60);

		string diff_string = "Easy";
		if (diff == 1) diff_string = "Medium";
		if (diff == 2) diff_string = "Hard";
		gSuper->getTextRenderer()->write(20, diff_string, { 0, 100, 480, 30 }, Center, Center);

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

			gSuper->getTextRenderer()->write(20, to_string(i + 1), 100, ypos);
			gSuper->getTextRenderer()->write(20, names[diff][i], { 0, ypos, 480, 100 }, Center);
			gSuper->getTextRenderer()->write(20, to_string(scores[diff][i]), { 0, ypos, 380, 100 }, Right);

			ypos += 60;
		}

		SDL_RenderPresent(data->getRenderer());

		//SDL_Delay(10);
	}

	SDLU_DestroyButton(again_button);
	SDLU_DestroyButton(back_button);

	if (action == Quit)
		gSuper->finish();
	else if (action == PlayAgain) {
		gSuper->playGame();
	}
	else if (action == BackToMenu) {
		gSuper->mainMenu();
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