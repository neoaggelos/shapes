#include "main.h"

double
Difficulty::shapeSpeed()
{
    switch (level) {
    case Easy: return 1;
	case Medium: return 2;
	case Hard: return 2;
    }

    return 1;
}

Uint32
Difficulty::respawnTime()
{
	return 3000 - 200 * (SDL_GetTicks() / 60000);
}

Uint32
Difficulty::changeModeTime()
{
	return 15000;
}

int
Difficulty::score()
{
    return 5 + 5 * level;
}

int
Difficulty::numShapes()
{
    return level * 2 + 1;
}
