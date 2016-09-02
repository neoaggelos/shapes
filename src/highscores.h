#ifndef _highscores_h
#define _highscores_h

#include "main.h"

class Highscores {
private:
	Super *parent;
	int scores[3][5];
	bool savingScores;
	string scoresIni;

public:
	Highscores(Super *super);
	~Highscores();

	int addScore(int diff, int score);
	void openMenu(int diff = -1, int current = -1);

	int getScore(int diff, int index);
};

#endif /* _highscores_h */