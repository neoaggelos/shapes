#include "main.h"

int random(int min, int max)
{
	static bool hasRun = false;

	if (!hasRun) {
		srand((unsigned int)time(NULL));
		hasRun = true;
	}

	return rand() % (max-min+1) + min;
}