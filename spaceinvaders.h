#include <stdlib.h>
#include "cpu.h"

typedef struct Shifter {
    uint8_t shift0;
    uint8_t shift1;
    uint8_t offset;
} Shifter;

typedef struct Ports {
    uint8_t port_0;
    uint8_t port_1;
    uint8_t port_2;
    uint8_t port_3;
    uint8_t port_4;
    uint8_t port_5;
    uint8_t port_6;
} Ports;

typedef struct Hardware {
    State8080 state;
    Shifter shifter;
    Ports ports;
    uint8_t screen_buffer[256][224][4];
} Hardware;

void hw_in(unsigned char *op_code);
void hw_out(unsigned char *op_code);
void handle_input(Ports *ports);
void generate_interrupt(State8080 *state, int interrupt_num);
void init();
void quit();
uint16_t si_memory_mapping_read(uint16_t address);
void si_memory_mapping_write(uint16_t address, uint8_t data);
void update_screen_buffer();
void render();
void update_texture();