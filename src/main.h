#ifndef _main_h
#define _main_h

/* Make love, not hardcoding */
#define NUM_SHAPES 7
#define WIDTH 480.0
#define HEIGHT 640.0

/* definitely not version */
#define VERSION "1.0"

/* C headers */
#include <cstdlib>
#include <ctime>

/* C++ headers */
#include <iostream>
#include <list>
#include <map>
#include <sstream>
using namespace std;

/* Libraries */
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDLU.h"

/* Forward declare all classes, so they can see each other */
class Shape;
class Game;
class RenderData;
class TextRenderer;
class Super;
class Difficulty;
class Settings;
class Highscores;
class EnterName;
class Tutorial;
class AudioData;

/* Shape headers */
#include "util.h"
#include "difficulty.h"
#include "settings.h"
#include "shape.h"
#include "render.h"
#include "textrenderer.h"
#include "game.h"
#include "audiodata.h"
#include "entername.h"
#include "highscores.h"
#include "tutorial.h"
#include "super.h"

/* Globals */
extern Super *gSuper;
extern int gWidth, gHeight;

#endif /* _main_h */
