

macos: spaceinvaders.c cpu.c emulation.c
	clang -o space spaceinvaders.c cpu.c emulation.c -Iinclude/ -L/opt/homebrew/lib -lSDL2 -O3
