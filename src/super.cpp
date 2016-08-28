#include "main.h"


Super::Super()
{
    data = new RenderData();
    settings = new Settings();

    currentGame = NULL;
}

Super::~Super()
{
    delete data;
    delete settings;
}

void
Super::playGame()
{
    SDL_Event event;
    currentGame = new Game(settings->difficulty());

    SDLU_FPS_Init(30);

    while (currentGame->isPlaying()) {
        SDL_PollEvent(&event);
        SDLU_FPS_Start();
        currentGame->handleEvents(event, settings);
        currentGame->render(data);
        
        SDLU_FPS_Cap();
    }

    delete currentGame;
}


void
Super::mainMenu()
{
    enum ActionToTake {
        None,
        StartNewGame,
        Exit
    } action = None;

    SDL_Event event;
    SDLU_Button *start_button, *exit_button;

    start_button = SDLU_CreateButton(data->getWindow(), "New Game", SDLU_BUTTON_TEXT | SDLU_BUTTON_ROUND);
    SDLU_SetButtonAction(start_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
    SDLU_SetButtonAction(start_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
    SDLU_SetButtonGeometry(start_button, 100, 300, -1, 30);

    exit_button = SDLU_CreateButton(data->getWindow(), "Exit", SDLU_BUTTON_ROUND | SDLU_BUTTON_TEXT);
    SDLU_SetButtonAction(exit_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
    SDLU_SetButtonAction(exit_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
    SDLU_SetButtonGeometry(exit_button, 100, 350, -1, 30);
    
    while (action == None) {
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) {
            action = Exit;
        }
        if (event.type == SDLU_BUTTON_PRESS) {
            if ((Uint32)event.user.code == start_button->id) {
                action = StartNewGame;
            }
            else if ((Uint32)event.user.code == exit_button->id) {
                action = Exit;
            }
        }

        SDL_RenderClear(data->getRenderer());

        SDLU_RenderButton(start_button);
        SDLU_RenderButton(exit_button);

        SDL_RenderPresent(data->getRenderer());
    }

    if (action == StartNewGame) {
        playGame();
    }
    else if (action == Exit) {
        return;
    }
}