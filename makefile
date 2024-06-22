static: sdl2_gui.o
	ar rcs libsdl2gui.a $<
dynamic: sdl2_gui.o.d
	clang -shared -o libsdl2gui.so $<

sdl2_gui.o: sdl2_gui.c
	clang $< -o $@ -c
sdl2_gui.o.d: sdl2_gui.c
	clang $< -o $@ -c -fpic

clean:
	rm *.o *.o.d -f
