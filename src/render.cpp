#include "render.h"

static const char* wintitle = "Shapes v" VERSION;
static const int winx = SDL_WINDOWPOS_CENTERED;
static const int winy = SDL_WINDOWPOS_CENTERED;
static const Uint32 winflags = SDL_WINDOW_SHOWN;

static const int rnddriver = -1;
static const int rndflags = SDL_RENDERER_PRESENTVSYNC;

void RenderData::SDL_CHECK(bool check, string msg)
{
	if (!check) {
		string err = "SDL Error: " + msg + ": " + SDL_GetError();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err.c_str(), NULL);
		if (SDL_WasInit(SDL_INIT_VIDEO) != 0)
			SDL_Quit();

		exit(0);
	}
}

RenderData::RenderData(Super *s)
{
    //TODO with this, we can't create two RenderData objects. Not a bad thing,
    //something that is worth to be noted though

	int r = SDL_Init(SDL_INIT_VIDEO);
	SDL_CHECK(r != -1, "Could not initialize SDL");

	window = SDL_CreateWindow(wintitle, winx, winy, WIDTH, HEIGHT, winflags);
	SDL_CHECK(window != NULL, "Could not create window");

	renderer = SDL_CreateRenderer(window, rnddriver, rndflags);
	SDL_CHECK(renderer != NULL, "Could not create renderer");

  SDL_RenderSetLogicalSize(renderer, 480, 640);

  reloadTexture(s);
}

RenderData::~RenderData()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	
    SDL_Quit();
}

SDL_Window*
RenderData::getWindow()
{
	return window;
}

SDL_Renderer*
RenderData::getRenderer()
{
	return renderer;
}

SDL_Texture*
RenderData::getTexture()
{
	return texture;
}

void
RenderData::reloadTexture(Super *s)
{
	string themesIni = getAssetsDir() + "themes.ini";
	SDLU_IniHandler *h = SDLU_LoadIni(themesIni.c_str());
	int colorkey = 1;
	if (h) {
		const char* prop = SDLU_GetIniProperty(h, s->getSettings()->theme.c_str(), "colorkey");
		if (prop != NULL) {
			int tmp = StringToInt(prop);
			if (tmp == 0 || tmp == 1)
				colorkey = tmp;
		}
	}
	SDLU_DestroyIni(h);
	string fname = getAssetsDir() + s->getSettings()->theme + ".bmp";

	SDL_Surface *tmp = SDL_LoadBMP(fname.c_str());
	SDL_CHECK(tmp != NULL, "Could not load BMP file");

	SDL_SetColorKey(tmp, colorkey, SDL_MapRGB(tmp->format, 0xff, 0xff, 0xff));
	texture = SDL_CreateTextureFromSurface(renderer, tmp);
	
	SDL_CHECK(texture != NULL, "Could not load texture");
}