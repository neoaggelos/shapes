#ifndef _util_h
#define _util_h

#include <iostream>
using namespace std;

int random(int min, int max);
char *IntToString(int i);
int StringToInt(const char *s);
string getAssetsDir(string subfolder = "");
const char* getSettingsDir();

void on_hover(void *_button, void* _unused);

#endif /* _util_h */