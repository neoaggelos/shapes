#include "main.h"

static void
moveRight(Shape *player)
{
    Difficulty d(gSuper->getSettings()->difficulty);
    gSuper->getAudioData()->play("bleep");

    int lane = player->getLane();
    lane++;
    if (lane > 3) lane = 1;
    player->setLane(lane);
}

static void
moveLeft(Shape *player)
{
    Difficulty d(gSuper->getSettings()->difficulty);
    gSuper->getAudioData()->play("bleep");

    int lane = player->getLane();
    lane--;
    if (lane < 1) lane = 3;
    player->setLane(lane);
}

static void
changeUp(Shape *player)
{
    gSuper->getAudioData()->play("bleep");

    int type = player->getType();
    type = (type == 5 - 1) ? 0 : type + 1;
    player->setType(type);
}

static void
changeDown(Shape *player)
{
    gSuper->getAudioData()->play("bleep");

    int type = player->getType();
    type = (type == 0) ? 5 - 1 : type - 1;
    player->setType(type);
}

static void waitInput()
{
    SDL_Event event;
    bool done = false;

    SDL_PumpEvents();
    SDL_FlushEvents(0, (Uint32)-1);

    while (!done) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gSuper->finish();
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN) {
                done = true;
                SDL_Delay(10);
            }
        }
        SDL_Delay(1);
    }
}

Tutorial::Tutorial()
{ 
    player = new Shape();
    falling = new Shape(1, 5);
}

Tutorial::~Tutorial()
{
    delete player;
    delete falling;
}

void
Tutorial::run()
{
    RenderData *data = gSuper->getRenderData();
    SDL_Renderer *renderer = data->getRenderer();

    gSuper->getRenderData()->reloadTexture(gSuper->getSettings()->theme);

    // WELCOME
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
    player->render();
    gSuper->getTextRenderer()->write(20, "Welcome to Shapes", { 0, 200, 480, 100 }, Center);

    SDL_RenderPresent(renderer);
    waitInput();

    // GOAL
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
    player->render();
    gSuper->getTextRenderer()->write(20, "Your goal is to catch", { 0, 200, 480, 100 }, Center);
    gSuper->getTextRenderer()->write(20, "the shapes falling from above", { 0, 230, 480, 100 }, Center);

    SDL_RenderPresent(renderer);
    waitInput();

    // MOVE RIGHT AND LEFT
    bool movedLeft = false, movedRight = false;
    SDL_Event event;
    while (!movedLeft || !movedRight) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gSuper->finish();
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == gSuper->getSettings()->moveRightKey) {
                    movedRight = true;
                    moveRight(player);
                }
                else if (event.key.keysym.scancode == gSuper->getSettings()->moveLeftKey) {
                    movedLeft = true;
                    moveLeft(player);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.x > WIDTH * 0.75) {
                    movedRight = true;
                    moveRight(player);
                }
                else if (event.button.x < WIDTH * 0.25) {
                    movedLeft = true;
                    moveLeft(player);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
        player->render();

        for (int i = 0; i < 5; i++) {
            SDL_Rect src = { 80 * i, 0, 80, 80 };
            SDL_Rect dest = { 480 - 35, 510 - i * 30, 30, 30 };

            SDL_RenderCopy(renderer, data->getTexture(), &src, &dest);
            if (player->getType() == i) {
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                SDL_RenderDrawRect(renderer, &dest);
            }
        }

#ifdef __ANDROID__
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 0xff);
        SDL_RenderDrawLine(renderer, WIDTH * 0.25, 0, WIDTH * 0.25, 540);
        SDL_RenderDrawLine(renderer, WIDTH * 0.75, 0, WIDTH * 0.75, 540);
        gSuper->getTextRenderer()->write(20, "Tap right or left to move", { 0, 200, 480, 100 }, Center);
#else
        gSuper->getTextRenderer()->write(20, "Use right or left keys to move", { 0, 200, 480, 100 }, Center);
