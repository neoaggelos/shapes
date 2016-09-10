#include "main.h"

Super *gSuper;
int gWidth, gHeight;

int SDL_main(int argc, char **argv)
{
    gSuper = new Super;

    gSuper->mainMenu();

    delete gSuper;

    return 0;
}