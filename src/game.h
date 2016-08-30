#ifndef _game_h
#define _game_h

#include "main.h"

class Game {
private:

	Super *parent;
	int score;

	list <Shape*> shapes;

    Shape* playerShape;

    bool playing;

    int lastAddTime;

    void addShape(Uint32 newTime);

public:

	Game(Super *super);

	~Game();

	void handleEvents(SDL_Event event);
	bool isPlaying();

	void render();
    
	void pauseMenu();
	void run();
	int getScore();
};

#endif /* _game_h */