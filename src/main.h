#ifndef _main_h
#define _main_h

#define NUM_SHAPES 7

#define WIDTH 480.0
#define HEIGHT 640.0

/* definitely not version */
#define VERSION "0.1"

/* C headers */
#include <cstdlib>
#include <ctime>

/* C++ headers */
#include <iostream>
#include <list>
using std::list;
using std::string;

/* Libraries */
#include "SDL.h"
#include "SDLU.h"

class Shape;
class Game;
class RenderData;
class Super;
class Difficulty;
class Settings;
class Highscores;
class EnterName;

/* Shape headers */
#include "util.h"
#include "difficulty.h"
#include "settings.h"
#include "shape.h"
#include "render.h"
#include "game.h"
#include "entername.h"
#include "highscores.h"
#include "super.h"

extern Super *gSuper;
extern int gWidth, gHeight;

#endif /* _main_h */