#include "main.h"

double
Difficulty::shapeSpeed()
{
    switch (level) {
    case Easy: return 1.5;
	case Medium: return 2;
	case Hard: return 2 * random(1,1);
    }

    return 1;
}

Uint32
Difficulty::respawnTime()
{
    switch (level)
    {
    case Easy: return 3000;
    case Medium: return 1800;
    case Hard: return random(2000, 2500);
    }

    return 4000;
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
