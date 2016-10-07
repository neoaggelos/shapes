#include "main.h"

double
Difficulty::shapeSpeed()
{
    switch (level) {
    case Easy: return 0.5;
    case Medium: return 1;
    case Hard: return 1;
    }

    return 1;
}

Uint32
Difficulty::respawnTime()
{
    return 3000;
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

DifficultyLevel difficultyLevel(string level)
{
    if (level == "Easy") return Easy;
    if (level == "Medium") return Medium;
    if (level == "Hard") return Hard;

    return Medium;
}

const char* difficultyName(int level)
{
    if (level == 0) return "Easy";
    if (level == 1) return "Medium";
    if (level == 2) return "Hard";

    return "Medium";
}
