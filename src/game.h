#ifndef _game_h
#define _game_h

#include "main.h"

class Game {
private:

	int difficulty;
	int score;

	list <Shape*> shapes;

    Shape* playerShape;

    bool playing;

    int startTime;
    int lastAddTime;

    void addShape();

public:

	Game(int difficulty = 1);

	~Game();

	void handleEvents(SDL_Event event);
	bool isPlaying();

	void render(RenderData *data);

	int getTime();
	int getScore();
};

#endif /* _game_h */