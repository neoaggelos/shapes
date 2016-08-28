#ifndef _main_h
#define _main_h

#define NUM_SHAPES 7

#define WIDTH 480
#define HEIGHT 640

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

/* Shape headers */
#include "random.h"
#include "difficulty.h"
#include "settings.h"
#include "shape.h"
#include "render.h"
#include "game.h"
#include "super.h"

#endif /* _main_h */