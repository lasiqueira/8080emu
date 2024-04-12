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
} Hardware;

void hw_in(Hardware *hardware, unsigned char *op_code);
void hw_out(Hardware *hardware, unsigned char *op_code);
void handle_input(Ports *ports);
void generate_interrupt(State8080 *state, int interrupt_num);