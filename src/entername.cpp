#include "main.h"

EnterName::EnterName(Super *super, string msg)
{
	parent = super;
	message = msg;
	
	SDL_Window *window = super->getRenderData()->getWindow();
	ok = SDLU_CreateButton(window, "OK", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(ok, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(ok, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonGeometry(ok, 160, 400, 160, 40);
	SDLU_SetButtonHotkey(ok, SDL_SCANCODE_RETURN);
}

EnterName::~EnterName()
{
	SDLU_DestroyButton(ok);
}

string
EnterName::openDialog()
{
	Uint32 count = 0;
	bool done = false;
	SDL_Event event;

	SDL_Renderer* renderer = parent->getRenderData()->getRenderer();
	string input = "";

	SDL_Rect inputRect = SDLU_CreateRect(100, 300, 280, 50);
	SDLU_SetFontSize(18);
	SDL_StartTextInput();
	SDL_SetTextInputRect(&inputRect);
	while (!done) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				parent->finish();
			}
			else if (event.type == SDLU_BUTTON_PRESS) {
				done = static_cast<Uint32>(event.user.code) == ok->id;
			}
			else if (event.type == SDL_TEXTINPUT) {
				input += event.text.text;

				count += 1;
			}
			else if (event.type == SDL_KEYDOWN) {
				SDL_Scancode key = event.key.keysym.scancode;

				if (key == SDL_SCANCODE_BACKSPACE && count > 0) {
					input = input.substr(0, count);
					count--;
				}
				else if (key == SDL_SCANCODE_RETURN) {
					done = true;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDLU_RenderText(renderer, SDLU_ALIGN_CENTER, 80, "%s", message.c_str());

		SDL_RenderDrawRect(renderer, &inputRect);
		SDLU_RenderText(renderer, 110, 310, "%s", input.length() ? input.c_str() : "");

		Uint8 color = 0xff * (SDL_GetTicks() % 1000 < 500);
		SDL_SetRenderDrawColor(renderer, color, color, color, 0xff);

		int width, height;
		if (count > 0) {
			SDLU_GetTextOutputSize(input.c_str(), &width, &height);
		}
		else {
			width = 0;
		}
		SDL_RenderDrawLine(renderer, 111 + width, 310, 111 + width, 335);

		SDLU_RenderButton(ok);

		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}

	return input;
}