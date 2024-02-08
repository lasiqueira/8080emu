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

typedef struct Shifter{
    uint8_t shift0;
    uint8_t shift1;
    uint8_t offset;
} Shifter;

//EMU
int disassemble_8080_op(unsigned char *code_buffer, int pos);
void unimplemented_instruction(State8080* state);
int emulate_8080_op(State8080* state);
int parity(int x, int size);
//STATE
State8080 initialize_state();
void read_to_memory(State8080 *state, char *file_name);
uint8_t* get_m(State8080 *state);
void print_state(State8080 *state);
//OPCODE FUNS
void add(State8080 *state, uint8_t *reg);
void mov(uint8_t *lhv, uint8_t *rhv);
void mvi(uint8_t *lhv, unsigned char *op_code, uint16_t *pc);
void hlt();
void rrc(State8080 *state);
void jmp(uint16_t *pc, unsigned char *op_code);
void cma(uint8_t *reg_a);
void jnz(State8080 *state, unsigned char *op_code);
void adi(State8080 *state, unsigned char *op_code);
void ret(State8080 *state);
void call(State8080 *state, unsigned char *op_code);
void rar(State8080 *state);
void ani(State8080 *state, unsigned char *op_code);
void cpi(State8080 *state, unsigned char *op_code);
void xchg(State8080 *state);
void out(State8080 *state, unsigned char *op_code);
void ei(State8080 *state);
void di(State8080 *state);
void in(State8080 *state, unsigned char *op_code);

//SPACE INVADERS
void hw_in(State8080 *state, Shifter *shifter, unsigned char *op_code);
void hw_out(State8080 *state, Shifter *shifter, unsigned char *op_code);

int main(int argc, char**argv)
{
    int done = 0;
    State8080 state = initialize_state();
    Shifter shifter;
    read_to_memory(&state, argv[1]); 
    print_state(&state);   
    
    while(done == 0)
    {
        //SPACE INVADERS I/O handling
        uint8_t *op_code= &state.memory[state.pc];
        if(*op_code == 0xdb)
        {
            hw_in(&state, &shifter, op_code);
        } 
        else if(*op_code == 0xd3)
        {
            hw_out(&state, &shifter, op_code);
        } 
        else
        {
            done= emulate_8080_op(&state);
        }
        
    }

    return 0;
}

//EMU
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

