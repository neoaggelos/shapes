#include "main.h"

void
TextRenderer::addfont(string filename, int size, int hinting)
{
	SDLU_Font* font = SDLU_LoadFont(filename.c_str(), size, hinting);
	SDL_CHECK(font != NULL, "Could not load font " + filename);

	fonts[size] = font;
}

TextRenderer::TextRenderer()
{
	int r = SDLU_StartFonts();
	SDL_CHECK(r != -1, "Could not initialize SDL2_ttf");

	/* open fonts */
	string filename = getAssetsDir() + "font.ttf";
	addfont(filename, 14);
	addfont(filename, 15);
	addfont(filename, 18);
	addfont(filename, 20);
	addfont(filename, 42);
}

TextRenderer::~TextRenderer()
{
	for (map<int, SDLU_Font*>::iterator it = fonts.begin(); it != fonts.end(); it++) {
		SDLU_CloseFont(it->second);
	}

	SDLU_FinishFonts();
}

void
TextRenderer::getSize(int fontsize, string text, int * w, int * h)
{
	SDLU_GetUTF8Size(fonts[fontsize], text.c_str(), w, h);
}

SDLU_Font*
TextRenderer::getFont(int size)
{
	return fonts[size];
}

void
TextRenderer::write(int size, string text, int x, int y)
{
	return write(size, text, { x, y, 0, 0});
}

void
TextRenderer::write(int size, string text, SDL_Rect pos, SDLU_Alignment halign, SDLU_Alignment valign)
{
	SDLU_RenderUTF8Ex(
		gSuper->getRenderData()->getRenderer(),
		fonts[size], text.c_str(),
		pos, halign, valign
	);
}