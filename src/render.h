#ifndef _render_h
#define _render_h

#include "main.h"

class RenderData {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture* texture;

public:
    RenderData(string theme);

    ~RenderData();

    SDL_Window *getWindow();
    SDL_Renderer *getRenderer();

    SDL_Texture *getTexture();
    void reloadTexture(string theme);
};

#endif /* _render_h */
