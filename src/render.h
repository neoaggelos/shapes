#ifndef _render_h
#define _render_h

#include "main.h"

class RenderData {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture* textures[NUM_SHAPES];

	string assetsDir;

	SDL_Texture *load_texture(const char *file);

	void SDL_CHECK(bool check, string msg = "SDL Error");
public:
	RenderData();

	~RenderData();

	SDL_Window *getWindow();
	SDL_Renderer *getRenderer();

	SDL_Texture *getTexture(int type);
};

#endif /* _render_h */