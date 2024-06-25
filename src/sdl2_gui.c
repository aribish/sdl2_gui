#include "sdl2_gui.h"

#include <stdlib.h>
#include <SDL2/SDL_image.h>

const char *BMP_FILE_ENDING = ".bmp";

GUI_Context* GUI_Init(SDL_Window *win, SDL_Renderer *render) {
	if(!SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
		return NULL;

	if(TTF_Init() < 0) {
		printf("%s\n", SDL_GetError());
		return NULL;
	}

	if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP | IMG_INIT_TIF) < 0) {
		printf("%s\n", SDL_GetError());
		return NULL;
	}

	GUI_Context *context = (GUI_Context*)malloc(sizeof(GUI_Context));

	context->window = win;
	context->render = render;
	SDL_GetWindowSize(win, &context->width, &context->height);
	context->components = NULL;
	context->fonts = NULL;
	context->numFonts = 0;
	
	return context;
}
void GUI_Quit(GUI_Context *context) {
	while(context->components != NULL) {
		GUI_FreeComponent(context, context->components->component);
	}

	for(int i = 0; i < context->numFonts; i++) {
		TTF_CloseFont(context->fonts[i]);
	}
	if(context->fonts != NULL)
		free(context->fonts);

	TTF_Quit();
	IMG_Quit();

	free(context);
}

void GUI_SerializeComponent(GUI_Context *context, void *component, GUI_ComponentType type) {
	GUI_ComponentNode **node = &context->components;
	GUI_ComponentNode *prev = NULL;
	
	while(*node != NULL) {
		prev = *node;
		node = &(*node)->next;
	}

	*node = (GUI_ComponentNode*) malloc(sizeof(GUI_ComponentNode));
	(*node)->component = component;
	(*node)->type = type;
	(*node)->events = NULL;
	(*node)->prev = prev;
	(*node)->next = NULL;
}
void GUI_FreeComponent(GUI_Context *context, void *component) {
	GUI_ComponentNode *node = context->components;

	while(node != NULL && node->component != component) {
		node = node->next;
	}

	if(node == NULL)
		return;

	GUI_ClearEvents(context, component);

	if(node->prev == NULL) {
		context->components = node->next;
		if(context->components != NULL) {
			context->components->prev = NULL;
		}
	}
	else {
		node->prev->next = node->next;
	}


	if(node->type == GUI_COMPONENT_TEXT)
		GUI_FreeText(context, (GUI_Text*) component);
	else if(node->type == GUI_COMPONENT_IMAGE)
		GUI_FreeImage(context, (GUI_Image*) component);
	else if(node->type == GUI_COMPONENT_BUTTON)
		GUI_FreeButton(context, (GUI_Button*) component);

	free(node);
}
GUI_FontID GUI_NewFont(GUI_Context *context, const char *path, int ptsize) {
	TTF_Font* font = TTF_OpenFont(path, ptsize);
	if(font == NULL)
		return -1;

	TTF_Font **newFonts = (TTF_Font**) calloc(context->numFonts + 1, sizeof(TTF_Font*));
	
	for(int i = 0; i < context->numFonts; i++) {
		newFonts[i] = context->fonts[i];
	}

	if(context->fonts != NULL)
		free(context->fonts);

	context->fonts = newFonts;
	context->fonts[context->numFonts] = font;
	context->numFonts++;

	return context->numFonts - 1;
}

