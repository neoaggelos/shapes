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

int fix_mouse_coordinates(void *_viewport, SDL_Event *event)
{
	SDL_Rect *viewport = static_cast<SDL_Rect*>(_viewport);
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		event->button.x -= viewport->x;
		event->button.y -= viewport->y;
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		event->button.x -= viewport->x;
		event->button.y -= viewport->y;
	}
	else if (event->type == SDL_MOUSEMOTION) {
		event->motion.x -= viewport->x;
		event->motion.y -= viewport->y;
	}
	else if (event->type == SDL_APP_DIDENTERFOREGROUND) {
		SDL_RenderSetViewport(gSuper->getRenderData()->getRenderer(), viewport);
		SDL_Log("THIS IS IT\n\n\n\n\n");
		return 0;
	}

	return 1;
}

RenderData::RenderData(string theme)
{
    //TODO with this, we can't create two RenderData objects. Not a bad thing,
    //something that is worth to be noted though

	int r = SDL_Init(SDL_INIT_VIDEO);
	SDL_CHECK(r != -1, "Could not initialize SDL");

	window = SDL_CreateWindow(wintitle, winx, winy, WIDTH, HEIGHT, winflags);
	SDL_CHECK(window != NULL, "Could not create window");

	renderer = SDL_CreateRenderer(window, rnddriver, rndflags);
	SDL_CHECK(renderer != NULL, "Could not create renderer");

	SDL_GetWindowSize(window, &gWidth, &gHeight);
	if (gWidth != WIDTH && gHeight != HEIGHT) {
		SDL_Rect* viewport = new SDL_Rect;
		viewport->x = (gWidth - WIDTH) / 2;
		viewport->y = (gHeight - HEIGHT) / 2;
		viewport->w = WIDTH;
		viewport->h = HEIGHT;

		SDL_RenderSetViewport(renderer, viewport);
#ifndef __ANDROID__
		SDL_SetEventFilter(fix_mouse_coordinates, viewport);
#endif /* __ANDROID__ */
	}


	reloadTexture(theme);
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
RenderData::reloadTexture(string theme)
{
	string themesIni = getAssetsDir() + "themes.ini";
	SDLU_IniHandler *h = SDLU_LoadIni(themesIni.c_str());
	int colorkey = 1;
	if (h) {
		const char* prop = SDLU_GetIniProperty(h,theme.c_str(), "colorkey");
		if (prop != NULL) {
			int tmp = StringToInt(prop);
			if (tmp == 0 || tmp == 1)
				colorkey = tmp;
		}
	}
	SDLU_DestroyIni(h);
	string fname = getAssetsDir() + theme + ".bmp";

	SDL_Surface *tmp = SDL_LoadBMP(fname.c_str());
	SDL_CHECK(tmp != NULL, "Could not load BMP file");

	SDL_SetColorKey(tmp, colorkey, SDL_MapRGB(tmp->format, 0xff, 0xff, 0xff));
	texture = SDL_CreateTextureFromSurface(renderer, tmp);
	
	SDL_CHECK(texture != NULL, "Could not load texture");
}