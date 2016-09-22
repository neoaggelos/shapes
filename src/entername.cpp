#include "main.h"

static void
ok_callback(void *_ok, void *done)
{
	gSuper->getAudioData()->play("bleep");
	*(static_cast<bool*>(done)) = true;
}

EnterName::EnterName(string msg)
{
	message = msg;
	done = false;

	SDL_Window *window = gSuper->getRenderData()->getWindow();
	ok = SDLU_CreateButton(window, "OK", SDLU_BUTTON_TEXT);
	SDLU_SetButtonAction(ok, SDLU_PRESS_ACTION, SDLU_PRESS_INVERT);
	SDLU_SetButtonAction(ok, SDLU_HOVER_ACTION, SDLU_HOVER_BG);
	SDLU_SetButtonCallback(ok, SDLU_PRESS_CALLBACK, ok_callback, &done);
	SDLU_SetButtonCallback(ok, SDLU_HOVER_CALLBACK, on_hover, NULL);
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
	SDL_Event event;
	SDL_Renderer* renderer = gSuper->getRenderData()->getRenderer();
	string input = gSuper->getSettings()->lastName;

	SDL_Rect inputRect = SDLU_CreateRect(100, 230, 280, 50);
	SDL_StartTextInput();
	SDL_SetTextInputRect(&inputRect);
	done = false;
	while (!done) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gSuper->finish();
			}
			else if (event.type == SDL_TEXTINPUT) {
				input += event.text.text;
			}
			else if (event.type == SDL_KEYDOWN) {
				SDL_Scancode key = event.key.keysym.scancode;

				if (key == SDL_SCANCODE_BACKSPACE && input.length()) {
					if (input.length() == 1)
						input = "";
					else
						input = input.substr(0, input.length() - 1);
				}
				else if (key == SDL_SCANCODE_RETURN) {
					done = true;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		gSuper->getTextRenderer()->write(18, message, { 0, 80, 480, 0 }, Center);

		SDL_RenderDrawRect(renderer, &inputRect);
		if (input != "")
			gSuper->getTextRenderer()->write(18, input, { inputRect.x + 10, inputRect.y, inputRect.w, inputRect.h }, Left, Center);

		Uint8 color = 0xff * (SDL_GetTicks() % 1000 < 500);
		SDL_SetRenderDrawColor(renderer, color, color, color, 0xff);

		int width, height;
		gSuper->getTextRenderer()->getSize(18, input, &width, &height);
		SDL_RenderDrawLine(renderer, 111 + width, 240, 111 + width, 265);

		SDLU_RenderButton(ok);

		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}
	SDL_StopTextInput();

	gSuper->getSettings()->lastName = input;
	return input;
}