GUI_Text* GUI_NewTextBg(GUI_Context *context, GUI_FontID font, const char *str, SDL_Color fg, SDL_Color bg) {
	GUI_Text *text = (GUI_Text*) malloc(sizeof(GUI_Text));

	SDL_Surface *surface = TTF_RenderText_Shaded_Wrapped(context->fonts[font], str, fg, bg, 0);
	if(surface == NULL)
		return NULL;

	text->texture = SDL_CreateTextureFromSurface(context->render, surface);
	SDL_FreeSurface(surface);

	if(text->texture == NULL)
		return NULL;

	TTF_SizeText(context->fonts[font], str, &text->width, &text->height);

	GUI_SerializeComponent(context, text, GUI_COMPONENT_TEXT);
	return text;
}
GUI_Text* GUI_NewText(GUI_Context *context, GUI_FontID font, const char *str, SDL_Color fg) {
	GUI_Text *text = (GUI_Text*) malloc(sizeof(GUI_Text));

	SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(context->fonts[font], str, fg, 0);
	if(surface == NULL)
		return NULL;

	text->texture = SDL_CreateTextureFromSurface(context->render, surface);
	SDL_FreeSurface(surface);

	if(text->texture == NULL)
		return NULL;

	TTF_SizeText(context->fonts[font], str, &text->width, &text->height);

	GUI_SerializeComponent(context, text, GUI_COMPONENT_TEXT);
	return text;
}
void GUI_FreeText(GUI_Context *context, GUI_Text *text) {
	SDL_DestroyTexture(text->texture);
	free(text);
}
void GUI_DrawText(GUI_Context *context, GUI_Text *text, int x, int y) {
	SDL_Rect rect = (SDL_Rect) {x, y, text->width, text->height};
	SDL_RenderCopy(context->render, text->texture, NULL, &rect);
}

GUI_Image* GUI_NewImage(GUI_Context *context, const char *path, float xScale, float yScale) {
	GUI_Image* image = (GUI_Image*) malloc(sizeof(GUI_Image));
	bool isBmp = true;
	int len = 0;

	while(path[len] != 0) {
		len++;
	}

	for(int i = 0; i < 4; i++) {
		if(path[len - 4 + i] != BMP_FILE_ENDING[i]) {
			isBmp = false;
			break;
		}
	}

	SDL_Surface *surface;
	if(isBmp)
		surface = SDL_LoadBMP(path);
	else
		surface = IMG_Load(path);

	if(surface == NULL)
		return NULL;

	SDL_Texture *texture = SDL_CreateTextureFromSurface(context->render, surface);
	SDL_FreeSurface(surface);
	if(texture == NULL)
		return NULL;

	image->texture = texture;

	SDL_QueryTexture(texture, NULL, NULL, &image->width, &image->height);
	image->width *= xScale;
	image->height *= yScale;

	GUI_SerializeComponent(context, image, GUI_COMPONENT_IMAGE);
	return image;
}
void GUI_FreeImage(GUI_Context *context, GUI_Image *image) {
	SDL_DestroyTexture(image->texture);
	free(image);
}
void GUI_DrawImage(GUI_Context *context, GUI_Image *image, int x, int y) {
	SDL_Rect rect = (SDL_Rect) {x, y, image->width, image->height};
	SDL_RenderCopy(context->render, image->texture, NULL, &rect);
}

GUI_Button* GUI_NewButton(GUI_Context *context, int width, int height, int borderWidth, SDL_Color fillColor, SDL_Color borderColor, GUI_Text *textContent, GUI_Image *imageContent) {
	GUI_Button *button = (GUI_Button*) malloc(sizeof(GUI_Button));

	button->textContent = textContent;
	button->imageContent = imageContent;
	button->x = -1;
	button->y = -1;
	button->width = width;
	button->height = height;
	button->borderWidth = borderWidth;
	button->fillColor = fillColor;
	button->borderColor = borderColor;
	button->inside = false;
	button->pressed = false;
	button->onScreen = false;

	GUI_SerializeComponent(context, button, GUI_COMPONENT_BUTTON);
	return button;
}
void GUI_FreeButton(GUI_Context *context, GUI_Button *button) {
	free(button);
}
void GUI_DrawButton(GUI_Context *context, GUI_Button *button, int x, int y) {
	button->x = x;
	button->y = y;
	button->onScreen = true;

	SDL_Rect rect = (SDL_Rect) {x, y, button->width, button->height};

	SDL_SetRenderDrawColor(context->render, button->fillColor.r, button->fillColor.g, button->fillColor.b, button->fillColor.a);
	SDL_RenderFillRect(context->render, &rect);

	SDL_SetRenderDrawColor(context->render, button->borderColor.r, button->borderColor.g, button->borderColor.b, button->borderColor.a);
	for(int i = 0; i < button->borderWidth; i++) {
		SDL_RenderDrawRect(context->render, &rect);
		rect.x++;
		rect.y++;
		rect.w -= 2;
		rect.h -= 2;
	}

	int yPadding = 0;
	int numContent = 1;

	if(button->textContent != NULL) {
		yPadding += button->textContent->height;
		numContent++;
	}
	if(button->imageContent != NULL) {
		yPadding += button->imageContent->height;
		numContent++;
	}

	yPadding = (button->height - yPadding) / numContent;

	if(button->imageContent != NULL) {
		GUI_DrawImage(context, button->imageContent, x + (button->width - button->imageContent->width) / 2, y + yPadding);
		y += yPadding + button->imageContent->height;
	}
	if(button->textContent != NULL) {
		GUI_DrawText(context, button->textContent, x + (button->width - button->textContent->width) / 2, y + yPadding);
	}
}

