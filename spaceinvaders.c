#include <SDL2/SDL.h>
#include <time.h>
#include "emulation.h"
#include "spaceinvaders.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT  600


int main(int argc, char**argv)
{
    int done = 0;
    Hardware hardware = {0};
    hardware.state = initialize_state();
    time_t last_interrupt = 0;
    read_to_memory(&hardware.state, argv[1]); 
    print_state(&hardware.state);   
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Failed to initialise SDL: %s", SDL_GetError());
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow(
        "Space Invaders",
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
           SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    

    if(window == NULL)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return 1;
    }
    SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
         -1,
         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
         );
    if(renderer == NULL)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if(texture == NULL)
    {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return 1;
    }

    while(done == 0)
    {   
        handle_input(&hardware.ports);
        //SPACE INVADERS I/O handling
        uint8_t *op_code= &hardware.state.memory[hardware.state.pc];
        if(*op_code == 0xdb)
        {
            hw_in(&hardware, op_code);
        } 
        else if(*op_code == 0xd3)
        {
            hw_out(&hardware, op_code);
        } 
        else
        {
            done= emulate_8080_op(&hardware.state);
        }

        if(time(NULL) - last_interrupt > 1.0/60.0)
        {
            if(hardware.state.int_enable)
            {
                generate_interrupt(&hardware.state, 2);
                last_interrupt = time(NULL);
            }
        }
        
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



//SPACE INVADERS
void hw_in(Hardware *hardware, unsigned char *op_code)
{
    printf("-----HWIN-----\n");
    switch(op_code[1])
    {
        case 0x00: 
        hardware->state.a = hardware->ports.port_0;
        break;
        case 0x01: 
        hardware->state.a = hardware->ports.port_1;
        break;
        case 0x02: 
        hardware->state.a = hardware->ports.port_2;
        break;
        case 0x03:{ 
        uint16_t v = (hardware->shifter.shift1<<8) | hardware->shifter.shift0;
        hardware->state.a = ((v >> (8- hardware->shifter.offset)) & 0xff); 
        }
        break;
    }
    hardware->state.pc+=1;
}

void hw_out(Hardware *hardware, unsigned char *op_code)
{
    printf("-----HWOUT-----\n");
    switch(op_code[1])
    {    
        case 0x02: 
        hardware->shifter.offset = hardware->state.a & 0x7; //bits 012
        break;
        case 0x03: 
        hardware->ports.port_3 = hardware->state.a;
        break;
        case 0x04: 
        hardware->shifter.shift0 = hardware->shifter.shift1;
        hardware->shifter.shift1 = hardware->state.a;
        break;
        case 0x05: 
        hardware->ports.port_5 = hardware->state.a;
        break;
        case 0x06: break;
    }
    hardware->state.pc+=1;
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