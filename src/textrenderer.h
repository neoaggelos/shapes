#ifndef _textrenderer_h
#define _textrenderer_h

#include "main.h"

class TextRenderer {
private:
	map<int, TTF_Font*> fonts;

	void addfont(string filename, int size, int hinting = TTF_HINTING_LIGHT);

public:
	TextRenderer();
	~TextRenderer();

	void write(int fontsize, string text, SDL_Rect output, SDLU_Alignment halign = Left, SDLU_Alignment valign = Top);
	void write(int fontsize, string text, int x, int y);

	void getSize(int fontsize, string text, int *w, int *h);
};

#endif /* _textrenderer_h */