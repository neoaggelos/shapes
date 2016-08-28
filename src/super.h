#ifndef _super_h
#define _super_h

#include "main.h"

class Super {
private:
    Game *currentGame;
    RenderData *data;
    Settings *settings;

public:
    Super();
    ~Super();

    void mainMenu();
    void playGame();
};

#endif /* _super_h */