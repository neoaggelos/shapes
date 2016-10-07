#ifndef _tutorial_h
#define _tutorial_h

#include "main.h"

class Tutorial {
private:
    Shape* player;
    Shape* falling;

public:
    Tutorial();
    ~Tutorial();

    void run();
};

#endif /* _tutorial_h */
