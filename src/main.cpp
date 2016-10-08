#include "main.h"

#if !SDL_VERSION_ATLEAST(2,0,5)
#warning "Requires SDL2 version 2.0.5 or higher"
#endif

Super *gSuper;
int gWidth, gHeight;

int main(int argc, char **argv)
{
    gSuper = new Super;

    gSuper->mainMenu();

    delete gSuper;

    return 0;
}
