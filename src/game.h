#ifndef _game_h
#define _game_h

#include "main.h"

class Game {
private:

	Difficulty d;
	int score;

	list <Shape*> shapes;

    Shape* playerShape;

    bool playing;

    int startTime;
    int lastAddTime;

    void addShape(Uint32 newTime);

public:

	Game(DifficultyLevel difficulty = Easy);

	~Game();

	void handleEvents(SDL_Event event, Settings *settings);
	bool isPlaying();

	void render(RenderData *data);
    
	int getTime();
	int getScore();
};

#endif /* _game_h */