#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "emulation.h"
#include "cpu_test.h"


#define FILE1 "../rom/TST8080.COM"

State8080 g_state;
bool done = 0;
void init()
{
    g_state = initialize_state();
    g_state.memory = (uint8_t*)malloc(0x10000);
    memset(g_state.memory, 0, 0x10000);

    in_ptr = &tst_in;
    out_ptr = &tst_out;
    memory_mapping_read_ptr = &memory_mapping_read;
    memory_mapping_write_ptr = &memory_mapping_write;
}
void quit()
{
    free(g_state.memory);
    exit(0);
}
int main()
{   
    init();
    
    //TODO run cpu tests
    read_rom_to_memory(&g_state, FILE1, 0x0100);
    g_state.pc = 0x0100;

    g_state.memory[0x0000] = 0xD3;
    g_state.memory[0x0001] = 0x00;

    g_state.memory[0x0005] = 0xD3;
    g_state.memory[0x0006] = 0x01;
    g_state.memory[0x0007] = 0xC9;
    
    while(!done)
    {
        emulate_8080_op(&g_state);
    }

    quit();
    return 0;
}

void tst_in(unsigned char *op_code)
{
    g_state.a = 0;
}

void tst_out(unsigned char *op_code)
{
    uint8_t port = op_code[1];

    if(port == 0)
    {
        done = 1;
    }
    else if(port == 1)
    {
        uint8_t op = g_state.c;
        if(op == 2)
        {
            printf("%c", g_state.e);
        }
        else if(op == 9)
        {
            uint16_t offset = (g_state.d << 8) | g_state.e;
            while(g_state.memory[offset] != '$')
            {
                printf("%c", g_state.memory[offset]);
                offset++;
            }
        }
    }
}
 
void memory_mapping_write(uint16_t address, uint8_t data)
{
    g_state.memory[address] = data;
}

uint16_t memory_mapping_read(uint16_t address)
{
    return address;
}