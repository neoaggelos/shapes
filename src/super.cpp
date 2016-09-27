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

	start_button = CreateButton("New Game", { 140, 270, 200, 40 }, 20, start_callback, &action);
	scores_button = CreateButton("High Scores", { 140, 350, 200, 40 }, 20, scores_callback, &action);
	settings_button = CreateButton("Settings", { 140, 430, 200, 40 }, 20, settings_callback, &action);
	exit_button = CreateButton("Exit", { 140, 510, 200, 40 }, 20, exit_callback, &action);

    while (action == None) {
        if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				action = Exit;
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
