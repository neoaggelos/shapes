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
    addShape(startTime);
}

Game::~Game()
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Ended", "TODO", NULL);
    /* add highscores */
    if (!shapes.empty()) {
        for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
            delete (*it);
        }
    }
}


void
Game::addShape(Uint32 newTime)
{
    lastAddTime = newTime;
    Shape * newShape = new Shape(difficulty);

    shapes.push_back(newShape);
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
    SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
    SDL_RenderFillRect(data->getRenderer(), NULL);

    if (!shapes.empty()) {
        for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
            (*it)->render(data);
        }
    }

    SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(data->getRenderer(), 0, 540, 640, 540);

    playerShape->render(data);

    SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0x00, 0xff, 0xff);
    SDLU_RenderText(data->getRenderer(), 0, 5, "SCORE: %d TIME: %.1lf", score, (double)(SDL_GetTicks() - startTime) / 1000.0);

    SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xff, 0x00, 0xff);
    SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_RIGHT, 5, "FPS: %d", SDLU_FPS_GetRealFramerate());

    for (int i = 0; i < NUM_SHAPES; i++) {
        SDL_Rect dest = { 480 - 25, 520 - i * 20, 20, 20 };

        SDL_RenderCopy(data->getRenderer(), data->getTexture(i), NULL, &dest);
        if (playerShape->getType() == i) {
            SDL_RenderDrawRect(data->getRenderer(), &dest);
        }
    }

    SDL_RenderPresent(data->getRenderer());
}

void
Game::handleEvents(SDL_Event event)
{
    static enum LastAction {
        None,
        MovedRight,
        MovedLeft,
        ChangedShape
    } lastAction;
    switch (event.type) {
    case SDL_KEYDOWN:
        if ((lastAction != MovedRight) && (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)) {
            int lane = playerShape->getLane();
            lane++;
            if (lane > 3) lane = 1;
            playerShape->setLane(lane);
            lastAction = MovedRight;
        }
        else if ((lastAction != MovedLeft) && (event.key.keysym.scancode == SDL_SCANCODE_LEFT)) {
            int lane = playerShape->getLane();
            lane--;
            if (lane < 1) lane = 3;
            playerShape->setLane(lane);
            lastAction = MovedLeft;
        }
        else if ((lastAction != ChangedShape) && (event.key.keysym.scancode == SDL_SCANCODE_SPACE)) {
            int type = playerShape->getType();
            type = (type == NUM_SHAPES - 1) ? 0 : type + 1;
            playerShape->setType(type);
            lastAction = ChangedShape;
        }
        break;
    case SDL_KEYUP:
        lastAction = None;
        break;
    case SDL_QUIT:
        playing = false;
        break;
    }

    if (!shapes.empty()) {
        for (ShapeIter i = shapes.begin(); i != shapes.end(); i++) {
            if (!(*i)->isFalling()) {
                if (*playerShape == **i) {
                    shapes.erase(i);
                    delete *i;
                    score += 10;
                    break;
                }
                else {
                    playing = false;
                }
            }
            else {
                (*i)->move();
            }
        }
    }

    Uint32 newTime = SDL_GetTicks();
    if ((newTime - lastAddTime >= 4000)) {
        addShape(newTime);
    }
}