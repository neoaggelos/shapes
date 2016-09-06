#include "main.h"

int SDL_main(int argc, char **argv)
{
    Super *s = new Super;

    s->mainMenu();

    delete s;

    return 0;
}