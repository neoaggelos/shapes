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
using std::list;
using std::string;
using std::map;

/* Libraries */
#include "SDL.h"
#include "SDLU.h"
#include "SDL_mixer.h"

/* Forward declare all classes, so they can see each other */
class Shape;
class Game;
class RenderData;
class Super;
class Difficulty;
class Settings;
class Highscores;
class EnterName;
class AudioData;

/* Shape headers */
#include "util.h"
#include "difficulty.h"
#include "settings.h"
#include "shape.h"
#include "render.h"
#include "game.h"
#include "audiodata.h"
#include "entername.h"
#include "highscores.h"
#include "super.h"

/* Globals */
extern Super *gSuper;
extern int gWidth, gHeight;

#endif /* _main_h */