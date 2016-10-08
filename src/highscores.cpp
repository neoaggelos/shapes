#include "main.h"

Highscores::Highscores()
{
    SDLU_IniHandler *ini;
    const char *scoresPath = getSettingsDir();
    savingScores = scoresPath != NULL;

    scoresIni = string(scoresPath) + "scores.ini";
    ini = SDLU_LoadIni(scoresIni.c_str());

    bool isOK = true;
    for (int diff = 0; diff < 3 && isOK; diff++) {
        for (int i = 0; i < 5 && isOK; i++) {
            int p = diff * 10 + i;

            const char *name = SDLU_GetIniProperty(ini, "names", int_to_string(p).c_str());
            const char *score = SDLU_GetIniProperty(ini, "scores", int_to_string(p).c_str());

            isOK = name != NULL && score != NULL;
            if (isOK) {
                scores[diff][i] = to_int(score);
                names[diff][i] = SDL_strdup(name); //TODO
            }
        }
    }

    if (!isOK) {
        for (int diff = 0; diff < 3; diff++) {
            for (int i = 0; i < 5; i++) {
                scores[diff][i] = 0;
                names[diff][i] = "no name";
            }
        }
    }
    
    if (ini)
        SDLU_DestroyIni(ini);
}

Highscores::~Highscores()
{
    if (savingScores) {
        SDLU_IniHandler *ini = SDLU_CreateIni();
        
        for (int diff = 0; diff < 3; diff++) {
            for (int i = 0; i < 5; i++) {
                int p = diff * 10 + i;
                SDLU_SetIniProperty(&ini, "scores", int_to_string(p).c_str(), int_to_string(scores[diff][i]).c_str());
                SDLU_SetIniProperty(&ini, "names", int_to_string(p).c_str(), names[diff][i].c_str());
            }
        }

        SDLU_SaveIni(ini, scoresIni.c_str());

        SDLU_DestroyIni(ini);
    }
}

int
Highscores::addScore(int diff, int score, string name)
{
    int index = -1;
    
    diff--;
    if (diff < 0 || diff >= 3)
        return -1;

    for (int i = 0; i < 5; i++) {
        if (score >= scores[diff][i]) {
            /* shift scores */
            for (int j = 4; j >= i; j--) {
                scores[diff][j] = scores[diff][j - 1];
                names[diff][j] = names[diff][j - 1];
            }

            index = i;
            scores[diff][index] = score;
            names[diff][index] = name;

            break;
        }
    }

    return index;
}

enum HighscoresMenuAction {
    PlayAgain, BackToMenu, Quit, None
};

static void
callback(void *_button, void *arg)
{
    gSuper->getAudioData()->play("bleep");
    const char* name = ((SDLU_Button*)_button)->name;

    if (SDL_strcmp(name, "again") == 0) {
        *(static_cast<HighscoresMenuAction*>(arg)) = PlayAgain;
    }
    else if (SDL_strcmp(name, "back") == 0) {
        *(static_cast<HighscoresMenuAction*>(arg)) = BackToMenu;
    }
    else if (SDL_strcmp(name, "right") == 0) {
        int *diff = static_cast<int*>(arg);
        if (*diff < 2) *diff += 1;
    }
    else if (SDL_strcmp(name, "left") == 0) {
        int *diff = static_cast<int*>(arg);
        if (*diff > 0) *diff -= 1;
    }
}

void
Highscores::openMenu(int currentdiff, int currentindex)
{
    SDLU_Button *again_button, *back_button, *right_button, *left_button;
    SDL_Event event;
    HighscoresMenuAction action = None;
    int diff = static_cast<int>(gSuper->getSettings()->difficulty) - 1;

    RenderData* data = gSuper->getRenderData();

    again_button = CreateButton("again", "I can beat that", SDL_RECT(140, 480, 200, 40), 20, callback, &action);
    back_button = CreateButton("back", "Back To Menu", SDL_RECT(140, 540, 200, 40), 20, callback, &action, SDL_SCANCODE_AC_BACK);
    right_button = CreateButton("right", ">", SDL_RECT(350, 100, 30, 30), 20, callback, &diff, SDL_SCANCODE_RIGHT);
    left_button = CreateButton("left", "<", SDL_RECT(100, 100, 30, 30), 20, callback, &diff, SDL_SCANCODE_LEFT);
    
    ((SDLU_Styles*)left_button->content)->fill_color = SDL_COLOR(0, 0, 0, 0xff);
    ((SDLU_Styles*)right_button->content)->fill_color = SDL_COLOR(0, 0, 0, 0xff);

    while (action == None) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                action = Quit;
        }

        SDL_SetRenderDrawColor(data->getRenderer(), 0, 0, 0, 0);
        SDL_RenderClear(data->getRenderer());

        SDLU_RenderButton(again_button);
        SDLU_RenderButton(back_button);
        SDLU_RenderButton(right_button);
        SDLU_RenderButton(left_button);

        SDL_SetRenderDrawColor(data->getRenderer(), 0xff, 0xff, 0xff, 0xff);
        gSuper->getTextRenderer()->write(20, "HIGH SCORES", SDL_RECT(0, 20, 480, 100), Center);
        SDL_RenderDrawLine(data->getRenderer(), 140, 60, 340, 60);

        gSuper->getTextRenderer()->write(20, difficultyName(diff), SDL_RECT(0, 100, 480, 30), Center, Center);

        SDL_RenderDrawLine(data->getRenderer(), 130, 100, 350, 100);
        SDL_RenderDrawLine(data->getRenderer(), 130, 129, 350, 129);

        int ypos = 170;
        for (int i = 0; i < 5; i++) {
            if (currentdiff == diff && currentindex == i) {
                SDL_SetRenderDrawColor(data->getRenderer(), 0x00, 0xff, 0x00, 0xff);
            }
            else if (i == 0) {
                SDL_SetRenderDrawColor(data->getRenderer(), 0xcc, 0xcc, 0xcc, 0xff);
            }
            else {
                SDL_SetRenderDrawColor(data->getRenderer(), 0x88, 0x88, 0x88, 0xff);
            }

            gSuper->getTextRenderer()->write(20, int_to_string(i + 1), 100, ypos);
            gSuper->getTextRenderer()->write(20, names[diff][i], SDL_RECT(0, ypos, 480, 100), Center);
            gSuper->getTextRenderer()->write(20, int_to_string(scores[diff][i]), SDL_RECT(0, ypos, 380, 100), Right);

            ypos += 60;
        }

        SDL_RenderPresent(data->getRenderer());

        SDL_Delay(1);
    }

    SDLU_DestroyButton(again_button);
    SDLU_DestroyButton(back_button);

    if (action == Quit)
        gSuper->finish();
    else if (action == PlayAgain) {
        gSuper->playGame();
    }
    else if (action == BackToMenu) {
        gSuper->mainMenu();
    }
}

int
Highscores::getScore(int diff, int index)
{
    diff--;
    if (index < 0 || index >= 5 || diff < 0 || diff >= 3)
        return -1;

    return scores[diff][index];
}
