#include "main.h"

#if defined(__WIN32__) || defined(__WINRT__)
const std::string PATHSEP = "\\";
#else
const std::string PATHSEP = "/";
#endif

int random(int min, int max)
{
	static bool hasRun = false;

	if (!hasRun) {
		srand((unsigned int)time(NULL));
		hasRun = true;
	}

	return rand() % (max - min + 1) + min;
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

string getAssetsDir(string subfolder)
{
#if defined( ASSETSDIR )
	std::string base = ASSETSDIR;
#elif defined( __APPLE__ )
	std::string base = SDL_GetBasePath();
#elif defined( __ANDROID__ )
	std::string base = "";
#else
	std::string base = "assets/";
#endif

	return subfolder != "" ? base + subfolder + PATHSEP : base;
}

const char* getSettingsDir()
{
#if defined (PORTABLE)
	return SDL_GetBasePath();
#else
	return SDL_GetPrefPath("shapes", "shapes");
#endif
}

void on_hover(void *_button, void *data)
{
	gSuper->getAudioData()->play("whoosh");
}