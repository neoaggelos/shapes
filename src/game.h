#ifndef _game_h
#define _game_h

#include "main.h"

enum GameMode {
    Normal,
    Reverse,
    Fake,
    Lots,
    Shaking,
    NumModes  /* not actual mode, it is here to keep track of the number of modes */
};

class Game {
private:
    int score;

    list <Shape*> shapes;
    Shape* playerShape;

    bool playing;
    int lastAddTime;
    int pauseTime;
    int startTime;

    GameMode mode;
    int lastModeChangeTime;

    void addShape(Uint32 newTime);

public:

    Game();

    ~Game();

    bool isPlaying();

    void render();
    
    void pauseMenu();
    void run();
    int getScore();
};

#endif /* _game_h */
