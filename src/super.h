#ifndef _super_h
#define _super_h

#include "main.h"

class Super {
private:
    Settings *settings;
    RenderData *render;
    TextRenderer *textrenderer;
    Highscores *highscores;
    AudioData *audio;

    Game *game;

public:
    Super();
    ~Super();

    void mainMenu();
    void playGame();
    void openSettings();

    Settings *getSettings() { return settings; }
    RenderData *getRenderData() { return render; }
    TextRenderer* getTextRenderer() { return textrenderer; }
    Highscores* getHighscores() { return highscores; }
    AudioData* getAudioData() { return audio; }

    void finish();
};

#endif /* _super_h */
