example: example.o sdl2_gui.o
	gcc $^ -o $@ -lSDL2 -lSDL2_ttf -lSDL2_image
static: sdl2_gui.o
	ar rcs libsdl2gui.a $<
dynamic: sdl2_gui.o.d
	gcc -shared -o libsdl2gui.so $< -lSDL2 -lSDL2_ttf -lSDL2_image


example.o: src/example.c
	gcc $< -o $@ -c
sdl2_gui.o: src/sdl2_gui.c
	gcc $< -o $@ -c
sdl2_gui.o.d: src/sdl2_gui.c
	gcc $< -o $@ -c -fpic

clean:
	rm *.o *.o.d -f
