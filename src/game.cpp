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
	case Lots: return "How many are there?";
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
	string msg = "You scored " + res + " points.";

	int diff = parent->getSettings()->difficulty;

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Ended", msg.c_str(), NULL);

	if (parent->getHighscores()->getScore(diff, 4) <= score && score > 0) {
		msg += "You set a new high score! Congratulations!";
		EnterName *nameDialog = new EnterName(parent, "Enter your name:");

		string name = nameDialog->openDialog();

		delete nameDialog;

		int s = parent->getHighscores()->addScore(diff, score, name);
	}
    
}


void
Game::addShape(Uint32 newTime)
{
	Difficulty d(parent->getSettings()->difficulty);
    lastAddTime = newTime;
	
	double shapeSpeed = d.shapeSpeed();
	shapeSpeed += 0.5 * (SDL_GetTicks() / 60000);
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

enum GameAction {
	None,
	MoveRight,
	MoveLeft,
	ChangeShapeUp,
	ChangeShapeDown
};


void
Game::handleEvents(SDL_Event event)
{
	static GameAction action = None;
	static bool executedAction = false;
	static bool handledMotionEvent = false;

	Settings *settings = parent->getSettings();
	Difficulty d(settings->difficulty);

	SDL_Scancode rightKey = settings->moveRightKey;
	SDL_Scancode leftKey = settings->moveLeftKey;
	SDL_Scancode upKey = settings->changeShapeUpKey;
	SDL_Scancode downKey = settings->changeShapeDownKey;

	if (mode == Reverse) {
		rightKey = settings->moveLeftKey;
		leftKey = settings->moveRightKey;

		upKey = settings->changeShapeDownKey;
		downKey = settings->changeShapeUpKey;
	}

    switch (event.type) {
    case SDL_KEYDOWN:
		handledMotionEvent = false;
		if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
			action = None;
			executedAction = true;
			pauseMenu();
		}
        else if ((action != MoveRight) && (event.key.keysym.scancode == rightKey)) {
			action = MoveRight;
			executedAction = false;
        }
        else if ((action != MoveLeft) && (event.key.keysym.scancode == leftKey)) {
			action = MoveLeft;
			executedAction = false;
        }
        else if ((action != ChangeShapeUp) && (event.key.keysym.scancode == upKey)) {
			action = ChangeShapeUp;
			executedAction = false;
        }
		else if ((action != ChangeShapeDown) && (event.key.keysym.scancode == downKey)) {
			action = ChangeShapeDown;
			executedAction = false;
		}
        break;
    case SDL_KEYUP:
        action = None;
		handledMotionEvent = false;
        break;
	case SDL_MOUSEMOTION:
		int xrel, yrel;
		xrel = event.motion.xrel;
		yrel = event.motion.yrel;

		if (event.motion.state != SDL_BUTTON(SDL_BUTTON_LEFT) || handledMotionEvent) {
			break;
		}

		if (SDL_abs(xrel) < 10 && yrel < -30) {
			action = ChangeShapeUp;
			executedAction = false;
			handledMotionEvent = true;
		}
		else if (SDL_abs(xrel) < 10 && yrel > 30) {
			action = ChangeShapeDown;
			executedAction = false;
			handledMotionEvent = true;
		}
		else if (SDL_abs(yrel) < 10 && xrel > 30) {
			action = MoveRight;
			executedAction = false;
			handledMotionEvent = true;
		}
		else if (SDL_abs(yrel) < 10 && xrel < -30) {
			action = MoveLeft;
			executedAction = false;
			handledMotionEvent = true;
		}
		else {
			handledMotionEvent = false;
		}
		break;
    case SDL_QUIT:
		parent->finish();
	default:
		handledMotionEvent = false;
    }

	if (!executedAction) {
		executedAction = true;

		if (action == MoveRight) {
			int lane = playerShape->getLane();
			lane++;
			if (lane > 3) lane = 1;
			playerShape->setLane(lane);
		}
		else if (action == MoveLeft) {
			int lane = playerShape->getLane();
			lane--;
			if (lane < 1) lane = 3;
			playerShape->setLane(lane);
		}
		else if (action == ChangeShapeUp) {
			int type = playerShape->getType();
			type = (type == d.numShapes() - 1) ? 0 : type + 1;
			playerShape->setType(type);
		}
		else if (action == ChangeShapeDown) {
			int type = playerShape->getType();
			type = (type == 0) ? d.numShapes() - 1 : type - 1;
			playerShape->setType(type);
		}
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
	double respawnTime = d.respawnTime() * ((mode == Lots) ? 0.5 : 1);
    if (newTime - lastAddTime >= respawnTime) {
        addShape(newTime);
    }

	if (newTime - lastModeChangeTime >= d.changeModeTime()) {
		mode = mode == Normal ? static_cast<GameMode>(random(2, 5)) : Normal;
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

enum PauseMenuAction {
	Resume, Forfeit, Exit, Idle
};

static void
resume_callback(void *_button, void *action)
{
	*(static_cast<PauseMenuAction*>(action)) = Resume;
}

static void
forfeit_callback(void *_button, void *action)
{
	*(static_cast<PauseMenuAction*>(action)) = Forfeit;
}

void
Game::pauseMenu()
{
	pauseTime = SDL_GetTicks();

	SDL_Event event;
	SDLU_Button *resume_button, *forfeit_button;
	RenderData *data = parent->getRenderData();
	PauseMenuAction action = Idle;

	resume_button = SDLU_CreateButton(data->getWindow(), "Resume Game", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(resume_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(resume_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(resume_button, SDLU_PRESS_CALLBACK, resume_callback, &action);
	SDLU_SetButtonGeometry(resume_button, 140, 270, 200, 45);

	forfeit_button = SDLU_CreateButton(data->getWindow(), "Forfeit Game", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(forfeit_button, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(forfeit_button, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(forfeit_button, SDLU_PRESS_CALLBACK, forfeit_callback, &action);
	SDLU_SetButtonGeometry(forfeit_button, 140, 370, 200, 45);

	while (action == Idle) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				action = Exit;
			}

			SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
			SDL_RenderClear(data->getRenderer());
			this->render();

			SDLU_SetFontSize(SDLU_TEXT_SIZE_LARGE);
			SDL_SetRenderDrawColor(data->getRenderer(), 0xaa, 0xaa, 0xaa, 0xff);
			SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 130, "Game Paused");

			SDLU_RenderButton(resume_button);
			SDLU_RenderButton(forfeit_button);

			SDL_RenderPresent(data->getRenderer());
		}

		//SDL_Delay(10);
	}

	SDLU_DestroyButton(resume_button);
	SDLU_DestroyButton(forfeit_button);

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
	else if (action == Forfeit) {
		playing = false;
	}
	else
	{
		std::cout << "Just slowly walk away from your PC. Don't look back in anger" << std::endl;
	}
}