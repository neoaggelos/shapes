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

Highscores::Highscores(Super *super)
{
	parent = super;

	SDLU_IniHandler *ini;
	const char *scoresPath = SDL_GetPrefPath("shapes", "shapes");
	savingScores = scoresPath != NULL;

	scoresIni = string(scoresPath) + "scores.ini";
	ini = SDLU_LoadIni(scoresIni.c_str());

	bool isOK = true;
	for (int diff = 0; diff < 3 && isOK; diff++) {
		for (int i = 0; i < 5 && isOK; i++) {
			const char *prop = SDLU_GetIniProperty(ini, IntToString(diff+1), IntToString(i));

			isOK = prop != NULL;
			if (isOK)
				scores[diff][i] = StringToInt(prop);
		}
	}

	if (!isOK) {
		for (int diff = 0; diff < 3; diff++)
			for (int i = 0; i < 5; i++)
				scores[diff][i] = 0;
	}
}

Highscores::~Highscores()
{
	if (savingScores) {
		SDLU_IniHandler *ini = SDLU_CreateIni();
		
		for (int diff = 0; diff < 3; diff++) {
			for (int i = 0; i < 5; i++) {
				SDLU_SetIniProperty(&ini, IntToString(diff+1), IntToString(i), IntToString(scores[diff][i]));
			}
		}

		SDLU_SaveIni(ini, scoresIni.c_str());
	}
}

int
Highscores::addScore(int diff, int score)
{
	int index = -1;
	
	diff--;
	if (diff < 0 || diff >= 3)
		return -1;

	for (int i = 0; i < 5; i++) {
		if (score > scores[diff][i]) {
			/* shift scores */
			for (int j = 4; j >= i; j--)
				scores[diff][j] = scores[diff][j - 1];

			index = i;
			scores[diff][index] = score;

			break;
		}
	}

	return index;
}

void
Highscores::openMenu(int currentdiff, int currentindex)
{
	SDLU_Button *again_button, *back_button;
	SDL_Event event;
	int ypos, xpos;

	RenderData* data = parent->getRenderData();
	again_button = SDLU_CreateButton(data->getWindow(), "I can beat it", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(again_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(again_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(again_button, 140, 480, 200, 40);

	back_button = SDLU_CreateButton(data->getWindow(), "Back To Menu", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(back_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(back_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(back_button, 140, 540, 200, 40);
	
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
			}
		}

		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());

		SDLU_RenderButton(again_button);
		SDLU_RenderButton(back_button);

		SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
		SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 20, "HIGH SCORES");
		SDL_RenderDrawLine(data->getRenderer(), 140, 60, 340, 60);

		SDLU_RenderText(data->getRenderer(), 80, 110, "Easy");
		SDLU_RenderText(data->getRenderer(), 200, 110, "Medium");
		SDLU_RenderText(data->getRenderer(), 350, 110, "Hard");

		xpos = 95;
		for (int diff = 0; diff < 3; diff++) {

			ypos = 170;
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

				//SDLU_RenderText(data->getRenderer(), 30, ypos, "%d.", i+1);
				SDLU_RenderText(data->getRenderer(), xpos, ypos, "%d", scores[diff][i]);

				ypos += 60;
			}

			xpos += 130;
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