void print_state(State8080 *state)
{
    printf("\t");
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("%c  ", state->cc.ac ? 'a' : '.');
	printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", state->a, state->b, state->c,
				state->d, state->e, state->h, state->l, state->sp);
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
    disassemble_8080_op(state->memory, state->pc);
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
        case 0x05: 
        {
			uint8_t sum = state->b - 1;
			state->cc.z = (sum == 0);
			state->cc.s = (0x80 == (sum & 0x80));
			state->cc.p = parity(sum, 8);
			state->b = sum;
		} 
        break;
        case 0x06: mvi(&state->b, op_code, &state->pc); break;
        case 0x07: unimplemented_instruction(state); break;
        case 0x08: break;
        case 0x09: 
        {
            uint32_t bc = (state->b << 8) | state->c;
			uint32_t res = bc + bc;
			state->h = (res & 0xff00) >> 8;
			state->l = res & 0xff;
			state->cc.cy = ((res & 0xffff0000) != 0);
        } 
        break;
        case 0x0a: unimplemented_instruction(state); break;
        case 0x0b: unimplemented_instruction(state); break;
        case 0x0c: unimplemented_instruction(state); break;
        case 0x0d:  
        {
            uint8_t sum = state->c - 1;
			state->cc.z = (sum == 0);
			state->cc.s = (0x80 == (sum & 0x80));
			state->cc.p = parity(sum, 8);
			state->c = sum;
        }
        break;
        case 0x0e: mvi(&state->c, op_code, &state->pc); break;
        case 0x0f: rrc(state); break;

        case 0x10: break;
        case 0x11:
        {
            state->d = op_code[2];
            state->e = op_code[1];
            state->pc += 2;
        }
        break;
        case 0x12: unimplemented_instruction(state); break;
        case 0x13: 
        {
            state->e++;
            if(state->e == 0)
                state->d++;
        }
        break;
        case 0x14: unimplemented_instruction(state); break;
        case 0x15: unimplemented_instruction(state); break;
        case 0x16: mvi(&state->d, op_code, &state->pc);break;
        case 0x17: unimplemented_instruction(state); break;
        case 0x18: break;
        case 0x19: 
        {
            uint32_t de = (state->d << 8) | state->e;
			uint32_t res = de + de;
			state->h = (res & 0xff00) >> 8;
			state->l = res & 0xff;
			state->cc.cy = ((res & 0xffff0000) != 0);
        } 
        break;
        case 0x1a:
        {
            uint16_t offset = (state->d << 8) | state->e;
            state->a = state->memory[offset];
        }
        break;
        case 0x1b: unimplemented_instruction(state); break;
        case 0x1c: unimplemented_instruction(state); break;
        case 0x1d: unimplemented_instruction(state); break;
        case 0x1e: mvi(&state->e, op_code, &state->pc); break;
        case 0x1f: rar(state); break;
        case 0x20: unimplemented_instruction(state); break;
        case 0x21:
        {
            state->h = op_code[2];
            state->l = op_code[1];
            state->pc += 2;
        }
        break; 
        case 0x22: unimplemented_instruction(state); break;
        case 0x23: 
        {
            state->l++;
            if(state->l == 0)
                state->h++;     
        }
        break;
        case 0x24: unimplemented_instruction(state); break;
        case 0x25: unimplemented_instruction(state); break;
        case 0x26: mvi(&state->h, op_code, &state->pc); break;
        case 0x27: unimplemented_instruction(state); break;
        case 0x28: break;
        case 0x29: 
        {
            uint32_t hl = (state->h << 8) | state->l;
			uint32_t res = hl + hl;
			state->h = (res & 0xff00) >> 8;
			state->l = res & 0xff;
			state->cc.cy = ((res & 0xffff0000) != 0);
        }
        break;
        case 0x2a: unimplemented_instruction(state); break;
        case 0x2b: unimplemented_instruction(state); break;
        case 0x2c: unimplemented_instruction(state); break;
        case 0x2d: unimplemented_instruction(state); break;
        case 0x2e: mvi(&state->l, op_code, &state->pc); break;
        case 0x2f: cma(&state->a); break;
        case 0x30: unimplemented_instruction(state); break;
        case 0x31: 
        {
            uint16_t pointer = (op_code[2]<<8) | op_code[1];
            state->sp = pointer;
            state->pc += 2;
        }
        break;
        case 0x32: 
        {    
            uint16_t offset = (op_code[2]<<8) | (op_code[1]);
			state->memory[offset] = state->a;
			state->pc+=2;
        }
        break;
        case 0x33: unimplemented_instruction(state); break;
        case 0x34: unimplemented_instruction(state); break;
        case 0x35: unimplemented_instruction(state); break;
        case 0x36: mvi(get_m(state), op_code, &state->pc);break;
        case 0x37: unimplemented_instruction(state); break;
        case 0x38: break;
        case 0x39: unimplemented_instruction(state); break;
        case 0x3a: 
        {
			uint16_t offset = (op_code[2]<<8) | (op_code[1]);
			state->a = state->memory[offset];
			state->pc+=2;
		}
        break;
        case 0x3b: unimplemented_instruction(state); break;
        case 0x3c: unimplemented_instruction(state); break;
        case 0x3d: unimplemented_instruction(state); break;
        case 0x3e: mvi(&state->a, op_code, &state->pc); break;
        case 0x3f: unimplemented_instruction(state); break;

        case 0x40: mov(&state->b, &state->b); break;
        case 0x41: mov(&state->b, &state->c); break;
        case 0x42: mov(&state->b, &state->d); break;
        case 0x43: mov(&state->b, &state->e); break;
        case 0x44: mov(&state->b, &state->h); break;
        case 0x45: mov(&state->b, &state->l); break;
        case 0x46: mov(&state->b, get_m(state)); break;
        case 0x47: mov(&state->b, &state->a); break;
        case 0x48: mov(&state->c, &state->b); break;
        case 0x49: mov(&state->c, &state->c); break;
        case 0x4a: mov(&state->c, &state->d); break;
        case 0x4b: mov(&state->c, &state->e); break;
        case 0x4c: mov(&state->c, &state->h); break;
        case 0x4d: mov(&state->c, &state->l); break;
        case 0x4e: mov(&state->c, get_m(state)); break;
        case 0x4f: mov(&state->c, &state->a); break;

        case 0x50: mov(&state->d, &state->b); break;
        case 0x51: mov(&state->d, &state->c); break;
        case 0x52: mov(&state->d, &state->d); break;
        case 0x53: mov(&state->d, &state->e); break;
        case 0x54: mov(&state->d, &state->h); break;
        case 0x55: mov(&state->d, &state->l); break;
        case 0x56: mov(&state->d, get_m(state)); break;
        case 0x57: mov(&state->d, &state->a); break;
        case 0x58: mov(&state->e, &state->b); break;
        case 0x59: mov(&state->e, &state->c); break;
        case 0x5a: mov(&state->e, &state->d); break;
        case 0x5b: mov(&state->e, &state->e); break;
        case 0x5c: mov(&state->e, &state->h); break;
        case 0x5d: mov(&state->e, &state->l); break;
        case 0x5e: mov(&state->e, get_m(state));break;
        case 0x5f: mov(&state->e, &state->a); break;

        case 0x60: mov(&state->h, &state->b); break;
        case 0x61: mov(&state->h, &state->c); break;
        case 0x62: mov(&state->h, &state->d); break;
        case 0x63: mov(&state->h, &state->e); break;
        case 0x64: mov(&state->h, &state->h); break;
        case 0x65: mov(&state->h, &state->l); break;
        case 0x66: mov(&state->h, get_m(state));break;
        case 0x67: mov(&state->h, &state->a); break;
        case 0x68: mov(&state->l, &state->b); break;
        case 0x69: mov(&state->l, &state->c); break;
        case 0x6a: mov(&state->l, &state->d); break;
        case 0x6b: mov(&state->l, &state->e); break;
        case 0x6c: mov(&state->l, &state->h); break;
        case 0x6d: mov(&state->l, &state->l); break;
        case 0x6e: mov(&state->l, get_m(state)); break;
        case 0x6f: mov(&state->l, &state->a); break;

        case 0x70: mov(get_m(state), &state->b); break;
        case 0x71: mov(get_m(state), &state->c); break;
        case 0x72: mov(get_m(state), &state->d); break;
        case 0x73: mov(get_m(state), &state->e); break;
        case 0x74: mov(get_m(state), &state->h); break;
        case 0x75: mov(get_m(state), &state->l); break;
        case 0x76: hlt(); break;
        case 0x77: mov(get_m(state), &state->a); break;
        case 0x78: mov(&state->a, &state->b); break;
        case 0x79: mov(&state->a, &state->c); break;
        case 0x7a: mov(&state->a, &state->d); break;
        case 0x7b: mov(&state->a, &state->e); break;
        case 0x7c: mov(&state->a, &state->h); break;
        case 0x7d: mov(&state->a, &state->l); break;
        case 0x7e: mov(&state->a, get_m(state)); break;
        case 0x7f: mov(&state->a, &state->a); break;

        case 0x80: add(state, &state->b); break;
        case 0x81: add(state, &state->c); break;
        case 0x82: add(state, &state->d); break;
        case 0x83: add(state, &state->e); break;
        case 0x84: add(state, &state->h); break;
        case 0x85: add(state, &state->l); break;
        case 0x86: add(state, get_m(state)); break;
        case 0x87: add(state, &state->a); break;
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
        case 0xa7: 
        {
            state->a = state->a&state->a;
            state->cc.z = (state->a == 0);
            state->cc.s = (0x80 == (state->a & 0x80));
            state->cc.cy = state->cc.ac = 0;
            state->cc.p = parity(state->a, 8);
        }
        break;
        case 0xa8: unimplemented_instruction(state); break;
        case 0xa9: unimplemented_instruction(state); break;
        case 0xaa: unimplemented_instruction(state); break;
        case 0xab: unimplemented_instruction(state); break;
        case 0xac: unimplemented_instruction(state); break;
        case 0xad: unimplemented_instruction(state); break;
        case 0xae: unimplemented_instruction(state); break;
        case 0xaf: 
        {
            state->a = state->a^state->a;
            state->cc.z = (state->a == 0);
            state->cc.s = (0x80 == (state->a & 0x80));
            state->cc.cy = state->cc.ac = 0;
            state->cc.p = parity(state->a, 8);
        }
        break;

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
        case 0xc1: 
        {    
            state->c = state->memory[state->sp];    
            state->b = state->memory[state->sp+1];    
            state->sp += 2;    
        } 
        break;
        case 0xc2: jnz(state, op_code); break;   
        case 0xc3: jmp(&state->pc, op_code); break;
        case 0xc4: unimplemented_instruction(state); break;
        case 0xc5: 
        {
            state->memory[state->sp-1] = state->b;    
            state->memory[state->sp-2] = state->c;    
            state->sp = state->sp - 2;    
        }    
        break;
        case 0xc6: adi(state, op_code); break;
        case 0xc7: unimplemented_instruction(state); break;
        case 0xc8: unimplemented_instruction(state); break;
        case 0xc9: ret(state); break;
        case 0xca: unimplemented_instruction(state); break;
        case 0xcb: break;
        case 0xcc: unimplemented_instruction(state); break;
        case 0xcd: call(state, op_code); break;
        case 0xce: unimplemented_instruction(state); break;
        case 0xcf: unimplemented_instruction(state); break;

        case 0xd0: unimplemented_instruction(state); break;
        case 0xd1: 
        {
            state->d = state->memory[state->sp];
            state->e = state->memory[state->sp+1];
            state->sp += 2;
        }
        break;
        case 0xd2: unimplemented_instruction(state); break;
        case 0xd3: out(state, op_code); break;
        case 0xd4: unimplemented_instruction(state); break;
        case 0xd5: 
        {
            state->memory[state->sp-1] = state->d;
            state->memory[state->sp-2] = state->e;
            state->sp -=2;
        }
        break;
        case 0xd6: unimplemented_instruction(state); break;
        case 0xd7: unimplemented_instruction(state); break;
        case 0xd8: unimplemented_instruction(state); break;
        case 0xd9: break;
        case 0xda: unimplemented_instruction(state); break;
        case 0xdb: in(state, op_code); break;
        case 0xdc: unimplemented_instruction(state); break;
        case 0xdd: break;
        case 0xde: unimplemented_instruction(state); break;
        case 0xdf: unimplemented_instruction(state); break;

        case 0xe0: unimplemented_instruction(state); break;
        case 0xe1: 
        {
            state->h = state->memory[state->sp];
            state->l = state->memory[state->sp+1];
            state->sp+=2;
        }
        break;
        case 0xe2: unimplemented_instruction(state); break;
        case 0xe3: unimplemented_instruction(state); break;
        case 0xe4: unimplemented_instruction(state); break;
        case 0xe5: 
        {
            state->memory[state->sp -1] = state->h;
            state->memory[state->sp -2] = state->l;
            state->sp -=2;
        }
        break;
        case 0xe6: ani(state, op_code); break;
        case 0xe7: unimplemented_instruction(state); break;
        case 0xe8: unimplemented_instruction(state); break;
        case 0xe9: unimplemented_instruction(state); break;
        case 0xea: unimplemented_instruction(state); break;
        case 0xeb: xchg(state); break;
        case 0xec: unimplemented_instruction(state); break;
        case 0xed: break;
        case 0xee: unimplemented_instruction(state); break;
        case 0xef: unimplemented_instruction(state); break;

        case 0xf0: unimplemented_instruction(state); break;
        case 0xf1: 
        {    
            state->a = state->memory[state->sp+1];    
            uint8_t psw = state->memory[state->sp];    
            state->cc.z  = (0x01 == (psw & 0x01));    
            state->cc.s  = (0x02 == (psw & 0x02));    
            state->cc.p  = (0x04 == (psw & 0x04));    
            state->cc.cy = (0x05 == (psw & 0x08));    
            state->cc.ac = (0x10 == (psw & 0x10));    
            state->sp += 2;    
        }    
        break;
        case 0xf2: unimplemented_instruction(state); break;
        case 0xf3: di(state); break;
        case 0xf4: unimplemented_instruction(state); break;
        case 0xf5: 
        {    
            state->memory[state->sp-1] = state->a;    
            uint8_t psw = (state->cc.z |    
                            state->cc.s << 1 |    
                            state->cc.p << 2 |    
                            state->cc.cy << 3 |    
                            state->cc.ac << 4 );    
            state->memory[state->sp-2] = psw;    
            state->sp = state->sp - 2;    
        } 
        break;
        case 0xf6: unimplemented_instruction(state); break;
        case 0xf7: unimplemented_instruction(state); break;
        case 0xf8: unimplemented_instruction(state); break;
        case 0xf9: unimplemented_instruction(state); break;
        case 0xfa: unimplemented_instruction(state); break;
        case 0xfb: ei(state); break;
        case 0xfc: unimplemented_instruction(state); break;
        case 0xfd: break;
        case 0xfe: cpi(state, op_code); break;
        case 0xff: unimplemented_instruction(state); break;
    }
    print_state(state);
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
//STATE
State8080 initialize_state()
{
    State8080 state;
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

//OPCODE FUN

void mov(uint8_t *lhv, uint8_t *rhv)
{
    *lhv = *rhv;
}

void add(State8080 *state, uint8_t *reg)
{
    uint16_t sum = (uint16_t) state->a + (uint16_t) *reg;
    state->cc.z = ((sum & 0xff) == 0);
    state->cc.s = ((sum & 0x80) != 0);
    state->cc.cy = (sum > 0xff);
    state->cc.p = parity(sum&0xff, 8);
    state->a = sum & 0xff;
}

void mvi(uint8_t *lhv, unsigned char *op_code, uint16_t *pc)
{
    *lhv = op_code[1];
    *pc += 1;
}

void hlt()
{
    exit(0);
}

void rrc(State8080 *state)
{
    uint8_t val = state->a;    
    state->a = ((val & 1) << 7) | (val >> 1);    
    state->cc.cy = (1 == (val&1));    
}

void jmp(uint16_t *pc, unsigned char *op_code)
{
    *pc = (op_code[2]<<8) | (op_code[1]);
}

void cma(uint8_t *reg_a)
{
    *reg_a = ~*reg_a;
}

void jnz(State8080 *state, unsigned char *op_code)
{
    if (0 == state->cc.z)    
        state->pc = (op_code[2] << 8) | op_code[1];    
    else    
        // branch not taken    
        state->pc += 2; 
}

void adi(State8080 *state, unsigned char *op_code)
{
    uint16_t sum = (uint16_t) state->a + (uint16_t) op_code[1];
    state->cc.z = ((sum & 0xff) == 0);
    state->cc.s = ((sum & 0x80) != 0);
    state->cc.cy = (sum > 0xff);
    state->cc.p = parity(sum&0xff, 8);
    state->a = sum & 0xff;
    state->pc+=1;
}

void ret(State8080 *state)
{
    state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);    
    state->sp += 2;
}

