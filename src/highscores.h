#ifndef _highscores_h
#define _highscores_h

#include "main.h"

class Highscores {
private:
	Super *parent;
	int scores[5];
	bool savingScores;
	string scoresIni;

public:
	Highscores(Super *super);
	~Highscores();

	int addScore(int score);
	void openMenu(int current = -1);

	int getScore(int index);
};

#endif /* _highscores_h */