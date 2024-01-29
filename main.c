#include <stdio.h>
#include <stdlib.h>

typedef struct ConditionCodes{
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
}ConditionCodes;

typedef struct State8080{
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

int disassemble_8080_op(unsigned char *code_buffer, int pos);
void unimplemented_instruction(State8080* state);
int emulate_8080_op(State8080* state);
int parity(int x, int size);

int main(int argc, char**argv)
{
    FILE *file = fopen(argv[1], "rb");
    if(file == NULL)
    {
        printf("error: Could't open %s\n", argv[1]);
    }

     //Get the file size and read it into a memory buffer
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    unsigned char *buffer = malloc(file_size);

    fread(buffer, file_size, 1, file);
    fclose(file);

    int pc= 0;

    while(pc < file_size)
    {
        pc += disassemble_8080_op(buffer, pc);
    }

    return 0;
}

int disassemble_8080_op(unsigned char *code_buffer, int pc)
{
    unsigned char *code = &code_buffer[pc];
    int op_bytes = 1;
    printf("%04x ", pc);
    switch(*code){
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x02: printf("STAX   B"); break;
        case 0x03: printf("INX    B"); break;
        case 0x04: printf("INR    B"); break;
        case 0x05: printf("DCR    B"); break;
        case 0x06: printf("MVI    B,#$%02x", code[1]); op_bytes=2; break;
        case 0x07: printf("RLC"); break;
        case 0x08: printf("NOP"); break;
        case 0x09: printf("DAD    B"); break;
        case 0x0a: printf("LDAX   B"); break;
        case 0x0b: printf("DCX    B"); break;
        case 0x0c: printf("INR    C"); break;
        case 0x0d: printf("DCR    C"); break;
        case 0x0e: printf("MVI    C,#$%02x", code[1]); op_bytes=2; break;
        case 0x0f: printf("RRC"); break;

        case 0x10: printf("NOP"); break;
        case 0x11: printf("LXI    D,#$%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x12: printf("STAX   D"); break;
        case 0x13: printf("INX    D"); break;
        case 0x14: printf("INR    D"); break;
        case 0x15: printf("DCR    D"); break;
        case 0x16: printf("MVI    D,#$%02x", code[1]); op_bytes=2; break;
        case 0x17: printf("RAL"); break;
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD    D"); break;
        case 0x1a: printf("LDAX   D"); break;
        case 0x1b: printf("DCX    D"); break;
        case 0x1c: printf("INR    E"); break;
        case 0x1d: printf("DCR    E"); break;
        case 0x1e: printf("MVI    E,#$%02x", code[1]); op_bytes=2; break;
        case 0x1f: printf("RAR"); break;

        case 0x20: printf("RIM"); break;
        case 0x21: printf("LXI    H,#$%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x22: printf("SHLD   $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x23: printf("INX    H"); break;
        case 0x24: printf("INR    H"); break;
        case 0x25: printf("DCR    H"); break;
        case 0x26: printf("MVI    H,#$%02x", code[1]); op_bytes=2; break;
        case 0x27: printf("DAA"); break;
        case 0x28: printf("NOP"); break;
        case 0x29: printf("DAD    H"); break;
        case 0x2a: printf("LHLD   $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x2b: printf("DCX    H"); break;
        case 0x2c: printf("INR    L"); break;
        case 0x2d: printf("DCR    L"); break;
        case 0x2e: printf("MVI    L,#$%02x", code[1]); op_bytes=2; break;
        case 0x2f: printf("CMA"); break;

        case 0x30: printf("SIM"); break;
        case 0x31: printf("LXI    SP,$%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x32: printf("STA    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x33: printf("INX    SP"); break;
        case 0x34: printf("INR    M"); break;
        case 0x35: printf("DCR    M"); break;
        case 0x36: printf("MVI    M,#$%02x", code[1]); op_bytes=2; break;
        case 0x37: printf("STC"); break;
        case 0x38: printf("NOP"); break;
        case 0x39: printf("DAD    SP"); break;
        case 0x3a: printf("LDA    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0x3b: printf("DCX    SP"); break;
        case 0x3c: printf("INR    A"); break;
        case 0x3d: printf("DCR    A"); break;
        case 0x3e: printf("MVI    A,#$%02x", code[1]); op_bytes=2; break;
        case 0x3f: printf("CMC"); break;

        case 0x40: printf("MOV    B,B"); break;
        case 0x41: printf("MOV    B,C"); break;
        case 0x42: printf("MOV    B,D"); break;
        case 0x43: printf("MOV    B,E"); break;
        case 0x44: printf("MOV    B,H"); break;
        case 0x45: printf("MOV    B,L"); break;
        case 0x46: printf("MOV    B,M"); break;
        case 0x47: printf("MOV    B,A"); break;
        case 0x48: printf("MOV    C,B"); break;
        case 0x49: printf("MOV    C,C"); break;
        case 0x4a: printf("MOV    C,D"); break;
        case 0x4b: printf("MOV    C,E"); break;
        case 0x4c: printf("MOV    C,H"); break;
        case 0x4d: printf("MOV    C,L"); break;
        case 0x4e: printf("MOV    C,M"); break;
        case 0x4f: printf("MOV    C,A"); break;

        case 0x50: printf("MOV    D,B"); break;
        case 0x51: printf("MOV    D,C"); break;
        case 0x52: printf("MOV    D,D"); break;
        case 0x53: printf("MOV    D,E"); break;
        case 0x54: printf("MOV    D,H"); break;
        case 0x55: printf("MOV    D,L"); break;
        case 0x56: printf("MOV    D,M"); break;
        case 0x57: printf("MOV    D,A"); break;
        case 0x58: printf("MOV    E,B"); break;
        case 0x59: printf("MOV    E,C"); break;
        case 0x5a: printf("MOV    E,D"); break;
        case 0x5b: printf("MOV    E,E"); break;
        case 0x5c: printf("MOV    E,H"); break;
        case 0x5d: printf("MOV    E,L"); break;
        case 0x5e: printf("MOV    E,M"); break;
        case 0x5f: printf("MOV    E,A"); break;

        case 0x60: printf("MOV    H,B"); break;
        case 0x61: printf("MOV    H,C"); break;
        case 0x62: printf("MOV    H,D"); break;
        case 0x63: printf("MOV    H,E"); break;
        case 0x64: printf("MOV    H,H"); break;
        case 0x65: printf("MOV    H,L"); break;
        case 0x66: printf("MOV    H,M"); break;
        case 0x67: printf("MOV    H,A"); break;
        case 0x68: printf("MOV    L,B"); break;
        case 0x69: printf("MOV    L,C"); break;
        case 0x6a: printf("MOV    L,D"); break;
        case 0x6b: printf("MOV    L,E"); break;
        case 0x6c: printf("MOV    L,H"); break;
        case 0x6d: printf("MOV    L,L"); break;
        case 0x6e: printf("MOV    L,M"); break;
        case 0x6f: printf("MOV    L,A"); break;

        case 0x70: printf("MOV    M,B"); break;
        case 0x71: printf("MOV    M,C"); break;
        case 0x72: printf("MOV    M,D"); break;
        case 0x73: printf("MOV    M,E"); break;
        case 0x74: printf("MOV    M,H"); break;
        case 0x75: printf("MOV    M,L"); break;
        case 0x76: printf("HLT"); break;
        case 0x77: printf("MOV    M,A"); break;
        case 0x78: printf("MOV    A,B"); break;
        case 0x79: printf("MOV    A,C"); break;
        case 0x7a: printf("MOV    A,D"); break;
        case 0x7b: printf("MOV    A,E"); break;
        case 0x7c: printf("MOV    A,H"); break;
        case 0x7d: printf("MOV    A,L"); break;
        case 0x7e: printf("MOV    A,M"); break;
        case 0x7f: printf("MOV    A,A"); break;

        case 0x80: printf("ADD    B"); break;
        case 0x81: printf("ADD    C"); break;
        case 0x82: printf("ADD    D"); break;
        case 0x83: printf("ADD    E"); break;
        case 0x84: printf("ADD    H"); break;
        case 0x85: printf("ADD    L"); break;
        case 0x86: printf("ADD    M"); break;
        case 0x87: printf("ADD    A"); break;
        case 0x88: printf("ADC    B"); break;
        case 0x89: printf("ADC    C"); break;
        case 0x8a: printf("ADC    D"); break;
        case 0x8b: printf("ADC    E"); break;
        case 0x8c: printf("ADC    H"); break;
        case 0x8d: printf("ADC    L"); break;
        case 0x8e: printf("ADC    M"); break;
        case 0x8f: printf("ADC    A"); break;

        case 0x90: printf("SUB    B"); break;
        case 0x91: printf("SUB    C"); break;
        case 0x92: printf("SUB    D"); break;
        case 0x93: printf("SUB    E"); break;
        case 0x94: printf("SUB    H"); break;
        case 0x95: printf("SUB    L"); break;
        case 0x96: printf("SUB    M"); break;
        case 0x97: printf("SUB    A"); break;
        case 0x98: printf("SBB    B"); break;
        case 0x99: printf("SBB    C"); break;
        case 0x9a: printf("SBB    D"); break;
        case 0x9b: printf("SBB    E"); break;
        case 0x9c: printf("SBB    H"); break;
        case 0x9d: printf("SBB    L"); break;
        case 0x9e: printf("SBB    M"); break;
        case 0x9f: printf("SBB    A"); break;

        case 0xa0: printf("ANA    B"); break;
        case 0xa1: printf("ANA    C"); break;
        case 0xa2: printf("ANA    D"); break;
        case 0xa3: printf("ANA    E"); break;
        case 0xa4: printf("ANA    H"); break;
        case 0xa5: printf("ANA    L"); break;
        case 0xa6: printf("ANA    M"); break;
        case 0xa7: printf("ANA    A"); break;
        case 0xa8: printf("XRA    B"); break;
        case 0xa9: printf("XRA    C"); break;
        case 0xaa: printf("XRA    D"); break;
        case 0xab: printf("XRA    E"); break;
        case 0xac: printf("XRA    H"); break;
        case 0xad: printf("XRA    L"); break;
        case 0xae: printf("XRA    M"); break;
        case 0xaf: printf("XRA    A"); break;

        case 0xb0: printf("ORA    B"); break;
        case 0xb1: printf("ORA    C"); break;
        case 0xb2: printf("ORA    D"); break;
        case 0xb3: printf("ORA    E"); break;
        case 0xb4: printf("ORA    H"); break;
        case 0xb5: printf("ORA    L"); break;
        case 0xb6: printf("ORA    M"); break;
        case 0xb7: printf("ORA    A"); break;
        case 0xb8: printf("CMP    B"); break;
        case 0xb9: printf("CMP    C"); break;
        case 0xba: printf("CMP    D"); break;
        case 0xbb: printf("CMP    E"); break;
        case 0xbc: printf("CMP    H"); break;
        case 0xbd: printf("CMP    L"); break;
        case 0xbe: printf("CMP    M"); break;
        case 0xbf: printf("CMP    A"); break;

        case 0xc0: printf("RNZ"); break;
        case 0xc1: printf("POP    B"); break;
        case 0xc2: printf("JNZ    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xc3: printf("JMP    $%02x%02x",code[2],code[1]); op_bytes=3; break;
        case 0xc4: printf("CNZ    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xc5: printf("PUSH   B"); break;
        case 0xc6: printf("ADI    D,#$%02x", code[1]); op_bytes=2; break;
        case 0xc7: printf("RST    0"); break;
        case 0xc8: printf("RZ"); break;
        case 0xc9: printf("RET"); break;
        case 0xca: printf("JZ     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xcb: printf("NOP"); break;
        case 0xcc: printf("CZ     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xcd: printf("CALL   $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xce: printf("ACI    #$%02x", code[1]); op_bytes=2; break;
        case 0xcf: printf("RST    1"); break;

        case 0xd0: printf("RNC"); break;
        case 0xd1: printf("POP    D"); break;
        case 0xd2: printf("JNC    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xd3: printf("OUT    #$%02x", code[1]); op_bytes=2; break;
        case 0xd4: printf("CNC    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xd5: printf("PUSH   D"); break;
        case 0xd6: printf("SUI    #$%02x", code[1]); op_bytes=2; break;
        case 0xd7: printf("RST    2"); break;
        case 0xd8: printf("RC"); break;
        case 0xd9: printf("NOP"); break;
        case 0xda: printf("JC     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xdb: printf("IN     #$%02x", code[1]); op_bytes=2; break;
        case 0xdc: printf("CC     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xdd: printf("NOP"); break;
        case 0xde: printf("SBI    #$%02x", code[1]); op_bytes=2; break;
        case 0xdf: printf("RST    3"); break;

        case 0xe0: printf("RPO"); break;
        case 0xe1: printf("POP    H"); break;
        case 0xe2: printf("JPO    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xe3: printf("XTHL"); break;
        case 0xe4: printf("CPO    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xe5: printf("PUSH   H"); break;
        case 0xe6: printf("ANI    #$%02x", code[1]); op_bytes=2; break;
        case 0xe7: printf("RST    4"); break;
        case 0xe8: printf("RPE"); break;
        case 0xe9: printf("PCHL"); break;
        case 0xea: printf("JPE    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xeb: printf("XCHG"); break;
        case 0xec: printf("CPE    $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xed: printf("NOP"); break;
        case 0xee: printf("XRI    #$%02x", code[1]); op_bytes=2; break;
        case 0xef: printf("RST    5"); break;

        case 0xf0: printf("RP"); break;
        case 0xf1: printf("POP    PSW"); break;
        case 0xf2: printf("JP     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xf3: printf("DI"); break;
        case 0xf4: printf("CP     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xf5: printf("PUSH   PSW"); break;
        case 0xf6: printf("ORI    #$%02x", code[1]); op_bytes=2; break;
        case 0xf7: printf("RST    6"); break;
        case 0xf8: printf("RM"); break;
        case 0xf9: printf("SPHL"); break;
        case 0xfa: printf("JM     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xfb: printf("EI"); break;
        case 0xfc: printf("CM     $%02x%02x", code[2], code[1]); op_bytes=3; break;
        case 0xfd: printf("NOP"); break;
        case 0xfe: printf("CPI    #$%02x", code[1]); op_bytes=2; break;
        case 0xff: printf("RST    7"); break;
    }

    printf("\n");

    return op_bytes;

}


void unimplemented_instruction(State8080* state)
{
    //pc will have advanced one, so undo that
    printf ("Error: Unimplemented instruction\n");
    state->pc--;
    disassemble_8080_op(state->memory, state->pc);
    exit(1);
}


int emulate_8080_op(State8080* state)
   {
    unsigned char *op_code = &state->memory[state->pc];
    disassemble_8080_op(op_code, state->pc);
    state->pc+=1; 

    switch(*op_code)
    {
        //TODO extract to functions
        case 0x00: break;
        case 0x01:
        {
            state->b = op_code[2];
            state->c = op_code[1];
            state->pc += 2;
        }
        break;
        case 0x02: unimplemented_instruction(state); break;
        case 0x03: unimplemented_instruction(state); break;
        case 0x04: unimplemented_instruction(state); break;
        case 0x05: unimplemented_instruction(state); break;
        case 0x06: 
        {
            state->b = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x07: unimplemented_instruction(state); break;
        case 0x08: break;
        case 0x09: unimplemented_instruction(state); break;
        case 0x0a: unimplemented_instruction(state); break;
        case 0x0b: unimplemented_instruction(state); break;
        case 0x0c: unimplemented_instruction(state); break;
        case 0x0d: unimplemented_instruction(state); break;
        case 0x0e: 
        {
            state->c = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x0f: unimplemented_instruction(state); break;

        case 0x10: break;
        case 0x11:
        {
            state->d = op_code[2];
            state->e = op_code[1];
            state->pc += 2;
        }
        break;
        case 0x12: unimplemented_instruction(state); break;
        case 0x13: unimplemented_instruction(state); break;
        case 0x14: unimplemented_instruction(state); break;
        case 0x15: unimplemented_instruction(state); break;
        case 0x16: 
        {
            state->d = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x17: unimplemented_instruction(state); break;
        case 0x18: break;
        case 0x19: unimplemented_instruction(state); break;
        case 0x1a: unimplemented_instruction(state); break;
        case 0x1b: unimplemented_instruction(state); break;
        case 0x1c: unimplemented_instruction(state); break;
        case 0x1d: unimplemented_instruction(state); break;
        case 0x1e: 
        {
            state->e = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x1f: unimplemented_instruction(state); break;

        case 0x20: unimplemented_instruction(state); break;
        case 0x21:
        {
            state->h = op_code[2];
            state->l = op_code[1];
            state->pc += 2;
        }
        break; 
        case 0x22: unimplemented_instruction(state); break;
        case 0x23: unimplemented_instruction(state); break;
        case 0x24: unimplemented_instruction(state); break;
        case 0x25: unimplemented_instruction(state); break;
        case 0x26: 
        {
            state->h = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x27: unimplemented_instruction(state); break;
        case 0x28: break;
        case 0x29: unimplemented_instruction(state); break;
        case 0x2a: unimplemented_instruction(state); break;
        case 0x2b: unimplemented_instruction(state); break;
        case 0x2c: unimplemented_instruction(state); break;
        case 0x2d: unimplemented_instruction(state); break;
        case 0x2e: 
        {
            state->l = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x2f: unimplemented_instruction(state); break;

        case 0x30: unimplemented_instruction(state); break;
        case 0x31: unimplemented_instruction(state); break;
        case 0x32: unimplemented_instruction(state); break;
        case 0x33: unimplemented_instruction(state); break;
        case 0x34: unimplemented_instruction(state); break;
        case 0x35: unimplemented_instruction(state); break;
        case 0x36: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x37: unimplemented_instruction(state); break;
        case 0x38: break;
        case 0x39: unimplemented_instruction(state); break;
        case 0x3a: unimplemented_instruction(state); break;
        case 0x3b: unimplemented_instruction(state); break;
        case 0x3c: unimplemented_instruction(state); break;
        case 0x3d: unimplemented_instruction(state); break;
        case 0x3e: 
        {
            state->a = op_code[1];
            state->pc+=1;
        } 
        break;
        case 0x3f: unimplemented_instruction(state); break;

        case 0x40: state->b = state->b; break;
        case 0x41: state->b = state->c; break;
        case 0x42: state->b = state->d; break;
        case 0x43: state->b = state->e; break;
        case 0x44: state->b = state->h; break;
        case 0x45: state->b = state->l; break;
        case 0x46: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->b = state->memory[offset];
            
        }
        break;
        case 0x47: state->b = state->a; break;
        case 0x48: state->c = state->b; break;
        case 0x49: state->c = state->c; break;
        case 0x4a: state->c = state->d; break;
        case 0x4b: state->c = state->e; break;
        case 0x4c: state->c = state->h; break;
        case 0x4d: state->c = state->l; break;
        case 0x4e: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->c = state->memory[offset];
            
        }
        break;
        case 0x4f: state->c = state->a; break;

        case 0x50: state->d = state->b; break;
        case 0x51: state->d = state->c; break;
        case 0x52: state->d = state->d; break;
        case 0x53: state->d = state->e; break;
        case 0x54: state->d = state->h; break;
        case 0x55: state->d = state->l; break;
        case 0x56: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->d = state->memory[offset];
            
        }
        break;
        case 0x57: state->d = state->a; break;
        case 0x58: state->e = state->b; break;
        case 0x59: state->e = state->c; break;
        case 0x5a: state->e = state->d; break;
        case 0x5b: state->e = state->e; break;
        case 0x5c: state->e = state->h; break;
        case 0x5d: state->e = state->l; break;
        case 0x5e: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->e = state->memory[offset];
            
        }
        break;
        case 0x5f: state->e = state->a; break;

        case 0x60: state->h = state->b; break;
        case 0x61: state->h = state->c; break;
        case 0x62: state->h = state->d; break;
        case 0x63: state->h = state->e; break;
        case 0x64: state->h = state->h; break;
        case 0x65: state->h = state->l; break;
        case 0x66: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->h = state->memory[offset];
            
        }
        break;
        case 0x67: state->h = state->a; break;
        case 0x68: state->l = state->b; break;
        case 0x69: state->l = state->c; break;
        case 0x6a: state->l = state->d; break;
        case 0x6b: state->l = state->e; break;
        case 0x6c: state->l = state->h; break;
        case 0x6d: state->l = state->l; break;
        case 0x6e: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->l = state->memory[offset];
            
        }
        break;
        case 0x6f: state->l = state->a; break;

        case 0x70: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->b;     
        }
        break;
        case 0x71: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->c;     
        }
        break;
        case 0x72: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->d;     
        }
        break;
        case 0x73: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->e;     
        }
        break;
        case 0x74: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->h;     
        }
        break;
        case 0x75: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->l;     
        }
        break;
        case 0x76: unimplemented_instruction(state); break;
        case 0x77: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->memory[offset] = state->a;     
        }
        break;
        case 0x78: state->a = state->b; break;
        case 0x79: state->a = state->c; break;
        case 0x7a: state->a = state->d; break;
        case 0x7b: state->a = state->e; break;
        case 0x7c: state->a = state->h; break;
        case 0x7d: state->a = state->l; break;
        case 0x7e: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            state->a = state->memory[offset];
            
        }
        break;
        case 0x7f: state->a = state->a; break;

        case 0x80:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->b;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            } break;
        case 0x81:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->c;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x82:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->d;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x83:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->e;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x84:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->h;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x85:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->l;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x86: 
            {
            uint16_t offset =  (state->h<<8) + (state->l);
            uint16_t answer = (uint16_t) state-> a + state->memory[offset];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x87:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->a;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            }
            break;
        case 0x88: unimplemented_instruction(state); break;
        case 0x89: unimplemented_instruction(state); break;
        case 0x8a: unimplemented_instruction(state); break;
        case 0x8b: unimplemented_instruction(state); break;
        case 0x8c: unimplemented_instruction(state); break;
        case 0x8d: unimplemented_instruction(state); break;
        case 0x8e: unimplemented_instruction(state); break;
        case 0x8f: unimplemented_instruction(state); break;

        case 0x90: unimplemented_instruction(state); break;
        case 0x91: unimplemented_instruction(state); break;
        case 0x92: unimplemented_instruction(state); break;
        case 0x93: unimplemented_instruction(state); break;
        case 0x94: unimplemented_instruction(state); break;
        case 0x95: unimplemented_instruction(state); break;
        case 0x96: unimplemented_instruction(state); break;
        case 0x97: unimplemented_instruction(state); break;
        case 0x98: unimplemented_instruction(state); break;
        case 0x99: unimplemented_instruction(state); break;
        case 0x9a: unimplemented_instruction(state); break;
        case 0x9b: unimplemented_instruction(state); break;
        case 0x9c: unimplemented_instruction(state); break;
        case 0x9d: unimplemented_instruction(state); break;
        case 0x9e: unimplemented_instruction(state); break;
        case 0x9f: unimplemented_instruction(state); break;

        case 0xa0: unimplemented_instruction(state); break;
        case 0xa1: unimplemented_instruction(state); break;
        case 0xa2: unimplemented_instruction(state); break;
        case 0xa3: unimplemented_instruction(state); break;
        case 0xa4: unimplemented_instruction(state); break;
        case 0xa5: unimplemented_instruction(state); break;
        case 0xa6: unimplemented_instruction(state); break;
        case 0xa7: unimplemented_instruction(state); break;
        case 0xa8: unimplemented_instruction(state); break;
        case 0xa9: unimplemented_instruction(state); break;
        case 0xaa: unimplemented_instruction(state); break;
        case 0xab: unimplemented_instruction(state); break;
        case 0xac: unimplemented_instruction(state); break;
        case 0xad: unimplemented_instruction(state); break;
        case 0xae: unimplemented_instruction(state); break;
        case 0xaf: unimplemented_instruction(state); break;

        case 0xb0: unimplemented_instruction(state); break;
        case 0xb1: unimplemented_instruction(state); break;
        case 0xb2: unimplemented_instruction(state); break;
        case 0xb3: unimplemented_instruction(state); break;
        case 0xb4: unimplemented_instruction(state); break;
        case 0xb5: unimplemented_instruction(state); break;
        case 0xb6: unimplemented_instruction(state); break;
        case 0xb7: unimplemented_instruction(state); break;
        case 0xb8: unimplemented_instruction(state); break;
        case 0xb9: unimplemented_instruction(state); break;
        case 0xba: unimplemented_instruction(state); break;
        case 0xbb: unimplemented_instruction(state); break;
        case 0xbc: unimplemented_instruction(state); break;
        case 0xbd: unimplemented_instruction(state); break;
        case 0xbe: unimplemented_instruction(state); break;
        case 0xbf: unimplemented_instruction(state); break;

        case 0xc0: unimplemented_instruction(state); break;
        case 0xc1: unimplemented_instruction(state); break;
        case 0xc2: unimplemented_instruction(state); break;
        case 0xc3: unimplemented_instruction(state); break;
        case 0xc4: unimplemented_instruction(state); break;
        case 0xc5: unimplemented_instruction(state); break;
        case 0xc6:
            {
            uint16_t answer = (uint16_t) state->a + (uint16_t) op_code[1];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
            state->pc+=1;
            }
            break;
        case 0xc7: unimplemented_instruction(state); break;
        case 0xc8: unimplemented_instruction(state); break;
        case 0xc9: unimplemented_instruction(state); break;
        case 0xca: unimplemented_instruction(state); break;
        case 0xcb: break;
        case 0xcc: unimplemented_instruction(state); break;
        case 0xcd: unimplemented_instruction(state); break;
        case 0xce: unimplemented_instruction(state); break;
        case 0xcf: unimplemented_instruction(state); break;

        case 0xd0: unimplemented_instruction(state); break;
        case 0xd1: unimplemented_instruction(state); break;
        case 0xd2: unimplemented_instruction(state); break;
        case 0xd3: unimplemented_instruction(state); break;
        case 0xd4: unimplemented_instruction(state); break;
        case 0xd5: unimplemented_instruction(state); break;
        case 0xd6: unimplemented_instruction(state); break;
        case 0xd7: unimplemented_instruction(state); break;
        case 0xd8: unimplemented_instruction(state); break;
        case 0xd9: break;
        case 0xda: unimplemented_instruction(state); break;
        case 0xdb: unimplemented_instruction(state); break;
        case 0xdc: unimplemented_instruction(state); break;
        case 0xdd: break;
        case 0xde: unimplemented_instruction(state); break;
        case 0xdf: unimplemented_instruction(state); break;

        case 0xe0: unimplemented_instruction(state); break;
        case 0xe1: unimplemented_instruction(state); break;
        case 0xe2: unimplemented_instruction(state); break;
        case 0xe3: unimplemented_instruction(state); break;
        case 0xe4: unimplemented_instruction(state); break;
        case 0xe5: unimplemented_instruction(state); break;
        case 0xe6: unimplemented_instruction(state); break;
        case 0xe7: unimplemented_instruction(state); break;
        case 0xe8: unimplemented_instruction(state); break;
        case 0xe9: unimplemented_instruction(state); break;
        case 0xea: unimplemented_instruction(state); break;
        case 0xeb: unimplemented_instruction(state); break;
        case 0xec: unimplemented_instruction(state); break;
        case 0xed: break;
        case 0xee: unimplemented_instruction(state); break;
        case 0xef: unimplemented_instruction(state); break;

        case 0xf0: unimplemented_instruction(state); break;
        case 0xf1: unimplemented_instruction(state); break;
        case 0xf2: unimplemented_instruction(state); break;
        case 0xf3: unimplemented_instruction(state); break;
        case 0xf4: unimplemented_instruction(state); break;
        case 0xf5: unimplemented_instruction(state); break;
        case 0xf6: unimplemented_instruction(state); break;
        case 0xf7: unimplemented_instruction(state); break;
        case 0xf8: unimplemented_instruction(state); break;
        case 0xf9: unimplemented_instruction(state); break;
        case 0xfa: unimplemented_instruction(state); break;
        case 0xfb: unimplemented_instruction(state); break;
        case 0xfc: unimplemented_instruction(state); break;
        case 0xfd: break;
        case 0xfe: unimplemented_instruction(state); break;
        case 0xff: unimplemented_instruction(state); break;
    }
    
    return 0;
}

int parity(int x, int size)
{
	int i;
	int p = 0;
	x = (x & ((1<<size)-1));
	for (i=0; i<size; i++)
	{
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}
