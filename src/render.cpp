#include "render.h"

static const char* wintitle = "Shapes v" VERSION;
static const int winx = SDL_WINDOWPOS_CENTERED;
static const int winy = SDL_WINDOWPOS_CENTERED;
static const Uint32 winflags = SDL_WINDOW_SHOWN;

static const int rnddriver = -1;
static const int rndflags = SDL_RENDERER_PRESENTVSYNC;

SDL_Texture *RenderData::load_texture(const char* file)
{
	SDL_Surface *tmp = SDL_LoadBMP(file);
    SDL_CHECK(tmp != NULL, "Could not load BMP file");

    SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 0xff, 0xff, 0xff));
	SDL_Texture *ret = SDL_CreateTextureFromSurface(renderer, tmp);
	SDL_FreeSurface(tmp);

	return ret;
}

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

RenderData::RenderData()
{
    //TODO with this, we can't create two RenderData objects. Not a bad thing,
    //something that is worth to be noted though

#ifdef ASSETSDIR
	assetsDir = ASSETSDIR;
#else
	assetsDir = "C:/assets";
#endif


	int r = SDL_Init(SDL_INIT_VIDEO);
	SDL_CHECK(r != -1, "Could not initialize SDL");

	window = SDL_CreateWindow(wintitle, winx, winy, WIDTH, HEIGHT, winflags);
	SDL_CHECK(window != NULL, "Could not create window");

	renderer = SDL_CreateRenderer(window, rnddriver, rndflags);
	SDL_CHECK(renderer != NULL, "Could not create renderer");

	for (int i = 0; i < NUM_SHAPES; i++) {
		// TODO this must be changed if NUM_SHAPES >= 10
		
		string fname = assetsDir + "/" + static_cast<char>(i + '1') + ".bmp";
		textures[i] = load_texture(fname.c_str());

		SDL_CHECK(textures[i] != NULL, "Could not load texture");
	}
}

RenderData::~RenderData()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	for (int i = 0; i < NUM_SHAPES; i++)
		SDL_DestroyTexture(textures[i]);
	
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
RenderData::getTexture(int type)
{
	return textures[type];
}