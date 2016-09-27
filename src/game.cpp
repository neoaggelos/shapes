#include "main.h"

typedef list<Shape*>::iterator ShapeIter;

static const char *
getDescription(GameMode mode)
{
	switch (mode)
	{
	case Fake: return "Which one is it really?";
	case Reverse: return "Some would say, it's the reverse!";
	case Lots: return "How many are there?";
	case Shaking: return "Right or left, left or right?";
	default: return "";
	}

}

Game::Game()
{
    score = 0;

    playerShape = new Shape();

    playing = true;
	pauseTime = 0;
	startTime = SDL_GetTicks();

    lastAddTime = SDL_GetTicks();
    addShape(lastAddTime);

	lastModeChangeTime = SDL_GetTicks();
	mode = Normal;
}

Game::~Game()
{
	if (!shapes.empty()) {
		for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
			delete (*it);
		}
	}

	string res = int_to_string(score);
	string msg = "You scored " + res + " points.";

	int diff = gSuper->getSettings()->difficulty;

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Ended", msg.c_str(), NULL);

	if (gSuper->getHighscores()->getScore(diff, 4) <= score && score > 0) {
		msg += "You set a new high score! Congratulations!";
		EnterName *nameDialog = new EnterName("Enter your name:");

		string name = nameDialog->openDialog();

		delete nameDialog;

		int s = gSuper->getHighscores()->addScore(diff, score, name);
	}
    
}


void
Game::addShape(Uint32 newTime)
{
	Difficulty d(gSuper->getSettings()->difficulty);
    lastAddTime = newTime;
	
	double shapeSpeed = d.shapeSpeed();
	shapeSpeed += 0.2 * ((SDL_GetTicks() - startTime) / 60000);
	Shape * newShape = new Shape(shapeSpeed, d.numShapes());

    shapes.push_back(newShape);
}

bool
Game::isPlaying()
{
    return playing;
}

int
Game::getScore()
{
    return score;
}

void
Game::render()
{
	Difficulty d(gSuper->getSettings()->difficulty);
	RenderData *data = gSuper->getRenderData();
	Highscores *h = gSuper->getHighscores();

	SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
    SDL_RenderFillRect(data->getRenderer(), NULL);

    if (!shapes.empty()) {
        for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
			(*it)->render();
        }
    }

    SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(data->getRenderer(), 0, 540, 640, 540);

	SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);
	double time = d.changeModeTime() /1000.0 - (pauseTime ? pauseTime - lastModeChangeTime : (SDL_GetTicks() - lastModeChangeTime)) / 1000.0;
	time = static_cast<int>(time * 10) / 10.0; /* keep one digit after dot */
	gSuper->getTextRenderer()->write(20, double_to_string(time), { 0, 5, 475, 100 }, Right);
	
	if (mode != Normal)
		gSuper->getTextRenderer()->write(20, getDescription(mode), { 0, 30, 480, 100 }, Center);

    SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xaa, 0xaa, 0xaa);
	gSuper->getTextRenderer()->write(20, "Score: " + int_to_string(getScore()), 0, 5);
	gSuper->getTextRenderer()->write(20, "High Score: " + int_to_string(h->getScore(gSuper->getSettings()->difficulty, 0)), { 0, 5, 480, 100 }, Center);
	
	playerShape->render();

    for (int i = 0; i < d.numShapes(); i++) {
		SDL_Rect src = { 80 * i, 0, 80, 80 };
        SDL_Rect dest = { 480 - 35, 510 - i * 30, 30, 30 };

        SDL_RenderCopy(data->getRenderer(), data->getTexture(), &src, &dest);
        if (playerShape->getType() == i) {
			SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
            SDL_RenderDrawRect(data->getRenderer(), &dest);
        }
    }
}

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

	int lane = player ->getLane();
	lane--;
	if (lane < 1) lane = 3;
	player->setLane(lane);
}

static void
changeUp(Shape *player)
{
	Difficulty d(gSuper->getSettings()->difficulty);
	gSuper->getAudioData()->play("bleep");

	int type = player->getType();
	type = (type == d.numShapes() - 1) ? 0 : type + 1;
	player->setType(type);
}

static void
changeDown(Shape *player)
{
	Difficulty d(gSuper->getSettings()->difficulty);
	gSuper->getAudioData()->play("bleep");

	int type = player->getType();
	type = (type == 0) ? d.numShapes() - 1 : type - 1;
	player->setType(type);
}

