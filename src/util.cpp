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

void SDL_CHECK(bool check, string msg)
{
	if (!check) {
		string err = "SDL Error: " + msg + ": " + SDL_GetError();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err.c_str(), NULL);
		if (SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
			SDL_Quit();

		exit(0);
	}
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


string int_to_string(int var)
{
	stringstream s;
	s << var;
	return s.str();
}

string double_to_string(double var)
{
	char buffer[20];
	snprintf(buffer, 19, "%.1lf", var);
	return string(buffer);
}

int to_int(string str)
{
	return to_int(str.c_str());
}

int to_int(const char* str)
{
	int j = 0;
	for (unsigned int i = 0; i < SDL_strlen(str); i++) {
		j = 10 * j + str[i] - '0';
	}

	return j;
}

SDLU_Button*
CreateButton(const char* name, const char* title, SDL_Rect pos, int fontsize, SDLU_Callback press, void *press_arg, SDL_Scancode hotkey, SDLU_Callback hover, void* hover_arg)
{
	SDLU_Button * ret = SDLU_CreateButton(gSuper->getRenderData()->getWindow(), title, SDLU_BUTTON_TEXT);

	SDLU_CloseFont(((SDLU_Styles*)ret->content)->font);
	((SDLU_Styles*)ret->content)->font = gSuper->getTextRenderer()->getFont(fontsize);
	((SDLU_Styles*)ret->content)->freefont = SDL_FALSE;

	SDLU_SetButtonAction(ret, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
#ifndef __ANDROID__
	SDLU_SetButtonAction(ret, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
#endif /* __ANDROID__ */

	SDLU_SetButtonGeometry(ret, pos.x, pos.y, pos.w, pos.h);
	SDLU_SetButtonHotkey(ret, hotkey);

	if (press) SDLU_SetButtonCallback(ret, SDLU_PRESS_CALLBACK, press, press_arg);
	if (hover) SDLU_SetButtonCallback(ret, SDLU_HOVER_CALLBACK, hover, hover_arg);

	ret->name = name;

	return ret;
}

void
DestroyButton(SDLU_Button* th)
{
	SDLU_DestroyButton(th);
}

SDLU_ComboBox*
CreateComboBox(string items[], int n, string active, int x, int y, int w, int h, int fontsize)
{
	SDLU_ComboBox* ret = SDLU_CreateComboBox(gSuper->getRenderData()->getWindow());

	SDLU_SetComboBoxGeometry(ret, x, y, w, h);

	SDLU_CloseFont(ret->styles->font);
	ret->styles->font = gSuper->getTextRenderer()->getFont(fontsize);
	ret->styles->freefont = SDL_FALSE;

	for (int i = 0; i < n; i++) {
		SDLU_AddComboBoxItem(&ret, items[i].c_str());
	}

	SDLU_SetComboBoxActiveItem(ret, active.c_str());

	return ret;
}

SDLU_ComboBox*
CreateComboBox(string items[], int n, int active, int x, int y, int w, int h, int fontsize)
{
	SDLU_ComboBox* ret = SDLU_CreateComboBox(gSuper->getRenderData()->getWindow());

	SDLU_SetComboBoxGeometry(ret, x, y, w, h);

	SDLU_CloseFont(ret->styles->font);
	ret->styles->font = gSuper->getTextRenderer()->getFont(fontsize);
	ret->styles->freefont = SDL_FALSE;

	for (int i = 0; i < n; i++) {
		SDLU_AddComboBoxItem(&ret, items[i].c_str());
	}

	SDLU_SetComboBoxActiveIndex(ret, active);

	return ret;
}

