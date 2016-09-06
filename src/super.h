#ifndef _super_h
#define _super_h

#include "main.h"

class Super {
private:
    RenderData *data;
    Settings *settings;
	Highscores *highscores;

	Game *game;

public:
    Super();
    ~Super();

    void mainMenu();
    void playGame();
	void openSettings();

	Settings *getSettings() { return settings; }
	RenderData *getRenderData() { return data; }
	Highscores* getHighscores() { return highscores; }

	void finish();
};

#endif /* _super_h */