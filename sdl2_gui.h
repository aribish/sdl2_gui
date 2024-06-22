#ifndef AB_SDL2_GUI_H
#define AB_SDL2_GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define COLOR_RED (SDL_Color) {0xff, 0, 0, 0xff}
#define COLOR_GREEN (SDL_Color) {0, 0xff, 0, 0xff}
#define COLOR_BLUE (SDL_Color) {0, 0, 0xff, 0xff}

#define COLOR_CYAN (SDL_Color) {0, 0xff, 0xff, 0xff}
#define COLOR_MAGENTA (SDL_Color) {0xff, 0, 0xff, 0xff}
#define COLOR_YELLOW (SDL_Color) {0xff, 0xff, 0, 0xff}

#define COLOR_BLACK (SDL_Color) {0, 0, 0, 0xff}
#define COLOR_DARK_GREY (SDL_Color) {0x44, 0x44, 0x44, 0xff}
#define COLOR_LIGHT_GREY (SDL_Color) {0xcc, 0xcc, 0xcc, 0xff}
#define COLOR_WHITE (SDL_Color) {0xff, 0xff, 0xff, 0xff}

typedef int GUI_FontID;

typedef enum {
	GUI_COMPONENT_TEXT,
	GUI_COMPONENT_IMAGE,
	GUI_COMPONENT_BUTTON,
	GUI_COMPONENT_SLIDER
} GUI_ComponentType;

struct _GUI_ComponentNode {
	void* component;
	GUI_ComponentType type;

	struct _GUI_ComponentNode *prev;
	struct _GUI_ComponentNode *next;
};
typedef struct _GUI_ComponentNode GUI_ComponentNode;

typedef void (*GUI_Event)(void* component);
typedef enum {
	GUI_BUTTON_ON_PRESS,
	GUI_BUTTON_ON_RELEASE,
	GUI_BUTTON_ON_ENTER,
	GUI_BUTTON_ON_EXIT,
	GUI_SLIDER_INTERNAL,
} GUI_EventType;

struct _GUI_EventNode {
	GUI_Event event;
	GUI_EventType type;
	void *component;

	struct _GUI_EventNode *prev;
	struct _GUI_EventNode *next;
};
typedef struct _GUI_EventNode GUI_EventNode;

typedef struct {
	SDL_Window *window;
	SDL_Renderer *render;
	int width, height;
	GUI_ComponentNode *components;
	TTF_Font **fonts;
	int numFonts;
	GUI_EventNode *events;
} GUI_Context;

typedef struct {
	SDL_Texture* texture;
	int width, height;
} GUI_Text;

typedef struct {
	SDL_Texture* texture;
	int width, height;
} GUI_Image;

typedef struct {
	GUI_Text *textContent;
	GUI_Image *imageContent;
	int x, y, width, height;
	int borderWidth;
	SDL_Color fillColor, borderColor;
	bool inside;
	bool pressed;
	bool onScreen;
} GUI_Button;

typedef struct {
	GUI_Button *inc, *dec;
	int x, y, width, length;
	bool vertical;
	SDL_Color sliderColor, handleColor;
	float value;
	float buttonValueMod;
	bool holding;
	bool onScreen;
} GUI_Slider;

GUI_Context* GUI_Init(SDL_Window *win, SDL_Renderer *render);
void GUI_Quit(GUI_Context *context);

void GUI_SerializeComponent(GUI_Context *context, void *component, GUI_ComponentType type);
void GUI_FreeComponent(GUI_Context *context, void *component);
GUI_FontID GUI_NewFont(GUI_Context *context, const char *path, int ptsize);

GUI_Text* GUI_NewTextBg(GUI_Context *context, GUI_FontID font, const char *str, SDL_Color fg, SDL_Color bg);
GUI_Text* GUI_NewText(GUI_Context *context, GUI_FontID font, const char *str, SDL_Color fg);
void GUI_FreeText(GUI_Context *context, GUI_Text *text);
void GUI_DrawText(GUI_Context *context, GUI_Text *text, int x, int y);

GUI_Image* GUI_NewImage(GUI_Context *context, const char *path, float xScale, float yScale); 
void GUI_FreeImage(GUI_Context *context, GUI_Image *image);
void GUI_DrawImage(GUI_Context *context, GUI_Image *image, int x, int y);

GUI_Button* GUI_NewButton(GUI_Context *context, int width, int height, int borderWidth, SDL_Color fillColor, SDL_Color borderColor, GUI_Text *textContent, GUI_Image *imageContent);
void GUI_FreeButton(GUI_Context *context, GUI_Button *button);
void GUI_DrawButton(GUI_Context *context, GUI_Button *button, int x, int y);

GUI_Slider* GUI_NewSlider(GUI_Context *context, GUI_Button *incrementButton, GUI_Button *decrementButton, float buttonValueMod, int width, int length, bool vertical, SDL_Color sliderColor, SDL_Color handleColor);
void GUI_FreeSlider(GUI_Context *context, GUI_Slider *slider);
void GUI_DrawSlider(GUI_Context *context, GUI_Slider *slider, int x, int y);

bool GUI_IsPointInsideRect(int mx, int my, int x, int y, int w, int h);
void GUI_SetEvent(GUI_Context *context, void* component, GUI_Event event, GUI_EventType type);
void GUI_RemoveEvent(GUI_Context *context, void* component, GUI_EventType type);
void GUI_UpdateEvents(GUI_Context *context, SDL_Event event);

#endif
