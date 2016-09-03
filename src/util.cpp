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

int StringToInt(const char* str)
{
	int j = 0;
	for (unsigned int i = 0; i < SDL_strlen(str); i++) {
		j = 10 * j + str[i] - '0';
	}

	return j;
}

char* IntToString(int i)
{
	string ret;
	do {
		ret = static_cast<char>(i % 10 + '0') + ret;
	} while (i /= 10);

	return SDL_strdup(ret.c_str());
}
