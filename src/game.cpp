#include "main.h"

typedef list<Shape*>::iterator ShapeIter;

static const char *
getDescription(GameMode mode)
{
	switch (mode)
	{
	case Fast: return "Shapes are falling faster!";
	case Fake: return "Which one is it really?";
	case Reverse: return "Left is right, right is left!";
	default: return "";
	}

}

Game::Game(Super *super)
{
	parent = super;
    score = 0;

    playerShape = new Shape();

    playing = true;
	pauseTime = 0;

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

	string res = IntToString(score);
	string msg = "You scored " + res + " points. Congratulations!";

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Ended", msg.c_str(), NULL);
    
	parent->getHighscores()->addScore(parent->getSettings()->difficulty, score);
}


void
Game::addShape(Uint32 newTime)
{
	Difficulty d(parent->getSettings()->difficulty);
    lastAddTime = newTime;

    Shape * newShape = new Shape(d.shapeSpeed(), d.numShapes());

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
	Difficulty d(parent->getSettings()->difficulty);
	RenderData *data = parent->getRenderData();
	Highscores *h = parent->getHighscores();

	SDLU_SetFontSize(SDLU_TEXT_SIZE_MEDIUM);
	SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
    SDL_RenderFillRect(data->getRenderer(), NULL);

    if (!shapes.empty()) {
        for (ShapeIter it = shapes.begin(); it != shapes.end(); it++) {
			int oldType = (*it)->getType();
			bool shouldChange = mode == Fake && (*it)->getHeight() <= 300;
			if(shouldChange) {
				(*it)->setType(random(1, d.numShapes()));
			}

			(*it)->render(data);

			if (shouldChange) {
				(*it)->setType(oldType);
			}
        }
    }

    SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(data->getRenderer(), 0, 540, 640, 540);

	SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);
	double time = d.changeModeTime() /1000.0 - (pauseTime ? pauseTime - lastModeChangeTime : (SDL_GetTicks() - lastModeChangeTime)) / 1000.0;
	SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_RIGHT, 5, "%.1lf", time);

	SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 30, "%s", getDescription(mode));
	playerShape->render(data);

    SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xaa, 0xaa, 0xaa);
    SDLU_RenderText(data->getRenderer(), 0, 5,  "Score: %d", getScore());
	SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 5, "High Score: %d", h->getScore(parent->getSettings()->difficulty, 0));

    //SDL_SetRenderDrawColor(data->getRenderer(), 0xaf, 0xaf, 0xaf, 0xaf);
    //SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_RIGHT, 5, "FPS: %d", SDLU_FPS_GetRealFramerate());
	
    for (int i = 0; i < d.numShapes(); i++) {
		SDL_Rect src = { 80 * i, 0, 80, 80 };
        SDL_Rect dest = { 480 - 35, 510 - i * 30, 30, 30 };

        SDL_RenderCopy(data->getRenderer(), data->getTexture(), &src, &dest);
        if (playerShape->getType() == i) {
			SDL_SetRenderDrawColor(data->getRenderer(), random(0,255), random(0,255), random(0,255), 0xff);
            SDL_RenderDrawRect(data->getRenderer(), &dest);
        }
    }
}

