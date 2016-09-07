#include "main.h"

Super *gSuper;

int SDL_main(int argc, char **argv)
{
    gSuper = new Super;

    gSuper->mainMenu();

    delete gSuper;

    return 0;
}