void GUI_InternalSliderEvent(void *component) {
	GUI_Slider *slider = (GUI_Slider*)component;
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	
	if(slider->vertical) {
		slider->value = 1 - ((float)(mouseY - slider->y)) / slider->length;
	}
	else {
		slider->value = ((float)(mouseX - slider->x)) / slider->length;
	}

	if(slider->value > 1.0)
		slider->value = 1.0;
	else if(slider->value < 0.0)
		slider->value = 0.0;
}

GUI_Slider* GUI_NewSlider(GUI_Context *context, GUI_Button *incrementButton, GUI_Button *decrementButton, float buttonValueMod, int width, int length, bool vertical, SDL_Color sliderColor, SDL_Color handleColor) {
	GUI_Slider *slider = (GUI_Slider*) malloc(sizeof(GUI_Slider));

	if(incrementButton != NULL && decrementButton != NULL) {
		slider->inc = incrementButton;
		slider->dec = decrementButton;
	}
	else {
		slider->inc = NULL;
		slider->dec = NULL;
	}
	slider->x = -1;
	slider->y = -1;
	slider->width = width;
	slider->length = length;
	slider->vertical = vertical;
	slider->sliderColor = sliderColor;
	slider->handleColor = handleColor;
	slider->value = 0.5;
	slider->buttonValueMod = buttonValueMod;
	slider->holding = false;
	slider->onScreen = false;

	GUI_SerializeComponent(context, slider, GUI_COMPONENT_SLIDER);
	GUI_AddEvent(context, slider, &GUI_InternalSliderEvent, GUI_SLIDER_INTERNAL);
	return slider;
}
void GUI_FreeSlider(GUI_Context *context, GUI_Slider *slider) {
	free(slider);
}
void GUI_DrawSlider(GUI_Context *context, GUI_Slider *slider, int x, int y) {
	slider->onScreen = true;
	slider->x = x;
	slider->y = y;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;

	if(slider->vertical) {
		rect.w = slider->width;
		rect.h = slider->length;
	}
	else {
		rect.w = slider->length;
		rect.h = slider->width;
	}

	SDL_SetRenderDrawColor(context->render, slider->sliderColor.r, slider->sliderColor.g, slider->sliderColor.b, slider->sliderColor.a);
	SDL_RenderFillRect(context->render, &rect);

	if(slider->vertical) {
		rect.x = x - slider->width;
		rect.y = y + slider->length - slider->width - (slider->length - slider->width) * slider->value;
		rect.w = slider->width * 3;
		rect.h = slider->width;
	}
	else {
		rect.x = x + (slider->length - slider->width) * slider->value;
		rect.y = y - slider->width;
		rect.w = slider->width;
		rect.h = slider->width * 3;
	}

	SDL_SetRenderDrawColor(context->render, slider->handleColor.r, slider->handleColor.g, slider->handleColor.b, slider->handleColor.a);
	SDL_RenderFillRect(context->render, &rect);

	if(slider->inc != NULL) {
		if(slider->vertical) {
			GUI_DrawButton(context, slider->inc, x - (slider->inc->width - slider->width) / 2, y - slider->inc->height - slider->width);
			GUI_DrawButton(context, slider->dec, x - (slider->inc->width - slider->width) / 2, y + slider->length + slider->width);
		}
		else {
			GUI_DrawButton(context, slider->dec, x - slider->dec->width - slider->width, y - (slider->dec->height - slider->width) / 2);
			GUI_DrawButton(context, slider->inc, x + slider->length + slider->width, y - (slider->inc->height - slider->width) / 2);
		}
	}
}

