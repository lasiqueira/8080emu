#include <stdlib.h>
#include <stdint.h>
#ifndef _CPU_H
#define _CPU_H
typedef struct ConditionCodes {
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
}ConditionCodes;

typedef struct State8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    uint8_t int_enable;
} State8080;

State8080 initialize_state();
void read_rom_to_memory(State8080 *state, char *file_name, int position);
uint8_t* get_m(State8080 *state);
void print_state(State8080 *state);
#endif