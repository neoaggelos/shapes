#include "main.h"

Super::Super()
{
    settings = new Settings();
	render = new RenderData(settings->theme);
	textrenderer = new TextRenderer();
	highscores = new Highscores();
	audio = new AudioData();

	game = NULL;
}

Super::~Super()
{
	finish();
}

void
Super::finish()
{
	if (game) delete game;

	delete highscores;
	delete render;
	delete textrenderer;
	delete settings;
	delete audio;
	exit(0);
}

void
Super::playGame()
{
    game = new Game();

	game->run();
    delete game;
	game = NULL;

	mainMenu();
}

void
Super::openSettings()
{
	settings->openMenu();

	mainMenu();
}

enum MainMenuAction {
	None,
	StartNewGame,
	OpenSettings,
	OpenHighScores,
	Exit
};

static void
start_callback(void* _button, void *action)
{
	*(static_cast<MainMenuAction*>(action)) = StartNewGame;
	gSuper->getAudioData()->play("bleep");
}

static void
settings_callback(void* _button, void *action)
{
	*(static_cast<MainMenuAction*>(action)) = OpenSettings;
	gSuper->getAudioData()->play("bleep");
}

static void
scores_callback(void* _button, void *action)
{
	*(static_cast<MainMenuAction*>(action)) = OpenHighScores;
	gSuper->getAudioData()->play("bleep");
}

static void
exit_callback(void* _button, void *action)
{
	gSuper->getAudioData()->play("bleep");
	*(static_cast<MainMenuAction*>(action)) = Exit;
}

void
Super::mainMenu()
{
	MainMenuAction action = None;
    SDL_Event event;
    SDLU_Button *start_button, *settings_button, *scores_button, *exit_button;

    start_button = SDLU_CreateButton(render->getWindow(), "New Game", SDLU_BUTTON_TEXT);
    SDLU_SetButtonAction(start_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
    SDLU_SetButtonAction(start_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(start_button, SDLU_PRESS_CALLBACK, start_callback, &action);
	SDLU_SetButtonCallback(start_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(start_button, 140, 270, 200, 40);
	
	settings_button = SDLU_CreateButton(render->getWindow(), "Settings", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(settings_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(settings_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(settings_button, SDLU_PRESS_CALLBACK, settings_callback, &action);
	SDLU_SetButtonCallback(settings_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(settings_button, 140, 430, 200, 40);

	scores_button = SDLU_CreateButton(render->getWindow(), "High Scores", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(scores_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(scores_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(scores_button, SDLU_PRESS_CALLBACK, scores_callback, &action);
	SDLU_SetButtonCallback(scores_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
	SDLU_SetButtonGeometry(scores_button, 140, 350, 200, 40);

    exit_button = SDLU_CreateButton(render->getWindow(), "Exit", SDLU_BUTTON_TEXT);
    SDLU_SetButtonAction(exit_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
    SDLU_SetButtonAction(exit_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(exit_button, SDLU_PRESS_CALLBACK, exit_callback, &action);
	SDLU_SetButtonCallback(exit_button, SDLU_HOVER_CALLBACK, on_hover, NULL);
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

		SDL_SetRenderDrawColor(render->getRenderer(), 0, 0, 0, 0);
        SDL_RenderClear(render->getRenderer());

        SDLU_RenderButton(start_button);
		SDLU_RenderButton(settings_button);
		SDLU_RenderButton(scores_button);
        SDLU_RenderButton(exit_button);

		SDL_SetRenderDrawColor(render->getRenderer(), 0xff, 0xff, 0xff, 0xff);
		textrenderer->write(14, "Shapes v" VERSION " Written by Aggelos Kolaitis", { 0, 0, 480, 640 }, Right, Bottom);
		textrenderer->write(42, "SHAPES", { 0, 100, 480, 100 }, Center);
        SDL_RenderPresent(render->getRenderer());

		//SDL_Delay(1); /* let the CPU rest */
    }

	SDLU_DestroyButton(start_button);
	SDLU_DestroyButton(scores_button);
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
