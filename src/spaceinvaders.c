#include <SDL3/SDL.h>
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
const SDL_PixelFormatDetails *g_format;

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
        if(!g_hardware.state.halt)
        {
            done = emulate_8080_op(&g_hardware.state);

        }

        if (SDL_GetTicks() - last_interrupt > (1.0 / FRAME_RATE) * 1000)
        {   
            handle_input(&g_hardware.ports);

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
        if (event.type == SDL_EVENT_QUIT)
        {
            exit(0);
        }
        
        if(event.type == SDL_EVENT_KEY_DOWN)
        {   
            switch(event.key.key)
            {
                case SDLK_C:  // Insert coin
                    ports->port_1 |= 1;
                    break;
                case SDLK_S:  // P1 Start
                    ports->port_1 |= 1 << 2;
                    break;
                case SDLK_W: // P1 Shoot
                    ports->port_1 |= 1 << 4;
                    break;
                case SDLK_A: // P1 Move Left
                    ports->port_1 |= 1 << 5;
                    break;
                case SDLK_D: // P1 Move Right
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
        else if(event.type == SDL_EVENT_KEY_UP)
        {
            switch(event.key.key)
            {
                case SDLK_C:  // Insert coin
                    ports->port_1 &= ~1;
                    break;
                case SDLK_S:  // P1 Start
                    ports->port_1 &= ~(1 << 2);
                    break;
                case SDLK_W: // P1 Shoot
                    ports->port_1 &= ~(1 << 4);
                    break;
                case SDLK_A: // P1 Move Left
                    ports->port_1 &= ~(1 << 5);
                    break;
                case SDLK_D: // P1 Move Right
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
    state->halt = 0;
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

    char exe_path[1024];
    get_executable_path(exe_path, sizeof(exe_path));

    char file1_path[1024], file2_path[1024], file3_path[1024], file4_path[1024];
    construct_rom_path(exe_path, FILE1, file1_path, sizeof(file1_path));
    construct_rom_path(exe_path, FILE2, file2_path, sizeof(file2_path));
    construct_rom_path(exe_path, FILE3, file3_path, sizeof(file3_path));
    construct_rom_path(exe_path, FILE4, file4_path, sizeof(file4_path));

    // Load ROM files
    read_rom_to_memory(&g_hardware.state, file1_path, 0x0000);
    read_rom_to_memory(&g_hardware.state, file2_path, 0x0800);
    read_rom_to_memory(&g_hardware.state, file3_path, 0x1000);
    read_rom_to_memory(&g_hardware.state, file4_path, 0x1800);

    //Initiating SDL

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to initialise SDL: %s", SDL_GetError());
        exit(1);
    }

    if(!SDL_CreateWindowAndRenderer("Space Invaders", SCREEN_WIDTH*2, SCREEN_HEIGHT*2 , SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, &g_window, &g_renderer))
    {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        exit(1);
    }

    
    SDL_SetWindowMinimumSize(g_window, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_SetRenderLogicalPresentation(g_renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);

    g_texture = SDL_CreateTexture(
        g_renderer,
        SDL_PIXELFORMAT_RGBX32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (g_texture == NULL)
    {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        exit(1);
    }
	SDL_SetTextureScaleMode(g_texture, SDL_SCALEMODE_NEAREST);

    g_format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBX32);
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
    if (!SDL_LockTexture(g_texture, NULL, &pixels, &pitch)) {
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
	SDL_RenderTexture(g_renderer, g_texture, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}

void get_executable_path(char* buffer, size_t size) {
    #ifdef _WIN32
    // Windows-specific logic
    GetModuleFileName(NULL, buffer, size);
    char* last_slash = strrchr(buffer, '\\');
    if (last_slash) {
        *last_slash = '\0'; // Remove the executable name to get the directory
    }
    #elif __APPLE__
    // macOS-specific logic
    uint32_t bufsize = (uint32_t)size;
    if (_NSGetExecutablePath(buffer, &bufsize) != 0) {
        fprintf(stderr, "Buffer too small; need size %u\n", bufsize);
        exit(1);
    }
    char* last_slash = strrchr(buffer, '/');
    if (last_slash) {
        *last_slash = '\0'; // Remove the executable name to get the directory
    }
    #else
    // Linux-specific logic
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len != -1) {
        buffer[len] = '\0';
        char* last_slash = strrchr(buffer, '/');
        if (last_slash) {
            *last_slash = '\0'; // Remove the executable name to get the directory
        }
    } else {
        perror("readlink");
        exit(1);
    }
    #endif
}

void construct_rom_path(const char* exe_path, const char* relative_path, char* output_path, size_t size) {
    char project_root[1024];
    strncpy(project_root, exe_path, sizeof(project_root));
    project_root[sizeof(project_root) - 1] = '\0';
    // Go up one level from build directory
    #ifdef _WIN32
    char* last_slash = strrchr(project_root, '\\');
    #else
    char* last_slash = strrchr(project_root, '/');
    #endif
    if (last_slash) {
        *last_slash = '\0';
    } else {
        fprintf(stderr, "Failed to locate project root\n");
        exit(1);
    }
    // Append the relative path (e.g., "rom/invaders.h")
    snprintf(output_path, size, "%s/%s", project_root, relative_path);
}