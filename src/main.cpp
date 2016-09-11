#include "main.h"

Super *gSuper;
int gWidth, gHeight;

int main(int argc, char **argv)
{
    gSuper = new Super;

    gSuper->mainMenu();

    delete gSuper;

    return 0;
}
