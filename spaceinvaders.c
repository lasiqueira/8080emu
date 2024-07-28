#include <SDL2/SDL.h>
#include "emulation.h"
#include "spaceinvaders.h"
#include <stdbool.h>

#define SCREEN_WIDTH  224
#define SCREEN_HEIGHT  256

#define FILE1 "rom/invaders.h"
#define FILE2 "rom/invaders.g"
#define FILE3 "rom/invaders.f"
#define FILE4 "rom/invaders.e"

#define RAM_ADDR 0x2000
#define RAM_MIRROR_ADDR 0x4000
#define VRAM_ADDR 0x2400

#define FRAME_RATE 60

Hardware g_hardware;
SDL_Renderer *g_renderer;
SDL_Window *g_window;
SDL_Texture *g_texture;
SDL_PixelFormat* g_format;

int main(int argc, char**argv)
{
    int done = 0;
    
    init();

    uint32_t last_interrupt = SDL_GetTicks();
    
    #ifndef NDEBUG
    print_state(&g_hardware.state);
    #endif
    int interrupt_num = 0x08;
    while(done == 0)
    {
        handle_input(&g_hardware.ports);
        done = emulate_8080_op(&g_hardware.state);

        if (SDL_GetTicks() - last_interrupt > (1.0 / FRAME_RATE) * 1000)
        {   
            if (g_hardware.state.int_enable)
            {
                generate_interrupt(&g_hardware.state, interrupt_num);
               
                update_screen_buffer();
                update_texture();

                render();
			
                interrupt_num = (interrupt_num == 0x08) ? 0x10 : 0x08;
                last_interrupt = SDL_GetTicks();
            }
           
        }
       
        // Limit FPS to 60
        SDL_Delay(1000 / 60);
    }
   
    quit();

    return 0;
}

//SPACE INVADERS
void hw_in(unsigned char *op_code)
{
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
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        
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
                case SDLK_1:
                    g_hardware.colour = ~g_hardware.colour;
                    break;
            }
        }
        else if(event.type == SDL_KEYUP)
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

        
    }

}

void generate_interrupt(State8080 *state, int interrupt_num)
{
    rst(state, interrupt_num);
}


void init() {
   //Initiating hardware
    g_hardware.state = initialize_state();
    //space invaders mirrors the memory in addresses above 0x4000 and we will handle by moving the address back if >= 0x4000
    g_hardware.state.memory = (uint8_t*)malloc(0x4000);
    memset(g_hardware.state.memory, 0, 0x4000);
    // initiate function pointers
    in_ptr = &hw_in;
    out_ptr = &hw_out;
    memory_mapping_read_ptr = &si_memory_mapping_read;
    memory_mapping_write_ptr = &si_memory_mapping_write;

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
        SDL_RENDERER_ACCELERATED 
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

    g_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    if (g_format == NULL) {
        SDL_Log("Failed to allocate pixel format: %s", SDL_GetError());
        exit(1);
    }
}
void quit()
{   
	free(g_hardware.state.memory);
    free(g_hardware.screen_buffer);
    SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
	SDL_Quit();
	exit(0);
}

uint16_t si_memory_mapping_read(uint16_t address)
{
	if(address >= RAM_MIRROR_ADDR && address < 0x6000)
	{
        address -= 0x2000;
	}
	return address;
}
 
void si_memory_mapping_write(uint16_t address, uint8_t data)
{
    if (address >= 0x2000 && address < 0x4000) {
        g_hardware.state.memory[address] = data;
    }
}
void update_screen_buffer()
{
    for (int i = 0; i < 256 * 224 / 8; i++) {
        const int y = i * 8 / 256;
        const int base_x = (i * 8) % 256;
        const uint8_t cur_byte = g_hardware.state.memory[VRAM_ADDR + i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            int px = base_x + bit;
            int py = y;
            const bool is_pixel_lit = (cur_byte >> bit) & 1;
            uint8_t r = 0, g = 0, b = 0;
            if (!g_hardware.colour && is_pixel_lit) {
                r = 255;
                g = 255;
                b = 255;
            }
            else if (g_hardware.colour && is_pixel_lit) {
                if (px < 16) {
                    if (py < 16 || py > 118 + 16) {
                        r = 255;
                        g = 255;
                        b = 255;
                    }
                    else {
                        g = 255;
                    }
                }
                else if (px >= 16 && px <= 16 + 56) {
                    g = 255;
                }
                else if (px >= 16 + 56 + 120 && px < 16 + 56 + 120 + 32) {
                    r = 255;
                }
                else {
                    r = 255;
                    g = 255;
                    b = 255;
                }
            }
            
            const int temp_x = px;
            px = py;
            py = -temp_x + SCREEN_HEIGHT - 1;

            g_hardware.screen_buffer[py][px][0] = r;
            g_hardware.screen_buffer[py][px][1] = g;
            g_hardware.screen_buffer[py][px][2] = b;
        }
    }
}

void update_texture()
{
    int pitch = 0;
    void* pixels = NULL;
    if (SDL_LockTexture(g_texture, NULL, &pixels, &pitch) != 0) {
        SDL_Log("Unable to lock texture: %s", SDL_GetError());
    }
    else {
        memcpy(pixels, g_hardware.screen_buffer, pitch * SCREEN_HEIGHT);
    }
    SDL_UnlockTexture(g_texture);
}
void render()
{	
    SDL_RenderClear(g_renderer);
	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}