# Documentation for sdl2_gui
## Macro Definitions
These macros represent an `SDL_Color` struct
* `COLOR_RED`
* `COLOR_GREEN`
* `COLOR_BLUE`
* `COLOR_CYAN`
* `COLOR_MAGENTA`
* `COLOR_YELLOW`
* `COLOR_BLACK`
* `COLOR_DARK_GREY`
* `COLOR_LIGHT_GREY`
* `COLOR_WHITE`

## Enums
### GUI_ComponentType
* GUI_COMPONENT_TEXT
* GUI_COMPONENT_IMAGE
* GUI_COMPONENT_BUTTON
* GUI_COMPONENT_SLIDER

### GUI_EventType
* GUI_BUTTON_ON_PRESS
* GUI_BUTTON_ON_RELEASE
* GUI_BUTTON_ON_ENTER
* GUI_BUTTON_ON_EXIT
* GUI_SLIDER_INTERNAL

## Typedefs
### `typedef int GUI_FontID`
This is returned by `GUI_NewFont` and used to reference your loaded font when making text components.

### `typedef void (*GUI_Event)(void* component)`
Event callback function. See the `Events` section for details.

## Context
```
typedef struct {
    SDL_Window *window;
    SDL_Renderer *render;
    int width, height;
    GUI_ComponentNode *components;
    TTF_Font **fonts;
    int numFonts;
    GUI_EventNode *events;
} GUI_Context; 
```

### `GUI_Context* GUI_Init(SDL_Window *win, SDL_Renderer *render)`
Initializes a GUI_Context to use with all functions in sdl2_gui. This function also initializes the `SDL_ttf` and `SDL_image` libraries.

`components` is a linked list of container structures that hold a pointer and some data for each serialized component. When ever a new component is created, it automatically gets serialized automatically, and then freed when `GUI_Quit` is called. It is also possible to free components manually by calling `GUI_FreeComponent`.

`fonts` is a list of fonts added with `GUI_NewFont`. **WARNING** Right now, sdl2_gui only supports TTF fonts.

`events` is a linked list of event listener structures. More Info under the Events header.
### `void GUI_Quit(GUI_Context *context)`
Frees all components, fonts, and events from memory, and terminates `SDL_ttf` and `SDL_image`.

## Components
### GUI_Text
```
typedef struct {
    SDL_Texture* texture;
    int width, height;
}
```

#### `GUI_Text* GUI_NewTextBg(GUI_Context *context, GUI_FontID font, const char *str, SDL_Color fg, SDL_Color bg)`
Creates and serializes a new text component with a colored highlight/background.

#### `GUI_Text* GUI_NewText(GUI_Context *context, GUI_FontID font, const char *str, SDL_Color fg)`
Creates and serializes a new text component without a background.

#### `void GUI_FreeText(GUI_Context *context, GUI_Text *text)`
This function is used inside of `GUI_FreeComponent` to free the acual component from memory, but not the container data surrounding the component. Do not use this function. Use `GUI_FreeComponent` instead.

#### `void GUI_DrawText(GUI_Context *context, GUI_Text *text, int x, int y)`
Draws a text component to the screen at position (x, y). All drawing functions in this library should be called in between your `SDL_RenderClear` and `SDL_RenderPresent` function calls.

### GUI_Image
```
typedef struct {
    SDL_Texture *texture;
    int x, y;
} GUI_Image;
```

#### `GUI_Image* GUI_NewImage(GUI_Context *context, const char *path, float xScale, float yScale)`
BMP, PNG, JPG, WEBP, TIF image files are supported. This function uses `SDL_image` and SDL's builtin BMP functions to load images, and scales them accordingly.

#### `void GUI_FreeImage(GUI_Context *context, GUI_Image *image)`
Read the section for `GUI_FreeText` for details. Do not use this function.

#### `void GUI_DrawImage(GUI_Context *context, GUI_Image *image, int x, int y)`
Draws the image texture to the screen.

### GUI_Button
```
typedef struct {
    GUI_Text *textContext;
    GUI_Image *imageContext;
    int x, y, width, height;
    int borderWidth;
    SDL_Color fillColor, borderColor;
    bool inside, pressed, onScreen;
}
```

