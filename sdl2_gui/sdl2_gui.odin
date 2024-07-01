package sdl2_gui

foreign import sdlgui "system:sdl2gui"
import sdl "vendor:sdl2"
import ttf "vendor:sdl2/ttf"
import img "vendor:sdl2/image"

COLOR_RED :: sdl.Color{0xff, 0, 0, 0xff}
COLOR_GREEN :: sdl.Color{0, 0xff, 0, 0xff}
COLOR_BLUE :: sdl.Color{0, 0, 0xff, 0xff}


COLOR_CYAN :: sdl.Color{0, 0xff, 0xff, 0xff}
COLOR_MAGENTA :: sdl.Color{0xff, 0, 0xff, 0xff}
COLOR_YELLOW :: sdl.Color{0xff, 0xff, 0, 0xff}

COLOR_BLACK :: sdl.Color{0, 0, 0, 0xff}
COLOR_DARK_GREY :: sdl.Color{0x44, 0x44, 0x44, 0xff}
COLOR_LIGHT_GREY :: sdl.Color{0xcc, 0xcc, 0xcc, 0xff}
COLOR_WHITE :: sdl.Color{0xff, 0xff, 0xff, 0xff}

FontID :: i32
Event :: proc "c" (component: rawptr)

ComponentType :: enum {
	COMPONENT_TEXT,
	COMPONENT_IMAGE,
	COMPONENT_BUTTON,
	COMPONENT_SLIDER
}

EventType :: enum {
	BUTTON_ON_PRESS,
	BUTTON_ON_RELEASE,
	BUTTON_ON_ENTER,
	BUTTON_ON_EXIT,
	SLIDER_INTERNAL
}

ComponentNode :: struct {
	component: rawptr,
	type: ComponentType,
	prev, next: ^ComponentNode
}

EventNode :: struct {
	event: Event,
	type: EventType,
	component: rawptr,
	prev, next: ^EventNode
}

Context :: struct {
	window: ^sdl.Window,
	render: ^sdl.Renderer,
	width, height: i32,
	components: ^ComponentNode,
	fonts: ^^ttf.Font,
	numFonts: i32,
	events: ^^EventNode
}

Text :: struct {
	texture: ^sdl.Texture,
	width, height: i32
}

Image :: struct {
	texture: ^sdl.Texture,
	width, height: i32
}

Button :: struct {
	textContent: ^Text,
	imageContent: ^Image,
	x, y, width, height, borderWidth: i32,
	fillColor, borderColor: sdl.Color,
	inside, pressed, onScreen: bool
}

Slider :: struct {
	inc, dec: ^Button,
	x, y, width, length: i32,
	vertical: bool,
	sliderColor, handleColor: sdl.Color,
	holding, onScreen: bool
}

@(link_prefix = "GUI_", default_calling_convention = "c")
foreign sdlgui {
	Init :: proc(win: ^sdl.Window, render: ^sdl.Renderer) -> ^Context ---
	Quit :: proc(gui: ^Context) ---

	SerializeComponent :: proc(ctx: ^Context, component: rawptr) ---
	FreeComponent :: proc(ctx: ^Context, component: rawptr) ---
	NewFont :: proc(ctx: ^Context, path: cstring, ptsize: i32) -> FontID ---

	NewTextBg :: proc(ctx: ^Context, font: FontID, str: cstring, fg, bg: sdl.Color) -> ^Text ---
	NewText :: proc(ctx: ^Context, font: FontID, str: cstring, fg: sdl.Color) -> ^Text ---
	FreeText :: proc(ctx: ^Context, text: ^Text) ---
	DrawText :: proc(ctx: ^Context, text: ^Text, x, y: i32) ---

	NewImage :: proc(ctx: ^Context, path: cstring, xScale, yScale: f32) -> ^Image ---
	FreeImage :: proc(ctx: ^Context, image: ^Image) ---
	DrawImage :: proc(ctx: ^Context, image: ^Image, x, y: i32) ---

	NewButton :: proc(ctx: ^Context, width, height, borderWidth: i32, fillColor, borderColor: sdl.Color, textContext: ^Text, imageContext: ^Image) -> ^Button ---
	FreeButton :: proc(ctx: ^Context, button: ^Button) ---
	DrawButton :: proc(ctx: ^Context, button: ^Button, x, y: i32) ---

	NewSlider :: proc(ctx: ^Context, incrementButton, decrementButton: ^Button, buttonValueMod: f32, width, length: i32, vertical: bool, sliderColor, handleColor: sdl.Color) -> ^Slider ---
	FreeSlider :: proc(ctx: ^Context, slider: ^Slider) ---
	DrawSlider :: proc(ctx: ^Context, slider: ^Slider, x, y: i32) ---

	IsPointInsideRect :: proc(mx, my, x, y, w, h: i32) -> bool ---
	AddEvent :: proc(ctx: ^Context, component: rawptr, event: Event, type: EventType) -> bool ---
	ClearEvents :: proc(ctx: ^Context, component: rawptr) -> bool ---
	RemoveEvent :: proc(ctx: ^Context, component: rawptr, event: Event, type: EventType) -> bool ---
	TriggerEvents :: proc(ctx: ^Context, component: rawptr, type: EventType) -> bool ---
	Update :: proc(ctx: ^Context, event: sdl.Event) -> bool ---
}
