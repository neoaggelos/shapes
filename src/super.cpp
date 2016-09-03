#include "main.h"

Super::Super()
{
    data = new RenderData();
    settings = new Settings();
	highscores = new Highscores(this);

	game = NULL;
	settingsMenu = NULL;
}

Super::~Super()
{
	finish();
}

void
Super::finish()
{
	if (game) delete game;
	if (settingsMenu) delete settingsMenu;


	delete highscores;
	delete data;
	delete settings;
	exit(0);
}

void
Super::playGame()
{
    game = new Game(this);

	game->run();
    delete game;
	game = NULL;

	mainMenu();
}

void
Super::openSettings()
{
	settingsMenu = new SettingsMenu(this);
	settingsMenu->run();
	delete settingsMenu;
	settingsMenu = NULL;

	mainMenu();
}


void
Super::mainMenu()
{
    enum ActionToTake {
        None,
        StartNewGame,
		OpenSettings,
		OpenHighScores,
        Exit
    } action = None;

    SDL_Event event;
    SDLU_Button *start_button, *settings_button, *scores_button, *exit_button;

    start_button = SDLU_CreateButton(data->getWindow(), "New Game", SDLU_BUTTON_TEXT);
    SDLU_SetButtonAction(start_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
    SDLU_SetButtonAction(start_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
    SDLU_SetButtonGeometry(start_button, 140, 270, 200, 40);
	
	settings_button = SDLU_CreateButton(data->getWindow(), "Settings", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(settings_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(settings_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(settings_button, 140, 350, 200, 40);

	scores_button = SDLU_CreateButton(data->getWindow(), "High Scores", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(scores_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(scores_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(scores_button, 140, 430, 200, 40);

    exit_button = SDLU_CreateButton(data->getWindow(), "Exit", SDLU_BUTTON_TEXT);
    SDLU_SetButtonAction(exit_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
    SDLU_SetButtonAction(exit_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
    SDLU_SetButtonGeometry(exit_button, 140, 510, 200, 40);

    while (action == None) {
		SDL_PumpEvents();
        if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				action = Exit;
			}
			if (event.type == SDLU_BUTTON_PRESS) {
				Uint32 button_id = static_cast<Uint32>(event.user.code);
				if (button_id == start_button->id) {
					action = StartNewGame;
				}
				else if (button_id == settings_button->id) {
					action = OpenSettings;
				}
				else if (button_id == scores_button->id) {
					action = OpenHighScores;
				}
				else if (button_id == exit_button->id) {
					action = Exit;
				}
			}
		}

		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
        SDL_RenderClear(data->getRenderer());

        SDLU_RenderButton(start_button);
		SDLU_RenderButton(settings_button);
		SDLU_RenderButton(scores_button);
        SDLU_RenderButton(exit_button);

		SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
		SDLU_SetFontSize(SDLU_TEXT_SIZE_SMALL);
		SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_RIGHT, SDLU_ALIGN_BOTTOM, "Shapes v0.1");
		SDLU_SetFontSize(SDLU_TEXT_SIZE_LARGE);
		SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 100, "SHAPES");

        SDL_RenderPresent(data->getRenderer());

		//SDL_Delay(1); /* let the CPU rest */
    }

	SDLU_DestroyButton(start_button);
	SDLU_DestroyButton(settings_button);
	SDLU_DestroyButton(exit_button);
    if (action == StartNewGame) {
        playGame();
	}
	else if (action == OpenSettings) {
		openSettings();
	}
	else if (action == OpenHighScores) {
		highscores->openMenu();
	}
    else if (action == Exit) {
		finish();
    }
}