void call (State8080 *state, unsigned char *op_code)
{
    uint16_t counter = state->pc+2;    
    state->memory[state->sp-1] = (counter >> 8) & 0xff;    
    state->memory[state->sp-2] = (counter & 0xff);    
    state->sp = state->sp - 2;    
    state->pc = (op_code[2] << 8) | op_code [1];  
}

void rar(State8080 *state)
{
    uint8_t x = state->a;    
    state->a = (state->cc.cy << 7) | (x >> 1);    
    state->cc.cy = (1 == (x&1));  
}

void ani(State8080 *state, unsigned char *op_code)
{
    uint8_t x = state->a & op_code[1];    
    state->cc.z = (x == 0);    
    state->cc.s = (0x80 == (x & 0x80));    
    state->cc.p = parity(x, 8);    
    state->cc.cy = 0;           //Data book says ANI clears CY    
    state->a = x;    
    state->pc++;                //for the data byte  
}

void cpi(State8080 *state, unsigned char *op_code)
{
    uint8_t x = state->a - op_code[1];    
    state->cc.z = (x == 0);    
    state->cc.s = (0x80 == (x & 0x80));    
    //It isn't clear in the data book what to do with p - had to pick    
    state->cc.p = parity(x, 8);    
    state->cc.cy = (state->a < op_code[1]);    
    state->pc++;   
}

