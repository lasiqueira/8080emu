

macos-release: spaceinvaders.c cpu.c emulation.c
	clang -o space spaceinvaders.c cpu.c emulation.c -Iinclude/ -L/opt/homebrew/lib -lSDL2 -O3 -DNDEBUG

macos-debug: spaceinvaders.c cpu.c emulation.c
	clang -o space spaceinvaders.c cpu.c emulation.c -Iinclude/ -L/opt/homebrew/lib -lSDL2 -g