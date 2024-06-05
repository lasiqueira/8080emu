#include <SDL2/SDL.h>
#include <time.h>
#include "emulation.h"
#include "spaceinvaders.h"

#define SCREEN_WIDTH  224
#define SCREEN_HEIGHT  256
#define FILE1 "rom/invaders.h"
#define FILE2 "rom/invaders.g"
#define FILE3 "rom/invaders.f"
#define FILE4 "rom/invaders.e"

Hardware g_hardware;
SDL_Renderer *g_renderer;
SDL_Window *g_window;
SDL_Texture *g_texture;


int main(int argc, char**argv)
{
    int done = 0;
    
    init();

    time_t last_interrupt = 0;
    
    print_state(&g_hardware.state);   
    
    while(done == 0)
    {   
        handle_input(&g_hardware.ports);

        uint8_t *op_code= &g_hardware.state.memory[g_hardware.state.pc];
 
        
        done= emulate_8080_op(&g_hardware.state);
        
        if(time(NULL) - last_interrupt > 1.0/60.0)
        {
            if(g_hardware.state.int_enable)
            {
                generate_interrupt(&g_hardware.state, 2);
                last_interrupt = time(NULL);
            }
        }
        //TODO render
        
    }
   
    quit();

    return 0;
}



//SPACE INVADERS
void hw_in(unsigned char *op_code)
{
    printf("-----HWIN-----\n");
    switch(op_code[1])
    {
        case 0x00: 
        g_hardware.state.a = g_hardware.ports.port_0;
        break;
        case 0x01: 
        g_hardware.state.a = g_hardware.ports.port_1;
        break;
        case 0x02: 
        g_hardware.state.a = g_hardware.ports.port_2;
        break;
        case 0x03:{ 
        uint16_t v = (g_hardware.shifter.shift1<<8) | g_hardware.shifter.shift0;
        g_hardware.state.a = ((v >> (8- g_hardware.shifter.offset)) & 0xff);
        }
        break;
    }
}

void hw_out(unsigned char *op_code)
{
    printf("-----HWOUT-----\n");
    switch(op_code[1])
    {    
        case 0x02: 
        g_hardware.shifter.offset = g_hardware.state.a & 0x7; //bits 012
        break;
        case 0x03: 
        g_hardware.ports.port_3 = g_hardware.state.a;
        break;
        case 0x04: 
        g_hardware.shifter.shift0 = g_hardware.shifter.shift1;
        g_hardware.shifter.shift1 = g_hardware.state.a;
        break;
        case 0x05: 
        g_hardware.ports.port_5 = g_hardware.state.a;
        break;
        case 0x06: break;
    }
}

void handle_input(Ports *ports)
{
    
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {   
        if(event.type == SDL_KEYDOWN)
        {   
            switch(event.key.keysym.sym)
            {
                case SDLK_c:  // Insert coin
                    ports->port_1 |= 1;
                    break;
                case SDLK_s:  // P1 Start
                    ports->port_1 |= 1 << 2;
                    break;
                case SDLK_w: // P1 Shoot
                    ports->port_1 |= 1 << 4;
                    break;
                case SDLK_a: // P1 Move Left
                    ports->port_1 |= 1 << 5;
                    break;
                case SDLK_d: // P1 Move Right
                    ports->port_1 |= 1 << 6;
                    break;
                case SDLK_LEFT: // P2 Move Left
                    ports->port_2 |= 1 << 5;
                    break;
                case SDLK_RIGHT: // P2 Move Right
                    ports->port_2 |= 1 << 6;
                    break;
                case SDLK_RETURN: // P2 Start
                    ports->port_1 |= 1 << 1;
                    break;
                case SDLK_UP: // P2 Shoot
                    ports->port_2 |= 1 << 4;
                    break;
            }
        }
        if(event.type == SDL_KEYUP)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_c:  // Insert coin
                    ports->port_1 &= ~1;
                    break;
                case SDLK_s:  // P1 Start
                    ports->port_1 &= ~(1 << 2);
                    break;
                case SDLK_w: // P1 Shoot
                    ports->port_1 &= ~(1 << 4);
                    break;
                case SDLK_a: // P1 Move Left
                    ports->port_1 &= ~(1 << 5);
                    break;
                case SDLK_d: // P1 Move Right
                    ports->port_1 &= ~(1 << 6);
                    break;
                case SDLK_LEFT: // P2 Move Left
                    ports->port_2 &= ~(1 << 5);
                    break;
                case SDLK_RIGHT: // P2 Move Right
                    ports->port_2 &= ~(1 << 6);
                    break;
                case SDLK_RETURN: // P2 Start
                    ports->port_1 &= ~(1 << 1);
                    break;
                case SDLK_UP: // P2 Shoot
                    ports->port_2 &= ~(1 << 4);
                    break;
            }
        }
        //TODO handle gamepad input

        if(event.type == SDL_QUIT)
        {
            exit(0);
        }
    }

}

void generate_interrupt(State8080 *state, int interrupt_num)
{
   push(state, (uint8_t)(state->pc & 0xff00) >> 8, (uint8_t)(state->pc & 0xff));
   state->pc = 8 * interrupt_num;
}


void init() {
   //Initiating hardware
    g_hardware.state = initialize_state();
    //space invaders mirrors the memory in addresses above 0x4000 and we will handle by moving the address back if >= 0x4000
    g_hardware.state.memory = (uint8_t*)malloc(0x4000);
    // initiate function pointers
    in_ptr = &hw_in;
    out_ptr = &hw_out;
    memory_mapping_ptr = &si_memory_mapping;

    //TODO allocate buffer for screen
    read_rom_to_memory(&g_hardware.state, FILE1, 0x0000);
    read_rom_to_memory(&g_hardware.state, FILE2, 0x0800);
    read_rom_to_memory(&g_hardware.state, FILE3, 0x1000);
    read_rom_to_memory(&g_hardware.state, FILE4, 0x1800);
    

    //Initiating SDL

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Failed to initialise SDL: %s", SDL_GetError());
        exit(1);
    }

    g_window = SDL_CreateWindow(
        "Space Invaders",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);


    if (g_window == NULL)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        exit(1);
    }
    SDL_SetWindowMinimumSize(g_window, SCREEN_WIDTH, SCREEN_HEIGHT);

   g_renderer = SDL_CreateRenderer(
        g_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (g_renderer == NULL)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        exit(1);
    }

    SDL_RenderSetLogicalSize(g_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    g_texture = SDL_CreateTexture(
        g_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (g_texture == NULL)
    {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        exit(1);
    }


}
void quit()
{   
	free(g_hardware.state.memory);
    //free(g_hardware->screen_buffer);
    SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
	SDL_Quit();
	exit(0);
}

uint16_t si_memory_mapping(uint16_t address)
{
	if(address >= 0x4000)
	{
        address = 0x2000 + (address & 0x1FFF);
	}
	return address;
}