void
Game::handleEvents(SDL_Event event)
{
    static enum LastAction {
        None,
        MovedRight,
        MovedLeft,
        ChangedShape
    } lastAction = None;

	Settings *settings = parent->getSettings();
	Difficulty d(settings->difficulty);

	SDL_Scancode rightKey = settings->moveRightKey;
	SDL_Scancode leftKey = settings->moveLeftKey;

	if (mode == Reverse) {
		rightKey = settings->moveLeftKey;
		leftKey = settings->moveRightKey;
	}

    switch (event.type) {
    case SDL_KEYDOWN:
		if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
			lastAction = None;
			pauseMenu();
		}
        else if ((lastAction != MovedRight) && (event.key.keysym.scancode == rightKey)) {
            int lane = playerShape->getLane();
            lane++;
            if (lane > 3) lane = 1;
            playerShape->setLane(lane);
            lastAction = MovedRight;
        }
        else if ((lastAction != MovedLeft) && (event.key.keysym.scancode == leftKey)) {
            int lane = playerShape->getLane();
            lane--;
            if (lane < 1) lane = 3;
            playerShape->setLane(lane);
            lastAction = MovedLeft;
        }
        else if ((lastAction != ChangedShape) && (event.key.keysym.scancode == settings->changeShapeKey)) {
            int type = playerShape->getType();
            type = (type == d.numShapes() - 1) ? 0 : type + 1;
            playerShape->setType(type);
            lastAction = ChangedShape;
        }
        break;
    case SDL_KEYUP:
        lastAction = None;
        break;
    case SDL_QUIT:
		parent->finish();
    }

    if (!shapes.empty()) {
        for (ShapeIter i = shapes.begin(); i != shapes.end(); i++) {
            if (!(*i)->isFalling()) {
                if (*playerShape == **i) {
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
				double var = 1.0;
				if (mode != Normal) var = 1.5;
				if (mode == Fast) var = 2.0;
                (*i)->move(var);
            }
        }
    }

    Uint32 newTime = SDL_GetTicks();
    if (newTime - lastAddTime >= d.respawnTime()) {
        addShape(newTime);
    }

	if (newTime - lastModeChangeTime >= d.changeModeTime()) {
		mode = mode == Normal ? static_cast<GameMode>(random(2, 4)) : Normal;
		lastModeChangeTime = newTime;
	}
}

void
Game::run()
{
	SDL_Event event;
	RenderData *data = parent->getRenderData();
	
	SDLU_FPS_Init(30);
	while (isPlaying()) {
		SDLU_FPS_Start();
		
		SDL_PollEvent(&event);
		handleEvents(event);
		
		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());
		render();
		SDL_RenderPresent(data->getRenderer());

		SDLU_FPS_Cap();
	}
}

void
Game::pauseMenu()
{
	pauseTime = SDL_GetTicks();

	SDL_Event event;
	SDLU_Button *resume_button, *exit_button;
	RenderData *data = parent->getRenderData();

	resume_button = SDLU_CreateButton(data->getWindow(), "Resume Game", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(resume_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(resume_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(resume_button, 140, 270, 200, 45);

	exit_button = SDLU_CreateButton(data->getWindow(), "Exit Game", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(exit_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(exit_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(exit_button, 140, 370, 200, 45);

	enum {
		Resume, BackToMenu, Exit, None
	} action = None;

	while (action == None) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDLU_BUTTON_PRESS) {
				Uint32 button_id = static_cast<Uint32>(event.user.code);
				if (button_id == resume_button->id)
					action = Resume;
				else if (button_id == exit_button->id)
					action = BackToMenu;
			}
			else if (event.type == SDL_QUIT) {
				action = Exit;
			}

			SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
			SDL_RenderClear(data->getRenderer());
			this->render();

			SDLU_SetFontSize(SDLU_TEXT_SIZE_LARGE);
			SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);
			SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 130, "Game Paused");

			SDLU_RenderButton(resume_button);
			SDLU_RenderButton(exit_button);

			SDL_RenderPresent(data->getRenderer());
		}

		//SDL_Delay(10);
	}

	SDLU_DestroyButton(resume_button);
	SDLU_DestroyButton(exit_button);

	if (action == Resume) {
		for (int i = 0; i < 3; i++) {
			SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
			SDL_RenderClear(data->getRenderer());
			
			this->render();

			SDLU_SetFontSize(SDLU_TEXT_SIZE_LARGE);
			SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);
			SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, SDLU_ALIGN_CENTER, "Resuming in %d...", 3 - i);

			SDL_RenderPresent(data->getRenderer());

			SDL_Delay(1000);
		}

		lastAddTime += SDL_GetTicks() - pauseTime;
		lastModeChangeTime += SDL_GetTicks() - pauseTime;
		pauseTime = 0;
	}
	else if (action == Exit) {
		parent->finish();
	}
	else if (action == BackToMenu) {
		playing = false;
	}
	else
	{
		std::cout << "Just slowly walk away from your PC. Don't look back in anger" << std::endl;
	}
}