#endif /* __ANDROID__ */
        SDL_RenderPresent(renderer);
    }

    // NICE JOB
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
    player->render();
    gSuper->getTextRenderer()->write(20, "Nice job!", { 0, 200, 480, 100 }, Center);

    SDL_RenderPresent(renderer);
    waitInput();

    // CHANGE UP AND DOWN
    bool changedUp = false, changedDown = false;
    while (!changedUp || !changedDown) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gSuper->finish();
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == gSuper->getSettings()->changeShapeUpKey) {
                    changedUp = true;
                    changeUp(player);
                }
                else if (event.key.keysym.scancode == gSuper->getSettings()->changeShapeDownKey) {
                    changedDown = true;
                    changeDown(player);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.x > WIDTH * 0.25 && event.button.x < WIDTH * 0.75) {
                if (event.button.y < HEIGHT * 0.5) {
                    changedUp = true;
                    changeUp(player);
                }
                else if (event.button.y > HEIGHT * 0.5) {
                    changedDown = true;
                    changeDown(player);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
        player->render();

        for (int i = 0; i < 5; i++) {
            SDL_Rect src = { 80 * i, 0, 80, 80 };
            SDL_Rect dest = { 480 - 35, 510 - i * 30, 30, 30 };

            SDL_RenderCopy(renderer, data->getTexture(), &src, &dest);
            if (player->getType() == i) {
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                SDL_RenderDrawRect(renderer, &dest);
            }
        }

#ifdef __ANDROID__
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 0xff);
        SDL_RenderDrawLine(renderer, 120, 0, 120, 540);
        SDL_RenderDrawLine(renderer, 360, 0, 360, 540);
        SDL_RenderDrawLine(renderer, 120, 320, 360, 320);
        gSuper->getTextRenderer()->write(20, "Tap up or down to change shape", { 0, 200, 480, 100 }, Center);
#else
        gSuper->getTextRenderer()->write(20, "Use up or down keys to change shape", { 0, 200, 480, 100 }, Center);
#endif /* __ANDROID__ */
        SDL_RenderPresent(renderer);
    }

    // NICE JOB
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
    player->render();
    gSuper->getTextRenderer()->write(20, "Nice job!", { 0, 200, 480, 100 }, Center);
    gSuper->getTextRenderer()->write(20, "Now try to catch the shape", { 0, 230, 480, 100 }, Center);
    gSuper->getTextRenderer()->write(20, "using what you learned", { 0, 260, 480, 100 }, Center);

    SDL_RenderPresent(renderer);
    waitInput();

    // CATCH SHAPE
    bool caught = false;
    SDLU_FPS_Init(60);
    while (!caught) {
        SDLU_FPS_Start();
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gSuper->finish();
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == gSuper->getSettings()->changeShapeUpKey) changeUp(player);
                else if (event.key.keysym.scancode == gSuper->getSettings()->changeShapeDownKey) changeDown(player);
                else if (event.key.keysym.scancode == gSuper->getSettings()->moveLeftKey) moveLeft(player);
                else if (event.key.keysym.scancode == gSuper->getSettings()->moveRightKey) moveRight(player);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.x < WIDTH * 0.25) moveLeft(player);
                else if (event.button.x > WIDTH * 0.75) moveRight(player);
                else if (event.button.y < HEIGHT * 0.5) changeUp(player);
                else if (event.button.y > HEIGHT * 0.5) changeDown(player);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
        player->render();
        falling->render();

        for (int i = 0; i < 5; i++) {
            SDL_Rect src = { 80 * i, 0, 80, 80 };
            SDL_Rect dest = { 480 - 35, 510 - i * 30, 30, 30 };

            SDL_RenderCopy(renderer, data->getTexture(), &src, &dest);
            if (player->getType() == i) {
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                SDL_RenderDrawRect(renderer, &dest);
            }
        }

        SDL_RenderPresent(renderer);

        if (falling->getHeight() > 460) {
            caught = (*player == *falling);
            if (!caught) {
                delete falling;
                falling = new Shape(1, 5);
            }
        }
        else {
            falling->move(1);
        }

        SDLU_FPS_Cap();
    }

    // RESULT
    gSuper->getSettings()->firstRun = false;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, 0, 540, 480, 540);
    player->render();
    gSuper->getTextRenderer()->write(20, "Congratulations!", { 0, 200, 480, 100 }, Center);
    gSuper->getTextRenderer()->write(20, "You have successfully completed the tutorial!", { 0, 230, 480, 100 }, Center);
    gSuper->getTextRenderer()->write(20, "Remember to have fun!", { 0, 260, 480, 100 }, Center);

    SDL_RenderPresent(renderer);
    waitInput();
}