void xchg(State8080 *state)
{
    uint8_t d = state->d;
    uint8_t e = state->e;
    state->d = state->h; 
    state->h = d;
    state->e = state->l;
    state->l = e;
}

void out(State8080 *state, unsigned char *op_code)
{
    //unsure about this
    //state->memory[op_code[1]] = state->a;
    state->pc +=1;
}

void ei(State8080 *state)
{
    state->int_enable = 1;
}

void di(State8080 *state)
{
    state->int_enable = 0;
}
void in(State8080 *state, unsigned char *op_code)
{
    //unsure about this
    //state->a = state->memory[op_code[1]];
    state->pc +=1;
}


//SPACE INVADERS
void hw_in(State8080 *state, Shifter *shifter, unsigned char *op_code)
{
    switch(op_code[1])
    {
        case 0x00: break; //TODO
        case 0x01: break; //TODO
        case 0x02: break; //TODO
        case 0x03: 
        uint16_t v = (shifter->shift1<<8) | shifter->shift0;
        state->a = ((v >> (8-shifter->offset)) && 0xff); 
        break;
    }
    state->pc+=1;
}

void hw_out(State8080 *state, Shifter *shifter, unsigned char *op_code)
{
    switch(op_code[1])
    {    
        case 0x02: 
        shifter-> offset = state->a & 0x7; //bits 012
        break;
        case 0x03: break; //TODO
        case 0x04: 
        shifter->shift0 = shifter->shift1;
        shifter->shift1 = state->a;
        break;
        case 0x05: break; //TODO
        case 0x06: break; //TODO
    }
    state->pc+=1;
}
