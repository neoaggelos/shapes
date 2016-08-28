#include "main.h"

double
Difficulty::shapeSpeed()
{
    switch (level) {
    case Easy: return 1.5;
    case Medium: return 1.5 * random(1,2);
    case Hard: return 1.5 * random(1,3);
    }

    return 1;
}

Uint32
Difficulty::respawnTime()
{
    switch (level)
    {
    case Easy: return 4000;
    case Medium: return 3000;
    case Hard: return random(2000, 3000);
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