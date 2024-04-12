#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

void print_state(State8080 *state)
{
    printf("\t");
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("%c  ", state->cc.ac ? 'a' : '.');
	printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP $%04x\n", state->a, state->b, state->c,
				state->d, state->e, state->h, state->l, state->sp);
}

State8080 initialize_state()
{
    State8080 state = {0};
    state.memory = malloc(0x10000);
    state.sp = 0xffff;
    return state;
}

void read_to_memory(State8080 *state, char *file_name)
{
     FILE *file = fopen(file_name, "rb");
    if(file == NULL)
    {
        printf("error: Could't open %s\n", file_name);
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    fread(state->memory, file_size, 1, file);
    fclose(file);
}

uint8_t* get_m(State8080 * state)
{
    uint16_t offset =  (state->h<<8) + (state->l);
    return &state->memory[offset];
}