#include "main.h"

typedef list<Shape*>::iterator ShapeIter;

Game::Game(int d)
{
    difficulty = d;
    score = 0;

    playerShape = new Shape();

    playing = true;
    
    startTime = SDL_GetTicks();
    lastAddTime = startTime;
    addShape();
}

Game::~Game()
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Ended", "Score is ", NULL);
    /* add highscores */
    for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
        delete (*it);
        shapes.erase(it);
    }
}

void
Game::addShape()
{
    int newTime = SDL_GetTicks();
    if ((newTime - lastAddTime >= 500) || (lastAddTime == startTime)) {
        lastAddTime = newTime;
        Shape * newShape = new Shape(difficulty);
        
        shapes.push_back(newShape);
    }
}

bool
Game::isPlaying()
{
    return playing;
}

int
Game::getTime()
{
    return (SDL_GetTicks() - startTime) / 1000;
}

int
Game::getScore()
{
    return score;
}

void
Game::render(RenderData *data)
{
    SDL_RenderClear(data->getRenderer());
    for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
        (*it)->render(data);
    }

    SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(data->getRenderer(), 0, 540, 640, 540);

    playerShape->render(data);
    
    /* TODO rid of SDLU, use bitmap font */
    SDLU_RenderText(data->getRenderer(), 5, 5, "Score: %5d -- Time: %d\n", score, (SDL_GetTicks() - startTime / 1000));
}

void
Game::handleEvents(SDL_Event event)
{
    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
            int lane = playerShape->getLane();
            lane++;
            if (lane >= 3) lane = 1;
            playerShape->setLane(lane);
        }
        else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
            int lane = playerShape->getLane();
            lane--;
            if (lane <= 1) lane = 3;
            playerShape->setLane(lane);
        }
        else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
            int type = playerShape->getType();
            type = (type == NUM_SHAPES - 1) ? 0 : type+1;
            playerShape->setType(type);
        }
        break;
    case SDL_QUIT:
        playing = false;
    }

    ShapeIter sh = shapes.begin();

    if (!(*sh)->isFalling()) {
        if (*playerShape == **sh) {
            score += 10;
            delete *sh;
            shapes.erase(sh);
        }
        else {
            playing = false;
        }
    }
}