bool GUI_IsPointInsideRect(int mx, int my, int x, int y, int w, int h) {
	if(mx >= x && my >= y && mx <= x + w && my <= y + h)
		return true;

	return false;
}
bool GUI_AddEvent(GUI_Context *context, void* component, GUI_Event event, GUI_EventType type) {
	GUI_ComponentNode *compNode = context->components;

	while(compNode != NULL && compNode->component != component) {
		compNode = compNode->next;
	}

	if(compNode == NULL) {
		return true;
	}
	GUI_EventNode **node = &compNode->events;
	GUI_EventNode *prev = NULL;

	if(type == GUI_BUTTON_ON_PRESS || type == GUI_BUTTON_ON_RELEASE || type == GUI_BUTTON_ON_ENTER || type == GUI_BUTTON_ON_EXIT) {
		if(compNode->type != GUI_COMPONENT_BUTTON) {
			return true;
		}
	}
	else if(type == GUI_SLIDER_INTERNAL || type == GUI_SLIDER_ON_HOLD || type == GUI_SLIDER_ON_RELEASE) {
		if(compNode->type != GUI_COMPONENT_SLIDER) {
			return true;
		}
	}
	else {
		return true;
	}
	
	while(*node != NULL) {
		prev = *node;
		node = &(*node)->next;
	}

	*node = (GUI_EventNode*) malloc(sizeof(GUI_EventNode));
	(*node)->type = type;
	(*node)->event = event;
	(*node)->prev = prev;
	(*node)->next = NULL;

	return false;
}
bool GUI_ClearEvents(GUI_Context *context, void *component) {
	GUI_ComponentNode *compNode = context->components;

	while(compNode != NULL && compNode->component != component) {
		compNode = compNode->next;
	}

	if(compNode == NULL)
		return true;

	while(compNode->events != NULL) {
		GUI_EventNode *next = compNode->events->next;
		free(compNode->events);
		compNode->events = next;
	}

	return false;
}
bool GUI_RemoveEvent(GUI_Context *context, void *component, GUI_Event event, GUI_EventType type) {
	GUI_ComponentNode *compNode = context->components;

	while(compNode != NULL && compNode->component != component) {
		compNode = compNode->next;
	}

	if(compNode == NULL)
		return true;

	GUI_EventNode *eventNode = compNode->events;
	while(eventNode != NULL && !(eventNode->type == type && eventNode->event == event)) {
		eventNode = eventNode->next;
	}

	if(eventNode == NULL)
		return true;

	if(eventNode->prev != NULL) {
		eventNode->prev->next = eventNode->next;

		if(eventNode->next != NULL)
			eventNode->next->prev = eventNode->prev;
	}
	else {
		eventNode->next->prev = NULL;
	}

	free(eventNode);
	return false;
}
bool GUI_TriggerEvents(GUI_Context *context, void *component, GUI_EventType type) {
	GUI_ComponentNode *compNode = context->components;
	while(compNode != NULL && compNode->component != component) {
		compNode = compNode->next;
	}

	if(compNode == NULL)
		return true;

	GUI_EventNode *eventNode = compNode->events;
	while(eventNode != NULL) {
		if(eventNode->type == type)
			eventNode->event(component);

		eventNode = eventNode->next;
	}

	return false;
}
bool GUI_Update(GUI_Context *context, SDL_Event event) {
	GUI_ComponentNode *compNode = context->components;
	bool handled = false;

	while(compNode != NULL) {
		if(compNode->type == GUI_COMPONENT_BUTTON) {
			GUI_Button *button = (GUI_Button*) compNode->component;
			if(button->onScreen)
				button->onScreen = false;
			else
				goto next;

			if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(GUI_IsPointInsideRect(event.button.x, event.button.y, button->x, button->y, button->width, button->height)) {
					handled = true;
					button->pressed = true;
					GUI_TriggerEvents(context, button, GUI_BUTTON_ON_PRESS);
				}
			}
			else if(event.type == SDL_MOUSEBUTTONUP && button->pressed) {
				handled = true;
				button->pressed = false;
				GUI_TriggerEvents(context, button, GUI_BUTTON_ON_RELEASE);
			}
			else if(event.type == SDL_MOUSEMOTION) {
				if(!button->inside && GUI_IsPointInsideRect(event.motion.x, event.motion.y, button->x, button->y, button->width, button->height)) {
					handled = true;
					button->inside = true;
					GUI_TriggerEvents(context, button, GUI_BUTTON_ON_ENTER);
				}
				else if(button->inside && (event.motion.x < button->x || event.motion.y < button->y || event.motion.x > button->x + button->width || event.motion.y > button->y + button->height)) {
					handled = true;
					button->inside = false;
					GUI_TriggerEvents(context, button, GUI_BUTTON_ON_EXIT);
				}
			}
		}
		else if(compNode->type == GUI_COMPONENT_SLIDER) {
			GUI_Slider *slider = (GUI_Slider*) compNode->component;

			if(slider->onScreen)
				slider->onScreen = false;
			else
				goto next;

			if(event.type == SDL_MOUSEBUTTONDOWN && !slider->holding) {
				SDL_Rect rect;

				if(slider->vertical) {
					rect.x = slider->x - slider->width;
					rect.y = slider->y + slider->length - slider->width - (slider->length - slider->width) * slider->value;
					rect.w = slider->width * 3;
					rect.h = slider->width;
				}
				else {	
					rect.x = slider->x + (slider->length - slider->width) * slider->value;
					rect.y = slider->y - slider->width;
					rect.w = slider->width;
					rect.h = slider->width * 3;
				}

				if(GUI_IsPointInsideRect(event.button.x, event.button.y, rect.x, rect.y, rect.w, rect.h)) {
					handled = true;
					slider->holding = true;
					GUI_TriggerEvents(context, slider, GUI_SLIDER_ON_HOLD);
				}
			}
			else if(event.type == SDL_MOUSEBUTTONUP) {
				if(slider->inc != NULL && slider->dec != NULL && GUI_IsPointInsideRect(event.button.x, event.button.y, slider->dec->x, slider->dec->y, slider->dec->width, slider->dec->height)) {
					handled = true;
					slider->value -= slider->buttonValueMod;
					if(slider->value < 0.0) {
						slider->value = 0.0;
					}
				}
				else if(slider->inc != NULL && slider->dec != NULL && GUI_IsPointInsideRect(event.button.x, event.button.y, slider->inc->x, slider->inc->y, slider->inc->width, slider->inc->height)) {
					handled = true;
					slider->value += slider->buttonValueMod;
					if(slider->value > 1.0) {
						slider->value = 1.0;
					}
				}
				else if(slider->holding) {
					handled = true;
					slider->holding = false;
					GUI_TriggerEvents(context, slider, GUI_SLIDER_ON_RELEASE);
				}
			}
			else if(event.type == SDL_MOUSEMOTION && slider->holding) {
				handled = true;
				GUI_TriggerEvents(context, slider, GUI_SLIDER_INTERNAL);
			}
		}

		next:
		compNode = compNode->next;
	}

	return handled;
}
