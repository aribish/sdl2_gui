#include <stdio.h>
#include "sdl2_gui.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

SDL_Window *win;
SDL_Renderer *render;
GUI_Context *gui;
SDL_Event event;

GUI_FontID font;
GUI_Text *header;
GUI_Button *button;
GUI_Slider *slider;

void onPress(void *component) {
	printf("on press\n");
}
void onRelease(void *component) {
	printf("on release\n");
}
void onEnter(void *component) {
	printf("on enter\n");
}
void onExit(void *component) {
	printf("on exit\n");
}

void sliderOnHold(void *component) {
	printf("slider held!!!!\n");
}

void sliderOnRelease(void *component) {
	printf("slider released!!!!\n");
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("Hello, GUI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
	render = SDL_CreateRenderer(win, -1, 0);
	gui = GUI_Init(win, render);

	font = GUI_NewFont(gui, "SourceCodePro-Regular.ttf", 24);
	header = GUI_NewText(gui, font, "Crazy example", COLOR_CYAN);
	
	button = GUI_NewButton(gui, 80, 80, 0, COLOR_RED, COLOR_BLACK, NULL, NULL);
	GUI_AddEvent(gui, button, &onPress, GUI_BUTTON_ON_PRESS);
	GUI_AddEvent(gui, button, &onRelease, GUI_BUTTON_ON_RELEASE);
	GUI_AddEvent(gui, button, &onEnter, GUI_BUTTON_ON_ENTER);
	GUI_AddEvent(gui, button, &onExit, GUI_BUTTON_ON_EXIT);

	slider = GUI_NewSlider(gui, NULL, NULL, 0.0, 6, 120, false, COLOR_LIGHT_GREY, COLOR_MAGENTA);
	GUI_AddEvent(gui, slider, &sliderOnHold, GUI_SLIDER_ON_HOLD);
	GUI_AddEvent(gui, slider, &sliderOnRelease, GUI_SLIDER_ON_RELEASE);
	
	while(true) {
		while(SDL_PollEvent(&event)) {
			if(GUI_Update(gui, event))
				continue;

			if(event.type == SDL_QUIT) {
				goto quit;
			}
		}

		SDL_SetRenderDrawColor(render, 0, 0, 0, 0xff);
		SDL_RenderClear(render);

		GUI_DrawText(gui, header, (WIN_WIDTH - header->width) / 2, 40);
		if(button != NULL) {GUI_DrawButton(gui, button, 0, 0); }
		GUI_DrawSlider(gui, slider, (WIN_WIDTH - slider->length) / 2, 150);

		SDL_RenderPresent(render);
	}

	quit:
	GUI_Quit(gui);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
