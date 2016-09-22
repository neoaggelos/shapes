#ifndef _util_h
#define _util_h

#include <iostream>
using namespace std;

int random(int min, int max);
void SDL_CHECK(bool check, string msg = "");
string getAssetsDir(string subfolder = "");
const char* getSettingsDir();

template<typename T> string to_string(T var);
string double_to_string(double var);
int to_int(string s);
int to_int(const char *s);

void on_hover(void *_button, void* _unused);

#endif /* _util_h */