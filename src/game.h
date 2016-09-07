#ifndef _game_h
#define _game_h

#include "main.h"

enum GameMode {
	Normal=1,
	Fast=2,
	Reverse=3,
	Fake=4,
	Lots=5
};

class Game {
private:
	int score;

	list <Shape*> shapes;
    Shape* playerShape;

    bool playing;
    int lastAddTime;
	int pauseTime;

	GameMode mode;
	int lastModeChangeTime;

    void addShape(Uint32 newTime);

public:

	Game();

	~Game();

	void handleEvents(SDL_Event event);
	bool isPlaying();

	void render();
    
	void pauseMenu();
	void run();
	int getScore();
};

#endif /* _game_h */