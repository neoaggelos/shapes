#include "main.h"


Super::Super()
{
    currentGame = NULL;
    data = new RenderData();
}

Super::~Super()
{
    delete data;
}


void
Super::playGame(int difficulty)
{
    SDL_Event event;
    currentGame = new Game(difficulty);

    SDLU_FPS_Init(30);

    while (currentGame->isPlaying()) {
        SDL_PollEvent(&event);
        SDLU_FPS_Start();
        currentGame->handleEvents(event);

        currentGame->render(data);

        SDLU_FPS_Cap();
    }

    delete currentGame;
}

void
Super::mainMenu()
{
    playGame(1);
}