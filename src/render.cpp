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

struct Transformation {
	float sc;
	SDL_Rect vp;
};

int fix_mouse_coordinates(void *_t, SDL_Event *event)
{
	Transformation *t = static_cast<Transformation*>(_t);
	static bool badIdea = false;

	/*
	
	regarding badIdea variable:
	
	SDL already sets some event filters for mouse events when changing the
	viewport or the scale of the renderer. However, it seems that these
	filters are not consistent among different systems, thus the code
	does different things for Android and Desktop systems.

	Now, imagine the scenario that you open the game in Android, lock the
	screen and then unlock. In this case, you will notice that the events
	are not properly re-aligned with the viewport. And so, a badIdea was
	born. When we get SDL_WINDOWEVENT_RESTORED, set this var to true,
	which in turn adds the proper extra padding to the mouse coordinates.
	This only seems to be needed the first time only, so keeping it as
	a static variable in this function works.

	If you ever encounter mouse behaving abnormaly in the game, revisit
	this code.

	TL;DR I fucked up, this is where you look for bugs if ever shit happens

	~ your younger self

	*/

	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESTORED) {
		SDL_RenderSetViewport(gSuper->getRenderData()->getRenderer(), NULL);
		SDL_RenderSetViewport(gSuper->getRenderData()->getRenderer(), &(t->vp));
		badIdea = true;
	}



#ifndef __ANDROID__
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		event->button.x -= t->vp.x;
		event->button.y -= t->vp.y;
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		event->button.x -= t->vp.x;
		event->button.y -= t->vp.y;
	}
	else if (event->type == SDL_MOUSEMOTION) {
		event->motion.x -= t->vp.x;
		event->motion.y -= t->vp.y;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		event->button.x /= t->sc;
		event->button.y /= t->sc;
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		event->button.x /= t->sc;
		event->button.y /= t->sc;
	}
	else if (event->type == SDL_MOUSEMOTION) {
		event->motion.x /= t->sc;
		event->motion.y /= t->sc;
	}
#else
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		event->button.x -= 2 * t->vp.x;
		event->button.y -= 2 * t->vp.y;
		if (badIdea) {
			event->button.x -= t->vp.x;
			event->button.y -= t->vp.y;
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		event->button.x -= 2 * t->vp.x;
		event->button.y -= 2 * t->vp.y;
		if (badIdea) {
			event->button.x -= t->vp.x;
			event->button.y -= t->vp.y;
		}
	}
	else if (event->type == SDL_MOUSEMOTION) {
		event->motion.x -= 2 * t->vp.x;
		event->motion.y -= 2 * t->vp.y;
		if (badIdea) {
			event->motion.x -= t->vp.x;
			event->motion.y -= t->vp.y;
		}
	}
#endif

	return 1;
}

RenderData::RenderData(string theme)
{
	SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "2", SDL_HINT_OVERRIDE);

    //TODO with this, we can't create two RenderData objects. Not a bad thing,
    //something that is worth to be noted though

	int r = SDL_Init(SDL_INIT_VIDEO);
	SDL_CHECK(r != -1, "Could not initialize SDL");

	window = SDL_CreateWindow(wintitle, winx, winy, WIDTH, HEIGHT, winflags);
	SDL_CHECK(window != NULL, "Could not create window");

	renderer = SDL_CreateRenderer(window, rnddriver, rndflags);
	SDL_CHECK(renderer != NULL, "Could not create renderer");

	SDL_GetWindowSize(window, &gWidth, &gHeight);
	double ratio = (double)gWidth / (double)gHeight;
	if (gWidth != WIDTH || gHeight != HEIGHT) {
		Transformation * t = new Transformation;

		if (ratio > WIDTH / HEIGHT) {
			t->sc = gHeight / HEIGHT;
			t->vp.x = (gWidth - WIDTH * t->sc) / 2;
			t->vp.y = 0;
			t->vp.w = WIDTH * t->sc;
			t->vp.h = gHeight;
		}
		else if (ratio < WIDTH / HEIGHT) {
			t->sc = gWidth / WIDTH;
			t->vp.x = 0;
			t->vp.y = (gHeight - HEIGHT * t->sc) / 2;
			t->vp.w = gWidth;
			t->vp.h = HEIGHT * t->sc;
		}
		else {
			t->sc = gWidth / WIDTH;
			t->vp.x = t->vp.y = 0;
			t->vp.w = gWidth;
			t->vp.h = gHeight;
		}

		SDL_RenderSetViewport(renderer, &(t->vp));
		SDL_RenderSetScale(renderer, t->sc, t->sc);

//#ifndef __ANDROID__
		SDL_SetEventFilter(fix_mouse_coordinates, t);
//#endif /* ! __ANDROID__ */
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