#include "main.h"

inline int
getTextWidth(TTF_Font* font, string text)
{
	int w, h;
	TTF_SizeUTF8(font, text.c_str(), &w, &h);

	return w;
}

inline int
getTextHeight(TTF_Font* font, string text)
{
	int w, h;
	TTF_SizeUTF8(font, text.c_str(), &w, &h);

	return h;
}

void
TextRenderer::addfont(string filename, int size, int hinting)
{
	TTF_Font *newfont = TTF_OpenFont(filename.c_str(), size);
	SDL_CHECK(newfont != NULL, "Could not load font " + filename);

	TTF_SetFontHinting(newfont, hinting);

	fonts[size] = newfont;
}

TextRenderer::TextRenderer()
{
	int r = TTF_Init();
	SDL_CHECK(r != -1, "Could not initialize SDL2_ttf");

	/* open font sizes */
	string filename = getAssetsDir() + "font.ttf";
	addfont(filename, 14);
	addfont(filename, 18);
	addfont(filename, 20);
	addfont(filename, 42);
}

TextRenderer::~TextRenderer()
{
	for (map<int, TTF_Font*>::iterator it = fonts.begin(); it != fonts.end(); it++) {
		TTF_CloseFont(it->second);
	}

	TTF_Quit();
}

void
TextRenderer::write(int size, string text, int x, int y)
{
	return write(size, text, { x, y, 0, 0});
}

void
TextRenderer::getSize(int fontsize, string text, int * w, int * h)
{
	TTF_SizeUTF8(fonts[fontsize], text.c_str(), w, h);
}

void
TextRenderer::write(int size, string text, SDL_Rect pos, SDLU_Alignment halign, SDLU_Alignment valign)
{
	SDL_Renderer* renderer = gSuper->getRenderData()->getRenderer();
	
	SDL_Color fg;
	SDL_GetRenderDrawColor(renderer, &fg.r, &fg.g, &fg.b, &fg.a);
	SDL_Surface *surface = TTF_RenderUTF8_Blended(fonts[size], text.c_str(), fg);
	SDL_CHECK(surface != NULL, "SDL2_ttf failed to render text");

	/* output x, y */
	int x, y;
	switch (halign) {
	case Left: x = pos.x;								break;
	case Center: x = pos.x + (pos.w - surface->w) / 2;	break;
	case Right: x = pos.x + pos.w - surface->w;			break;
	}
	switch (valign) {
	case Top: y = pos.y;								break;
	case Center: y = pos.y + (pos.h - surface->h) / 2;	break;
	case Bottom: y = pos.y + pos.h - surface->h;		break;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_CHECK(texture != NULL, "Could not create texture");

	SDLU_CopyTexture(renderer, texture, x, y);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}