#ifndef _render_h
#define _render_h

#include "main.h"

class RenderData {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture* texture;

	void SDL_CHECK(bool check, string msg = "SDL Error");
public:
	RenderData(Super *s);

	~RenderData();

	SDL_Window *getWindow();
	SDL_Renderer *getRenderer();

	SDL_Texture *getTexture();

	void reloadTexture(Super *s);
};

#endif /* _render_h */