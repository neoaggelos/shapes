#ifndef _entername_h
#define _entername_h

#include "main.h"

class EnterName {
private:

	Super *parent;
	string message;
	SDLU_Button *ok;

public:
	EnterName(Super *super, string msg);
	~EnterName();

	string openDialog();
};

#endif /* _entername_h */