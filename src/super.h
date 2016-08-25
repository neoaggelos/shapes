#ifndef _super_h
#define _super_h

#include "main.h"

class Super {
private:
    Game *currentGame;
    RenderData *data;

public:
    Super();
    ~Super();

    void mainMenu();
    void playGame(int difficulty);
};

#endif /* _super_h */