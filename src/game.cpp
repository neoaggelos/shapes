#include "main.h"

typedef list<Shape*>::iterator ShapeIter;

static const char *
getDescription(GameMode mode)
{
	switch (mode)
	{
	case Fake: return "Which one is it really?";
	case Reverse: return "Some might say, it's the reverse!";
	case Lots: return "How many are there?";
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

	string res = IntToString(score);
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

			(*it)->render();

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
	playerShape->render();

    SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xaa, 0xaa, 0xaa);
    SDLU_RenderText(data->getRenderer(), 0, 5,  "Score: %d", getScore());
	SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 5, "High Score: %d", h->getScore(gSuper->getSettings()->difficulty, 0));
	
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
	ChangeShapeDown,
	Quit
};

static void
right_callback(void *_this, void *_player)
{
	Shape *player = static_cast<Shape*>(_player);
	Difficulty d(gSuper->getSettings()->difficulty);
	
	int lane = player->getLane();
	lane++;
	if (lane > 3) lane = 1;
	player->setLane(lane);
}
static void
left_callback(void *_this, void *_player)
{
	Shape *player = static_cast<Shape*>(_player);
	Difficulty d(gSuper->getSettings()->difficulty);

	int lane = player ->getLane();
	lane--;
	if (lane < 1) lane = 3;
	player->setLane(lane);
}
static void
up_callback(void *_this, void *_player)
{
	Shape *player = static_cast<Shape*>(_player);
	Difficulty d(gSuper->getSettings()->difficulty);

	int type = player->getType();
	type = (type == d.numShapes() - 1) ? 0 : type + 1;
	player->setType(type);
}
static void
down_callback(void *_this, void *_player)
{
	Shape *player = static_cast<Shape*>(_player);
	Difficulty d(gSuper->getSettings()->difficulty);

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
	GameAction action = None;

	SDLU_Button *upButton, *downButton, *rightButton, *leftButton;

	upButton = SDLU_CreateButton(data->getWindow(), "Up", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(upButton, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(upButton, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(upButton, SDLU_PRESS_CALLBACK, up_callback, playerShape);
	SDLU_SetButtonHotkey(upButton, settings->changeShapeUpKey);
	SDLU_SetButtonGeometry(upButton, 240, 500, 60, 30);
	((SDLU_Styles*)upButton->content)->font_size = 15;
	((SDLU_Styles*)upButton->content)->blendmode = SDL_BLENDMODE_BLEND;
	((SDLU_Styles*)upButton->content)->box_color.a = 80;
	((SDLU_Styles*)upButton->content)->fill_color.a = 80;
	((SDLU_Styles*)upButton->content)->text_color.a = 80;

	downButton = SDLU_CreateButton(data->getWindow(), "Down", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(downButton, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(downButton, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(downButton, SDLU_PRESS_CALLBACK, down_callback, playerShape);
	SDLU_SetButtonHotkey(downButton, settings->changeShapeDownKey);
	SDLU_SetButtonGeometry(downButton, 160, 500, 60, 30);
	((SDLU_Styles*)downButton->content)->font_size = 15;
	((SDLU_Styles*)downButton->content)->blendmode = SDL_BLENDMODE_BLEND;
	((SDLU_Styles*)downButton->content)->box_color.a = 80;
	((SDLU_Styles*)downButton->content)->fill_color.a = 80;
	((SDLU_Styles*)downButton->content)->text_color.a = 80;

	rightButton = SDLU_CreateButton(data->getWindow(), "Right", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(rightButton, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(rightButton, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(rightButton, SDLU_PRESS_CALLBACK, right_callback, playerShape);
	SDLU_SetButtonHotkey(rightButton, settings->moveRightKey);
	SDLU_SetButtonGeometry(rightButton, 320, 500, 60, 30);
	((SDLU_Styles*)rightButton->content)->font_size = 15;
	((SDLU_Styles*)rightButton->content)->blendmode = SDL_BLENDMODE_BLEND;
	((SDLU_Styles*)rightButton->content)->box_color.a = 80;
	((SDLU_Styles*)rightButton->content)->fill_color.a = 80;
	((SDLU_Styles*)rightButton->content)->text_color.a = 80;

	leftButton = SDLU_CreateButton(data->getWindow(), "Left", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(leftButton, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(leftButton, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(leftButton, SDLU_PRESS_CALLBACK, left_callback, playerShape);
	SDLU_SetButtonHotkey(leftButton, settings->moveLeftKey);
	SDLU_SetButtonGeometry(leftButton, 80, 500, 60, 30);
	((SDLU_Styles*)leftButton->content)->font_size = 15;
	((SDLU_Styles*)leftButton->content)->blendmode = SDL_BLENDMODE_BLEND;
	((SDLU_Styles*)leftButton->content)->box_color.a = 80;
	((SDLU_Styles*)leftButton->content)->fill_color.a = 80;
	((SDLU_Styles*)leftButton->content)->text_color.a = 80;

	SDLU_FPS_Init(30);
	while (isPlaying()) {
		SDLU_FPS_Start();
		
		SDL_PollEvent(&event);

		SDLU_SetButtonCallback(upButton, SDLU_PRESS_CALLBACK, up_callback, playerShape);
		SDLU_SetButtonCallback(downButton, SDLU_PRESS_CALLBACK, down_callback, playerShape);
		SDLU_SetButtonCallback(rightButton, SDLU_PRESS_CALLBACK, right_callback, playerShape);
		SDLU_SetButtonCallback(leftButton, SDLU_PRESS_CALLBACK, left_callback, playerShape);

		if (mode == Reverse) {
			SDLU_SetButtonCallback(upButton, SDLU_PRESS_CALLBACK, down_callback, playerShape);
			SDLU_SetButtonCallback(downButton, SDLU_PRESS_CALLBACK, up_callback, playerShape);
			SDLU_SetButtonCallback(rightButton, SDLU_PRESS_CALLBACK, left_callback, playerShape);
			SDLU_SetButtonCallback(leftButton, SDLU_PRESS_CALLBACK, right_callback, playerShape);
		}

		switch (event.type) {
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST || event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
				action = None;
				pauseMenu();
			}
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				action = None;
				pauseMenu();
			}
			break;
		case SDL_KEYUP:
			action = None;
			break;
		case SDL_QUIT:
			gSuper->finish();
			break;
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
					double var = mode == Normal ? 1 : 1.5;
					(*i)->move(var);
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
			mode = mode == Normal ? static_cast<GameMode>(random(1, 3)) : Normal;
			lastModeChangeTime = newTime;
		}
		
		SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
		SDL_RenderClear(data->getRenderer());
		render();

		SDLU_RenderButton(upButton);
		SDLU_RenderButton(downButton);
		SDLU_RenderButton(rightButton);
		SDLU_RenderButton(leftButton);

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
	RenderData *data = gSuper->getRenderData();
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
		startTime += SDL_GetTicks() - pauseTime;
		pauseTime = 0;
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