#ifndef _settingsmenu_h
#define _settingsmenu_h

#include "main.h"

class SettingsMenu {
private:
	Super *parent;

	SDLU_Button *shapeButton, *rightButton, *leftButton;
	SDLU_Button *resetButton, *backButton;
	SDLU_ComboBox *diffBox;

	bool mustClose;
public:
	SettingsMenu(Super *super);
	~SettingsMenu();

	void handleEvent(SDL_Event event);
	void render();

	void run();
	bool isDone();
};

#endif /* _settingsmenu_h */