#include "main.h"

Super::Super()
{
    currentGame = NULL;
    data = new RenderData();
}

Super::~Super()
{
    delete data;
    if (currentGame) delete currentGame;
}

void
Super::playGame(int difficulty)
{
    SDL_Event event;
    currentGame = new Game(difficulty);

    while (SDL_PollEvent(&event) && currentGame->isPlaying()) {
        currentGame->handleEvents(event);

        currentGame->render(data);
    }

    delete currentGame;
}

void
Super::mainMenu()
{
    playGame(1);
}