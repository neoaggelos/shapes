#include "main.h"

typedef list<Shape*>::iterator ShapeIter;

static char* IntToString(int i)
{
	string ret;
	do {
		ret = static_cast<char>(i % 10 + '0') + ret;
	} while (i /= 10);

	return SDL_strdup(ret.c_str());
}

Game::Game(Super *super)
{
	parent = super;
    score = 0;

    playerShape = new Shape();

    playing = true;

    lastAddTime = SDL_GetTicks();
    addShape(lastAddTime);
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
    
	int i = parent->getHighscores()->addScore(score);
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
            (*it)->render(data);
        }
    }

    SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(data->getRenderer(), 0, 540, 640, 540);

    playerShape->render(data);

    SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xaa, 0xaa, 0xaa);
    SDLU_RenderText(data->getRenderer(), 0, 5,  "Score: %d", getScore());
	SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_CENTER, 5, "High Score: %d", h->getScore(0));


    SDL_SetRenderDrawColor(data->getRenderer(), 0xaf, 0xaf, 0xaf, 0xaf);
    SDLU_RenderText(data->getRenderer(), SDLU_ALIGN_RIGHT, 5, "FPS: %d", SDLU_FPS_GetRealFramerate());
	
    for (int i = 0; i < d.numShapes(); i++) {
        SDL_Rect dest = { 480 - 25, 520 - i * 20, 20, 20 };

        SDL_RenderCopy(data->getRenderer(), data->getTexture(i), NULL, &dest);
        if (playerShape->getType() == i) {
			SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xff, 0x00, 0xff);
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
        ChangedShapeUp,
        ChangedShapeDown
    } lastAction = None;

	Settings *settings = parent->getSettings();
	Difficulty d(settings->difficulty);

    switch (event.type) {
    case SDL_KEYDOWN:
		if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
			pauseMenu();
		}
        else if ((lastAction != MovedRight) && (event.key.keysym.scancode == settings->moveRightKey)) {
            int lane = playerShape->getLane();
            lane++;
            if (lane > 3) lane = 1;
            playerShape->setLane(lane);
            lastAction = MovedRight;
        }
        else if ((lastAction != MovedLeft) && (event.key.keysym.scancode == settings->moveLeftKey)) {
            int lane = playerShape->getLane();
            lane--;
            if (lane < 1) lane = 3;
            playerShape->setLane(lane);
            lastAction = MovedLeft;
        }
        else if ((lastAction != ChangedShapeUp) && (event.key.keysym.scancode == settings->changeShapeUpKey)) {
            int type = playerShape->getType();
            type = (type == d.numShapes() - 1) ? 0 : type + 1;
            playerShape->setType(type);
            lastAction = ChangedShapeUp;
        }
        else if ((lastAction != ChangedShapeDown) && (event.key.keysym.scancode == settings->changeShapeDownKey)) {
            int type = playerShape->getType();
            type = (type == 0) ? d.numShapes() - 1 : type - 1;
            playerShape->setType(type);
            lastAction = ChangedShapeDown;
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
                (*i)->move();
            }
        }
    }

    Uint32 newTime = SDL_GetTicks();
    if (newTime - lastAddTime >= d.respawnTime()) {
        addShape(newTime);
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
	int pauseTime = SDL_GetTicks();

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

		SDL_Delay(10);
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