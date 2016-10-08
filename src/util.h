#ifndef _util_h
#define _util_h

#include "main.h"
using namespace std;

int random(int min, int max);
void SDL_CHECK(bool check, string msg = "");
string getAssetsDir(string subfolder = "");
const char* getSettingsDir();

string int_to_string(int var);
string double_to_string(double var);
int to_int(string s);
int to_int(const char *s);

void on_hover(void *_button, void* _unused);

SDLU_Button * CreateButton(
    const char* name, const char * title,
    SDL_Rect pos, int fontsize = 20,
    SDLU_Callback press = NULL, void * press_arg = NULL,
    SDL_Scancode hotkey = SDL_SCANCODE_UNKNOWN,
    SDLU_Callback hover = on_hover, void * hover_arg = NULL);

SDLU_ComboBox * CreateComboBox(string items[], int n, string active, int x, int y, int w = -1, int h = -1, int fontsize = 15);
SDLU_ComboBox * CreateComboBox(string items[], int n, int active, int x, int y, int w = -1, int h = -1, int fontsize = 15);

SDL_Rect SDL_RECT(int x, int y, int w, int h);
SDL_Color SDL_COLOR(Uint8 r, Uint8 g, Uint8 b, Uint8 a);


#endif /* _util_h */
