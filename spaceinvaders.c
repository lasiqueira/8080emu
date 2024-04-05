#include <SDL2/SDL.h>
#include <time.h>
#include "emulation.h"
#include "spaceinvaders.h"

int main(int argc, char**argv)
{
    int done = 0;
    State8080 state = initialize_state();
    Hardware hardware = {0};
    read_to_memory(&state, argv[1]); 
    print_state(&state);   
    SDL_Init(SDL_INIT_EVERYTHING);
    time_t last_interrupt = 0;
    while(done == 0)
    {   
        handle_input(&hardware.ports);
        //SPACE INVADERS I/O handling
        uint8_t *op_code= &state.memory[state.pc];
        if(*op_code == 0xdb)
        {
            hw_in(&state, &hardware, op_code);
        } 
        else if(*op_code == 0xd3)
        {
            hw_out(&state, &hardware, op_code);
        } 
        else
        {
            done= emulate_8080_op(&state);
        }

        if(time(NULL) - last_interrupt > 1.0/60.0)
        {
            if(state.int_enable)
            {
                generate_interrupt(&state, 2);
                last_interrupt = time(NULL);
            }
        }
        
    }
    SDL_Quit();
    return 0;
}



//SPACE INVADERS
void hw_in(State8080 *state, Hardware *hardware, unsigned char *op_code)
{
    printf("-----HWIN-----\n");
    switch(op_code[1])
    {
        case 0x00: 
        state->a = hardware->ports.port_0;
        break;
        case 0x01: 
        state->a = hardware->ports.port_1;
        break;
        case 0x02: 
        state->a = hardware->ports.port_2;
        break;
        case 0x03:{ 
        uint16_t v = (hardware->shifter.shift1<<8) | hardware->shifter.shift0;
        state->a = ((v >> (8- hardware->shifter.offset)) & 0xff); 
        }
        break;
    }
    state->pc+=1;
}

void hw_out(State8080 *state, Hardware *hardware, unsigned char *op_code)
{
    printf("-----HWOUT-----\n");
    switch(op_code[1])
    {    
        case 0x02: 
        hardware->shifter.offset = state->a & 0x7; //bits 012
        break;
        case 0x03: 
        hardware->ports.port_3 = state->a;
        break;
        case 0x04: 
        hardware->shifter.shift0 = hardware->shifter.shift1;
        hardware->shifter.shift1 = state->a;
        break;
        case 0x05: 
        hardware->ports.port_5 = state->a;
        break;
        case 0x06: break;
    }
    state->pc+=1;
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