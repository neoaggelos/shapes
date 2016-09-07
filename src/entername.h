#ifndef _entername_h
#define _entername_h

#include "main.h"

class EnterName {
private:

	string message;
	SDLU_Button *ok;
	bool done;

public:
	EnterName(string msg);
	~EnterName();

	string openDialog();
};

#endif /* _entername_h */