#### `GUI_Button* GUI_NewButton(GUI_Context *context, int width, int height, int borderWidth, SDL_Color fillColor, SDL_Color borderColor, GUI_Text *textContent, GUI_Image *imageContent)`
Generates a button with no events attached. If 0 is passed for `borderWidth`, no border is drawn and `borderColor` is ignored. `textContext` and `imageContext` can be NULL, in which case they will be ignored. If both values passed are non-NULL, the image will be drawn above the text. These content components are drawn in the center of the button, and are **not** freed if `GUI_FreeComponent` is called on the parent button component..

#### `void GUI_FreeButton(GUI_Context *context, GUI_Button *button)`
Read the section for `GUI_FreeText` for details. Do not use this function.

#### `void GUI_DrawButton(GUI_context *context, GUI_Button *button, int x, int y)`
Draws the button to the screen. The button's `onScreen` value is also flagged true, which allows it to see input events. If the button has not been drawn after its last handled input event, it will stop receiving input until redrawn. The button's position coordinates are also updated to be used when handling events internally.

### GUI_Slider
```
typedef struct {
    GUI_Button *inc, *dec;
    int x, y, width, length;
    bool vertical;
    SDL_Color sliderColor, handleColor;
    float value, buttonValueMod;
    bool holding, onScreen;
}
```

#### `GUI_Slider* GUI_NewSlider(GUI_Context *context, GUI_Button *incrementButton, GUI_Button *decrementButton, float buttonValueMod, int width, int length, bool vertical, SDL_Color sliderColor, SDL_Color handleColor)`
Creates a new slider component, which can be horizontal or vertical. `incrementButton` and `decrementButton` are optional, but if just one is NULL, the other is ignored. `buttonValueMod` is the amount the slider's `value` increases or decreases when the increment/decrement buttons are clicked, and will be ignored if no buttons are used. `value` is an interpolated float that is in between 0 and 1.

#### `void GUI_FreeSlider(GUI_Context *context, GUI_Slider *slider)`
Read the section for `GUI_FreeText` for details. Do not use this function.

#### `void GUI_DrawSlider(GUI_Context *context, GUI_Slider *slider, int x, int y)`
The slider is drawn relative to the bar, not the increment/decrement buttons. The slider's handle's width is 3 times the width of the bar, and its length is equal to the width of the bar. Like the button component, it will only receive events while on screen.

## Events
```
void GUI_Event(void *component) {

}
```
Input events are handled in these callback functions. `component` can be typecasted to its correlative component type to access the component anonymously. It is not required to be used, and is mostly there for sliders, which make use of the event system internally.

### `void GUI_SetEvent(GUI_Context *context, void *component, GUI_Event event, GUI_EventType type)`
Binds the given event callback to the component to be called when an event of the given type occurs. As of now, only one event callback can be added to a component for each event type. If you attempt to add a second callback for the same event type, this function will simply return without doing anything.

### `void GUI_RemoveEvent(GUI_context *context, void *component, GUI_EventType type)`
Unbinds the event callback of the given type from the component.

### `void GUI_UpdateEvents(GUI_Context *context, SDL_Event event)`
This function is expected to be placed inside of your `SDL_PollEvent` loop. It checks registered event callbacks and calls them if their conditions have been met.

## Misc
### `void GUI_SerializeComponent(GUI_Context *context, void *component, GUI_ComponentType type)`
This function is automatically called when a new component is created. It registers the component to be freed from memory when `GUI_Quit` is called.

### `void GUI_FreeComponent(GUI_Context *context, void *component)`
If you need to destroy and recreate a component during program runtime, this is the function you should use. It removes the component from the internal list, freeing all component and container data from memory.

### `GUI_FontID GUI_NewFont(GUI_context *context, const char *path, int ptsize)`
Loads a TTF font and returns a `GUI_FontID` to reference it with. All fonts loaded with this function are freed when `GUI_Quit` is called.

### `bool GUI_IsPointInsideRect(int mx, int my, int x, int y, int w, int h)`
Utility function used internally. Returns true if point (`mx`, `my`) lies inside of the given box coordinates.
