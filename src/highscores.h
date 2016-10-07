#ifndef _highscores_h
#define _highscores_h

#include "main.h"

class Highscores {
private:
    int scores[3][5];
    string names[3][5];
    bool savingScores;
    string scoresIni;

public:
    Highscores();
    ~Highscores();

    int addScore(int diff, int score, string name);
    void openMenu(int diff = -1, int current = -1);

    int getScore(int diff, int index);
};

#endif /* _highscores_h */