void
Game::run()
{
	SDL_Event event;
	RenderData *data = gSuper->getRenderData();
	Settings *settings = gSuper->getSettings();
	Difficulty d(settings->difficulty);

	SDL_PumpEvents();
	SDL_FlushEvents((Uint32)0, (Uint32)-1);
	SDLU_FPS_Init(60);
	while (isPlaying()) {
		SDLU_FPS_Start();

		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST || event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
					pauseMenu();
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.x > gWidth * 0.75)
					mode != Reverse ? moveRight(playerShape) : moveLeft(playerShape);
				else if (event.button.x < gWidth * 0.25)
					mode != Reverse ? moveLeft(playerShape) : moveRight(playerShape);
				else if (event.button.y > gHeight * 0.5)
					mode != Reverse ? changeDown(playerShape) : changeUp(playerShape);
				else
					mode != Reverse ? changeUp(playerShape) : changeDown(playerShape);
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE || event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
					pauseMenu();
				else if (event.key.keysym.scancode == settings->moveRightKey)
					mode != Reverse ? moveRight(playerShape) : moveLeft(playerShape);
				else if (event.key.keysym.scancode == settings->moveLeftKey)
					mode != Reverse ? moveLeft(playerShape) : moveRight(playerShape);
				else if (event.key.keysym.scancode == settings->changeShapeDownKey)
					mode != Reverse ? changeDown(playerShape) : changeUp(playerShape);
				else if (event.key.keysym.scancode == settings->changeShapeUpKey)
					mode != Reverse ? changeUp(playerShape) : changeDown(playerShape);
				break;
			case SDL_QUIT:
				gSuper->finish();
				break;
			}
		}


		if (!shapes.empty()) {
			for (ShapeIter i = shapes.begin(); i != shapes.end(); i++) {
				if (!(*i)->isFalling()) {
					if (*playerShape == **i) {
						gSuper->getAudioData()->play("whoosh");
						delete *i;
						shapes.erase(i);
						score += d.score();
						break;
					}
					else {
						playing = false;
					}
				}
				else {
					double var = mode == Normal ? 1 : 1.5;
					(*i)->move(var);

					if (mode == Fake) {
						if (random(1, 100) < 20 && pauseTime == 0 && (*i)->getHeight() < 300) {
							(*i)->setType(random(0, d.numShapes()-1));
						}
					}
					else if (mode == Shaking) {
						if (random(1, 100) < 5 && pauseTime == 0 && (*i)->getHeight() < 300) {
							(*i)->setLane(random(1, 3));
						}
					}
				}
			}
		}

		Uint32 newTime = SDL_GetTicks();
		double respawnTime = d.respawnTime();
		respawnTime *= ((mode == Lots) ? 0.5 : 1);
		if (newTime - lastAddTime >= respawnTime) {
			addShape(newTime);
		}

		if (newTime - lastModeChangeTime >= d.changeModeTime()) {
			mode = mode == Normal ? static_cast<GameMode>(random(1, NumModes-1)) : Normal;
			lastModeChangeTime = newTime;
		}
		
		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());
		render();
		SDL_RenderPresent(data->getRenderer());

		SDLU_FPS_Cap();
	}
}

enum PauseMenuAction {
	Resume, Forfeit, Exit, Idle
};

static void
resume_callback(void *_button, void *action)
{
	*(static_cast<PauseMenuAction*>(action)) = Resume;
	gSuper->getAudioData()->play("bleep");
}

static void
forfeit_callback(void *_button, void *action)
{
	*(static_cast<PauseMenuAction*>(action)) = Forfeit;
	gSuper->getAudioData()->play("bleep");
}

void
Game::pauseMenu()
{
	pauseTime = SDL_GetTicks();
	gSuper->getAudioData()->play("bleep");

	SDL_Event event;
	SDLU_Button *resume_button, *forfeit_button;
	RenderData *data = gSuper->getRenderData();
	PauseMenuAction action = Idle;

	resume_button = CreateButton("Resume Game", { 140, 270, 200, 45 }, 20, resume_callback, &action, SDL_SCANCODE_ESCAPE);
	forfeit_button = CreateButton("Forfeit Game", { 140, 370, 200, 45 }, 20, forfeit_callback, &action);

	while (action == Idle) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				action = Exit;
			}
		}

		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());
		this->render();

		SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);
		gSuper->getTextRenderer()->write(42, "Game Paused", { 0, 130, 480, 100 }, Center);

		SDLU_RenderButton(resume_button);
		SDLU_RenderButton(forfeit_button);

		SDL_RenderPresent(data->getRenderer());

		SDL_Delay(1);
	}

	SDLU_DestroyButton(resume_button);
	SDLU_DestroyButton(forfeit_button);

	if (action == Resume) {
		for (int i = 0; i < 3; i++) {
			SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
			SDL_RenderClear(data->getRenderer());
			
			this->render();

			SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);

			gSuper->getTextRenderer()->write(42, "Resuming in " + int_to_string(3 - i) + "...", { 0,0,480,640 }, Center, Center);

			SDL_RenderPresent(data->getRenderer());

			SDL_Delay(1000);
		}

		lastAddTime += SDL_GetTicks() - pauseTime;
		lastModeChangeTime += SDL_GetTicks() - pauseTime;
		startTime += SDL_GetTicks() - pauseTime;
		pauseTime = 0;
		SDL_PumpEvents();
		SDL_FlushEvents((Uint32)0, (Uint32)-1);
	}
	else if (action == Exit) {
		gSuper->finish();
	}
	else if (action == Forfeit) {
		playing = false;
	}
	else
	{
		std::cout << "Just slowly walk away from your PC. Don't look back in anger" << std::endl;
	}
}