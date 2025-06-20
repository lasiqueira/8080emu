#include <stdio.h>
#include "emulation.h"
//EMU


int gInstructionCount = 0;
int disassemble_8080_op(unsigned char *code_buffer, int pc)
{
    unsigned char *code = &code_buffer[pc];
    int op_bytes = 1;
    printf("iCount: %d ", ++gInstructionCount);
    printf("pc: %04x ", pc);
    printf("opcode: %02x ", *code);
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
    unsigned char *op_code = &state->memory[(*memory_mapping_read_ptr)(state->pc)];
    #ifndef NDEBUG
    disassemble_8080_op(state->memory, state->pc);
    #endif
    state->pc+=1; 

    switch(*op_code)
    {
        case 0x00: break;
        case 0x01: lxi(state, op_code, &state->b, &state->c); break;
        case 0x02: stax(state, state->b, state->c); break;
        case 0x03: inx(&state->b, &state->c); break;
        case 0x04: inr(state, &state->b); break;
        case 0x05: dcr(state, &state->b); break;
        case 0x06: mvi(&state->b, op_code, &state->pc); break;
        case 0x07: rlc(state); break;
        case 0x08: break;
        case 0x09: dad(state, state->b, state->c); break;
        case 0x0a: ldax(state, state->b, state->c); break;
        case 0x0b: dcx(&state->b, &state->c); break;
        case 0x0c: inr(state, &state->c); break;
        case 0x0d: dcr(state, &state->c); break;
        case 0x0e: mvi(&state->c, op_code, &state->pc); break;
        case 0x0f: rrc(state); break;

        case 0x10: break;
        case 0x11: lxi(state, op_code, &state->d, &state->e); break;
        case 0x12: stax(state, state->d, state->e); break;
        case 0x13: inx(&state->d, &state->e); break;
        case 0x14: inr(state, &state->d); break;
        case 0x15: dcr(state, &state->d); break;
        case 0x16: mvi(&state->d, op_code, &state->pc);break;
        case 0x17: ral(state); break;
        case 0x18: break;
        case 0x19: dad(state, state->d, state->e); break;
        case 0x1a: ldax(state, state->d, state->e); break;
        case 0x1b: dcx(&state->d, &state->e); break;
        case 0x1c: inr(state, &state->e); break;
        case 0x1d: dcr(state, &state->e); break;
        case 0x1e: mvi(&state->e, op_code, &state->pc); break;
        case 0x1f: rar(state); break;
        case 0x20: break;
        case 0x21: lxi(state, op_code, &state->h, &state->l); break;
        case 0x22: shld(state, op_code); break;
        case 0x23: inx(&state->h, &state->l); break;
        case 0x24: inr(state, &state->h); break;
        case 0x25: dcr(state, &state->h); break;
        case 0x26: mvi(&state->h, op_code, &state->pc); break;
        case 0x27: daa(state); break;
        case 0x28: break;
        case 0x29: dad(state, state->h, state->l); break;
        case 0x2a: lhld(state, op_code); break;
        case 0x2b: dcx(&state->h, &state->l); break;
        case 0x2c: inr(state, &state->l); break;
        case 0x2d: dcr(state, &state->l); break;
        case 0x2e: mvi(&state->l, op_code, &state->pc); break;
        case 0x2f: cma(&state->a); break;
        case 0x30: break;
        case 0x31: lxi_sp(state, op_code); break;
        case 0x32: sta(state, op_code); break;
        case 0x33: inx_sp(state); break;
        case 0x34: inr(state, get_m(state)); break;
        case 0x35: dcr(state, get_m(state)); break;
        case 0x36: mvi(get_m(state), op_code, &state->pc);break;
        case 0x37: stc(state); break;
        case 0x38: break;
        case 0x39: dad_sp(state); break;
        case 0x3a: lda(state, op_code); break;
        case 0x3b: dcx_sp(state); break;
        case 0x3c: inr(state, &state->a); break;
        case 0x3d: dcr(state, &state->a); break;
        case 0x3e: mvi(&state->a, op_code, &state->pc); break;
        case 0x3f: cmc(state); break;

        case 0x40: mov(&state->b, state->b); break;
        case 0x41: mov(&state->b, state->c); break;
        case 0x42: mov(&state->b, state->d); break;
        case 0x43: mov(&state->b, state->e); break;
        case 0x44: mov(&state->b, state->h); break;
        case 0x45: mov(&state->b, state->l); break;
        case 0x46: mov(&state->b, *get_m(state)); break;
        case 0x47: mov(&state->b, state->a); break;
        case 0x48: mov(&state->c, state->b); break;
        case 0x49: mov(&state->c, state->c); break;
        case 0x4a: mov(&state->c, state->d); break;
        case 0x4b: mov(&state->c, state->e); break;
        case 0x4c: mov(&state->c, state->h); break;
        case 0x4d: mov(&state->c, state->l); break;
        case 0x4e: mov(&state->c, *get_m(state)); break;
        case 0x4f: mov(&state->c, state->a); break;

        case 0x50: mov(&state->d, state->b); break;
        case 0x51: mov(&state->d, state->c); break;
        case 0x52: mov(&state->d, state->d); break;
        case 0x53: mov(&state->d, state->e); break;
        case 0x54: mov(&state->d, state->h); break;
        case 0x55: mov(&state->d, state->l); break;
        case 0x56: mov(&state->d, *get_m(state)); break;
        case 0x57: mov(&state->d, state->a); break;
        case 0x58: mov(&state->e, state->b); break;
        case 0x59: mov(&state->e, state->c); break;
        case 0x5a: mov(&state->e, state->d); break;
        case 0x5b: mov(&state->e, state->e); break;
        case 0x5c: mov(&state->e, state->h); break;
        case 0x5d: mov(&state->e, state->l); break;
        case 0x5e: mov(&state->e, *get_m(state));break;
        case 0x5f: mov(&state->e, state->a); break;

        case 0x60: mov(&state->h, state->b); break;
        case 0x61: mov(&state->h, state->c); break;
        case 0x62: mov(&state->h, state->d); break;
        case 0x63: mov(&state->h, state->e); break;
        case 0x64: mov(&state->h, state->h); break;
        case 0x65: mov(&state->h, state->l); break;
        case 0x66: mov(&state->h, *get_m(state));break;
        case 0x67: mov(&state->h, state->a); break;
        case 0x68: mov(&state->l, state->b); break;
        case 0x69: mov(&state->l, state->c); break;
        case 0x6a: mov(&state->l, state->d); break;
        case 0x6b: mov(&state->l, state->e); break;
        case 0x6c: mov(&state->l, state->h); break;
        case 0x6d: mov(&state->l, state->l); break;
        case 0x6e: mov(&state->l, *get_m(state)); break;
        case 0x6f: mov(&state->l, state->a); break;

        case 0x70: mov(get_m(state), state->b); break;
        case 0x71: mov(get_m(state), state->c); break;
        case 0x72: mov(get_m(state), state->d); break;
        case 0x73: mov(get_m(state), state->e); break;
        case 0x74: mov(get_m(state), state->h); break;
        case 0x75: mov(get_m(state), state->l); break;
        case 0x76: hlt(state); break;
        case 0x77: mov(get_m(state), state->a); break;
        case 0x78: mov(&state->a, state->b); break;
        case 0x79: mov(&state->a, state->c); break;
        case 0x7a: mov(&state->a, state->d); break;
        case 0x7b: mov(&state->a, state->e); break;
        case 0x7c: mov(&state->a, state->h); break;
        case 0x7d: mov(&state->a, state->l); break;
        case 0x7e: mov(&state->a, *get_m(state)); break;
        case 0x7f: mov(&state->a, state->a); break;

        case 0x80: add(state, state->b); break;
        case 0x81: add(state, state->c); break;
        case 0x82: add(state, state->d); break;
        case 0x83: add(state, state->e); break;
        case 0x84: add(state, state->h); break;
        case 0x85: add(state, state->l); break;
        case 0x86: add(state, *get_m(state)); break;
        case 0x87: add(state, state->a); break;
        case 0x88: adc(state, state->b); break;
        case 0x89: adc(state, state->c); break;
        case 0x8a: adc(state, state->d); break;
        case 0x8b: adc(state, state->e); break;
        case 0x8c: adc(state, state->h); break;
        case 0x8d: adc(state, state->l); break;
        case 0x8e: adc(state, *get_m(state)); break;
        case 0x8f: adc(state, state->a); break;

        case 0x90: sub(state, state->b); break;
        case 0x91: sub(state, state->c); break;
        case 0x92: sub(state, state->d); break;
        case 0x93: sub(state, state->e); break;
        case 0x94: sub(state, state->h); break;
        case 0x95: sub(state, state->l); break;
        case 0x96: sub(state, *get_m(state)); break;
        case 0x97: sub(state, state->a); break;
        case 0x98: sbb(state, state->b); break;
        case 0x99: sbb(state, state->c); break;
        case 0x9a: sbb(state, state->d); break;
        case 0x9b: sbb(state, state->e); break;
        case 0x9c: sbb(state, state->h); break;
        case 0x9d: sbb(state, state->l); break;
        case 0x9e: sbb(state, *get_m(state)); break;
        case 0x9f: sbb(state, state->a); break;

        case 0xa0: ana(state, state->b); break;
        case 0xa1: ana(state, state->c); break;
        case 0xa2: ana(state, state->d); break;
        case 0xa3: ana(state, state->e); break;
        case 0xa4: ana(state, state->h); break;
        case 0xa5: ana(state, state->l); break;
        case 0xa6: ana(state, *get_m(state)); break;
        case 0xa7: ana(state, state->a); break;
        case 0xa8: xra(state, state->b); break;
        case 0xa9: xra(state, state->c); break;
        case 0xaa: xra(state, state->d); break;
        case 0xab: xra(state, state->e); break;
        case 0xac: xra(state, state->h); break;
        case 0xad: xra(state, state->l); break;
        case 0xae: xra(state, *get_m(state)); break;
        case 0xaf: xra(state, state->a); break;

        case 0xb0: ora(state, state->b); break;
        case 0xb1: ora(state, state->c); break;
        case 0xb2: ora(state, state->d); break;
        case 0xb3: ora(state, state->e); break;
        case 0xb4: ora(state, state->h); break;
        case 0xb5: ora(state, state->l); break;
        case 0xb6: ora(state, *get_m(state)); break;
        case 0xb7: ora(state, state->a); break;
        case 0xb8: cmp(state, state->b); break;
        case 0xb9: cmp(state, state->c); break;
        case 0xba: cmp(state, state->d); break;
        case 0xbb: cmp(state, state->e); break;
        case 0xbc: cmp(state, state->h); break;
        case 0xbd: cmp(state, state->l); break;
        case 0xbe: cmp(state, *get_m(state)); break;
        case 0xbf: cmp(state, state->a); break;

        case 0xc0: rnz(state); break;
        case 0xc1: pop(state, &state->b, &state->c); break;
        case 0xc2: jnz(state, op_code); break;   
        case 0xc3: jmp(&state->pc, op_code); break;
        case 0xc4: cnz(state, op_code); break;
        case 0xc5: push(state, state->b, state->c); break;
        case 0xc6: adi(state, op_code); break;
        case 0xc7: rst(state, 0x00); break;
        case 0xc8: rz(state); break;
        case 0xc9: ret(state); break;
        case 0xca: jz(state, op_code); break;
        case 0xcb: break;
        case 0xcc: cz(state, op_code); break;
        case 0xcd: call(state, op_code); break;
        case 0xce: aci(state, op_code); break;
        case 0xcf: rst(state, 0x08); break;

        case 0xd0: rnc(state); break;
        case 0xd1: pop(state, &state->d, &state->e); break;
        case 0xd2: jnc(state, op_code); break;
        case 0xd3: out(state, op_code); break;
        case 0xd4: cnc(state, op_code); break;
        case 0xd5: push(state, state->d, state->e); break;
        case 0xd6: sui(state, op_code); break;
        case 0xd7: rst(state, 0x10); break;
        case 0xd8: rc(state); break;
        case 0xd9: break;
        case 0xda: jc(state, op_code); break;
        case 0xdb: in(state, op_code); break;
        case 0xdc: cc(state, op_code); break;
        case 0xdd: break;
        case 0xde: sbi(state, op_code); break;
        case 0xdf: rst(state, 0x18); break;

        case 0xe0: rpo(state); break;
        case 0xe1: pop(state, &state->h, &state->l); break;
        case 0xe2: jpo(state, op_code); break;
        case 0xe3: xthl(state); break;
        case 0xe4: cpo(state, op_code); break;
        case 0xe5: push(state, state->h, state->l); break;
        case 0xe6: ani(state, op_code); break;
        case 0xe7: rst(state, 0x20); break;
        case 0xe8: rpe(state); break;
        case 0xe9: pchl(state); break;
        case 0xea: jpe(state, op_code); break;
        case 0xeb: xchg(state); break;
        case 0xec: cpe(state, op_code); break;
        case 0xed: break;
        case 0xee: xri(state, op_code); break;
        case 0xef: rst(state, 0x28); break;

        case 0xf0: rp(state); break;
        case 0xf1: pop_psw(state); break;
        case 0xf2: jp(state, op_code); break;
        case 0xf3: di(state); break;
        case 0xf4: cp(state, op_code); break;
        case 0xf5: push_psw(state); break;
        case 0xf6: ori(state, op_code); break;
        case 0xf7: rst(state, 0x30); break;
        case 0xf8: rm(state); break;
        case 0xf9: sphl(state); break;
        case 0xfa: jm(state, op_code); break;
        case 0xfb: ei(state); break;
        case 0xfc: cm(state, op_code); break;
        case 0xfd: break;
        case 0xfe: cpi(state, op_code); break;
        case 0xff: rst(state, 0x38); break;
    }
    #ifndef NDEBUG
    print_state(state);
    #endif
    return 0;
}

uint8_t parity(uint8_t x)
{
    uint8_t p = 0;
    p = x ^ (x >> 1);
    p = p ^ (p >> 2);
    p = p ^ (p >> 4);

    return (p & 1) == 0;
}

//OPCODE FUN
void mov(uint8_t * reg, uint8_t val)
{
    *reg = val;
}

void add(State8080 *state, uint8_t val)
{
  
    uint16_t sum = (uint16_t) state->a + (uint16_t) val;
    uint8_t truncated_sum = sum & 0xff;
    state->cc.z = (truncated_sum == 0);
    state->cc.s = truncated_sum >> 7;
    state->cc.cy = (sum > 0xff);
    //unsure about this
    state->cc.ac = (truncated_sum & 0xF) == 0;
    state->cc.p = parity(truncated_sum);
    state->a = truncated_sum;
}

void mvi(uint8_t * reg, unsigned char *op_code, uint16_t *pc)
{
    *reg = op_code[1];
    *pc += 1;
}

void hlt(State8080 *state)
{
    state->halt = 1;
}

void rrc(State8080 *state)
{   
    state->cc.cy = state->a & 1;
    state->a = (state->a >> 1) | (state->cc.cy << 7);
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
void jz(State8080* state, unsigned char* op_code)
{
    if (1 == state->cc.z)
        state->pc = (op_code[2] << 8) | op_code[1];
    else
        // branch not taken    
        state->pc += 2;
}

void jnc(State8080* state, unsigned char* op_code)
{
    if (0 == state->cc.cy)
        state->pc = (op_code[2] << 8) | op_code[1];
    else
        // branch not taken    
        state->pc += 2;
}

void jc(State8080* state, unsigned char* op_code)
{
	if (1 == state->cc.cy)
		state->pc = (op_code[2] << 8) | op_code[1];
	else
		// branch not taken    
		state->pc += 2;
}
void jpe(State8080* state, unsigned char* op_code)
{
	if (1 == state->cc.p)
		state->pc = (op_code[2] << 8) | op_code[1];
	else
		// branch not taken    
		state->pc += 2;
}

void adi(State8080 *state, unsigned char *op_code)
{
    uint16_t sum = (uint16_t)state->a + (uint16_t)op_code[1];
    uint8_t truncated_sum = sum & 0xff;
    state->cc.z = (truncated_sum == 0);
    state->cc.s = truncated_sum >> 7;
    state->cc.cy = (sum > 0xff);
    //unsure about this
    state->cc.ac = (truncated_sum & 0xF) == 0;
    state->cc.p = parity(truncated_sum);
    state->a = truncated_sum;
    
    state->pc+=1;
}

void ret(State8080 *state)
{
    state->pc = (state->memory[(*memory_mapping_read_ptr)(state->sp + 1)] << 8) | state->memory[(*memory_mapping_read_ptr)(state->sp)];
    state->sp += 2;
}
void rz(State8080 *state)
{
    if (state->cc.z == 1) {
        ret(state);
    }
		
}

void rnz(State8080 *state)
{
	if (state->cc.z == 0) {
		ret(state);
	}
}
void rm(State8080 *state)
{
    if (state->cc.s == 1) {
        ret(state);
    }
}
void call (State8080 *state, unsigned char *op_code)
{
    uint16_t counter = state->pc+2;
    push(state, (counter >> 8) & 0xff, counter & 0xff); 
    jmp(&state->pc, op_code);
}

void rar(State8080 *state)
{
    uint8_t x = state->a;    
    state->a = (state->cc.cy << 7) | (x >> 1);    
    state->cc.cy = x & 1;  
}

void ani(State8080 *state, unsigned char *op_code)
{
    uint8_t res = state->a & op_code[1];

    state->cc.cy = 0;
    state->cc.ac = ((state->a | op_code[1]) & 0x08) != 0;
    state->cc.z = (res == 0);
    state->cc.s = res >> 7;
    state->cc.p = parity(res);
    state->a = res;
    state->pc++;
}

void cpi(State8080 *state, unsigned char *op_code)
{
    int16_t res = state->a - op_code[1];
    state->cc.cy = res >> 8;
    state->cc.ac = ~(state->a ^ res ^ op_code[1]) & 0x10;
    state->cc.z = (res == 0);
    state->cc.s = res >> 7;
    state->cc.p = parity(res);
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
    (*out_ptr)(op_code);
    state->pc += 1;
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
    (*in_ptr)(op_code);
    state->pc += 1;
}

void push(State8080 *state, uint8_t val1, uint8_t val2)
{
    (*memory_mapping_write_ptr)(state->sp - 1, val1);
    (*memory_mapping_write_ptr)(state->sp - 2, val2);
    
    state->sp = state->sp - 2;   
}

void dcr(State8080* state, uint8_t* reg)
{
    uint8_t sum = *reg - 1;
    state->cc.ac = !((sum & 0xF) == 0xF);
    state->cc.z = (sum == 0);
    state->cc.s = sum >> 7;
    state->cc.p = parity(sum);
    *reg = sum;
}

void dcx(uint8_t* reg1, uint8_t* reg2)
{
    uint16_t combined_regs = (*reg1 << 8) | *reg2;
    combined_regs -= 1;
    *reg1 = combined_regs >> 8;
    *reg2 = combined_regs & 0xff;
}

void lxi(State8080* state, unsigned char* op_code, uint8_t* reg1, uint8_t* reg2)
{
	*reg1 = op_code[2];
	*reg2 = op_code[1];
	state->pc += 2;
}

void dad(State8080* state, uint8_t val1, uint8_t val2)
{
    uint16_t combined_values = (val1 << 8) | val2;
    uint16_t hl = (state->h << 8) | state->l;
    state->cc.cy = ((hl + combined_values) >> 16) & 1;
    
    uint16_t res = hl + combined_values;
        
    state->h = res >> 8;
    state->l = res & 0xff;
}

void dad_sp(State8080* state)
{
	uint16_t sp = state->sp;
	uint16_t hl = (state->h << 8) | state->l;
	state->cc.cy = ((hl + sp) >> 16) & 1;
	
	uint16_t res = hl + sp;
		
	state->h = res >> 8;
	state->l = res & 0xff;
}

void ldax(State8080* state, uint8_t val1, uint8_t val2)
{
    uint16_t offset = (val1 << 8) | val2;
	state->a = state->memory[(*memory_mapping_read_ptr)(offset)];
}

void inx(uint8_t* reg1, uint8_t* reg2)
{
    uint16_t combined_regs = (*reg1 << 8) | *reg2;
	combined_regs += 1;
	*reg1 = combined_regs >> 8;
	*reg2 = combined_regs & 0xff;
}

void inr(State8080* state, uint8_t* reg)
{
    uint8_t sum = *reg + 1;
    state->cc.ac = ((sum & 0xF) == 0);
    state->cc.z = (sum == 0);
    state->cc.s = sum >> 7;
    state->cc.p = parity(sum);
    *reg = sum;
}

void lxi_sp(State8080* state, unsigned char* op_code)
{
    state->sp = (op_code[2] << 8) | op_code[1];
	state->pc += 2;
}
void sta(State8080* state, unsigned char* op_code)
{
    uint16_t offset = (op_code[2] << 8) | (op_code[1]);
    (*memory_mapping_write_ptr)(offset, state->a);
    state->pc += 2;
}
void lda(State8080* state, unsigned char* op_code)
{
	uint16_t offset = (op_code[2] << 8) | (op_code[1]);
	state->a = state->memory[(*memory_mapping_read_ptr)(offset)];
	state->pc += 2;
}
void ana(State8080* state, uint8_t val)
{   
    uint8_t res = state->a & val;

    state->cc.cy = 0;
    state->cc.ac = ((state->a | val) & 0x08) != 0;
    state->cc.z = (res == 0);
    state->cc.s = res >> 7;
    state->cc.p = parity(res);
    state->a = res;
}
void xra(State8080* state, uint8_t val)
{
    state->a ^= val;
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.z = (state->a == 0);
    state->cc.s = state->a >> 7;
    state->cc.p = parity(state->a);
}

void pop(State8080* state, uint8_t* reg1, uint8_t* reg2)
{
	*reg1 = state->memory[(*memory_mapping_read_ptr)(state->sp+1)];
	*reg2 = state->memory[(*memory_mapping_read_ptr)(state->sp)];
	state->sp += 2;
}
void pop_psw(State8080* state)
{
    state->a = state->memory[(*memory_mapping_read_ptr)(state->sp + 1)];
    uint8_t psw = state->memory[(*memory_mapping_read_ptr)(state->sp)];
 
    state->cc.cy = (psw >> 0) & 1;
    state->cc.p = (psw >> 2) & 1;
    state->cc.ac = (psw >> 4) & 1;
    state->cc.z = (psw >> 6) & 1;
    state->cc.s = (psw >> 7) & 1;

    state->sp += 2;
}
void push_psw(State8080* state)
{
    uint8_t psw = (state->cc.cy |
        1 << 1 |
        state->cc.p << 2 |
        state->cc.ac << 4 |
        state->cc.z << 6 |
        state->cc.s << 7);
    push(state, state->a, psw);
}

void shld(State8080* state, unsigned char* op_code)
{
	uint16_t offset = (op_code[2] << 8) | (op_code[1]);
	(*memory_mapping_write_ptr)(offset, state->l);
	(*memory_mapping_write_ptr)(offset + 1, state->h);
	state->pc += 2;
}

void lhld(State8080* state, unsigned char* op_code)
{
	uint16_t offset = (op_code[2] << 8) | (op_code[1]);
	state->l = state->memory[(*memory_mapping_read_ptr)(offset)];
	state->h = state->memory[(*memory_mapping_read_ptr)(offset + 1)];
	state->pc += 2;
}

void pchl(State8080* state)
{
	state->pc = (state->h << 8) | state->l;
}

void xthl(State8080* state)
{
	uint8_t temp = state->l;
	state->l = state->memory[(*memory_mapping_read_ptr)(state->sp)];
	(*memory_mapping_write_ptr)(state->sp, temp);
	temp = state->h;
	state->h = state->memory[(*memory_mapping_read_ptr)(state->sp + 1)];
	(*memory_mapping_write_ptr)(state->sp + 1, temp);
}

void sphl(State8080* state)
{
	state->sp = (state->h << 8) | state->l;
}

void rlc(State8080* state)
{
	uint8_t x = state->a;
	state->a = (x << 1) | (x >> 7);
	state->cc.cy = (x >> 7) & 1;
}

void cmc(State8080* state)
{
	state->cc.cy = !state->cc.cy;
}

void daa(State8080* state)
{
    uint8_t lower_nibble = state->a & 0x0F;
    uint8_t upper_nibble = state->a >> 4;
    uint8_t correction = 0;
    if (state->cc.ac || lower_nibble > 9)
    {
        correction |= 0x06;
    }
    if (state->cc.cy || upper_nibble > 9 || (upper_nibble >= 9 && lower_nibble > 9))
    {
        correction |= 0x60;
        state->cc.cy = 1;
    }
    state->a += correction;
    state->cc.z = (state->a == 0);
    state->cc.s = state->a >> 7;
    state->cc.p = parity(state->a);
}

void stax(State8080* state, uint8_t val1, uint8_t val2)
{
    uint16_t offset = (val1 << 8) | val2;
    (*memory_mapping_write_ptr)(offset, state->a);
}

void cz(State8080* state, unsigned char* op_code)
{
    if (state->cc.z == 1)
    {
        call(state, op_code);
    }
    else
    {
        state->pc += 2;
    }
}

void cnz(State8080* state, unsigned char* op_code)
{
	if (state->cc.z == 0)
	{
		call(state, op_code);
	}
    else
    {
        state->pc += 2;
    }
}

void cnc(State8080* state, unsigned char* op_code)
{
	if (state->cc.cy == 0)
	{
		call(state, op_code);
	}
	else
	{
		state->pc += 2;
	}
}

void jm(State8080* state, unsigned char* op_code)
{
    if (state->cc.s == 1)
    {
        state->pc = (op_code[2] << 8) | op_code[1];
    }
    else
    {
        state->pc += 2;
    }
}

void ora(State8080* state, uint8_t val)
{
    state->a |= val;
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.z = (state->a == 0);
    state->cc.s = state->a >> 7;
    state->cc.p = parity(state->a);
}

void adc(State8080* state, uint8_t val)
{
    uint16_t sum = (uint16_t)state->a + (uint16_t)val + state->cc.cy;
    uint8_t truncated_sum = sum & 0xff;
    state->cc.z = (truncated_sum == 0);
    state->cc.s = truncated_sum >> 7;
    state->cc.cy = (sum > 0xff);
    state->cc.ac = (truncated_sum & 0xF) == 0;
    state->cc.p = parity(truncated_sum);
    state->a = truncated_sum;
}

void sbi(State8080* state, unsigned char* op_code)
{
    uint16_t sum = (uint16_t)state->a - (uint16_t)op_code[1] - state->cc.cy;
    uint8_t truncated_sum = sum & 0xff;
    state->cc.z = (truncated_sum == 0);
    state->cc.s = truncated_sum >> 7;
    state->cc.cy = (sum > 0xff);
    state->cc.ac = (truncated_sum & 0xF) == 0;
    state->cc.p = parity(truncated_sum);
    state->a = truncated_sum;
    state->pc++;
}
void rst(State8080* state, uint16_t op_code)
{
	push(state, (state->pc >> 8) & 0xff, state->pc & 0xff);
	state->pc = op_code;
}

void stc(State8080* state)
{
	state->cc.cy = 1;
}

void rc(State8080* state)
{
	if (state->cc.cy == 1)
	{
		ret(state);
	}
}
void rnc(State8080* state)
{
	if (state->cc.cy == 0)
	{
		ret(state);
	}
}

void sui(State8080* state, unsigned char* op_code)
{
	uint16_t sum = (uint16_t)state->a - (uint16_t)op_code[1];
	uint8_t truncated_sum = sum & 0xff;
	state->cc.z = (truncated_sum == 0);
	state->cc.s = truncated_sum >> 7;
	state->cc.cy = (sum > 0xff);
	state->cc.ac = (truncated_sum & 0xF) == 0;
	state->cc.p = parity(truncated_sum);
	state->a = truncated_sum;
	state->pc++;
}

void ori(State8080* state, unsigned char* op_code)
{
	state->a |= op_code[1];
	state->cc.cy = 0;
	state->cc.ac = 0;
	state->cc.z = (state->a == 0);
	state->cc.s = state->a >> 7;
	state->cc.p = parity(state->a);
	state->pc++;
}

void cmp(State8080* state, uint8_t val)
{
	uint16_t res = state->a - val;
	state->cc.cy = res >> 8;
	state->cc.ac = ~(state->a ^ res ^ val) & 0x10;
	state->cc.z = (res == 0);
	state->cc.s = res >> 7;
	state->cc.p = parity(res);
}

void sub(State8080* state, uint8_t val)
{
	uint16_t sum = (uint16_t)state->a - (uint16_t)val;
	uint8_t truncated_sum = sum & 0xff;
	state->cc.z = (truncated_sum == 0);
	state->cc.s = truncated_sum >> 7;
	state->cc.cy = (sum > 0xff);
	state->cc.ac = (truncated_sum & 0xF) == 0;
	state->cc.p = parity(truncated_sum);
	state->a = truncated_sum;
}

void sbb(State8080* state, uint8_t val)
{
	uint16_t sum = (uint16_t)state->a - (uint16_t)val - state->cc.cy;
	uint8_t truncated_sum = sum & 0xff;
	state->cc.z = (truncated_sum == 0);
	state->cc.s = truncated_sum >> 7;
	state->cc.cy = (sum > 0xff);
	state->cc.ac = (truncated_sum & 0xF) == 0;
	state->cc.p = parity(truncated_sum);
	state->a = truncated_sum;
}

void ral(State8080* state)
{
	uint8_t x = state->a;
	state->a = (x << 1) | state->cc.cy;
	state->cc.cy = x >> 7;
}

void inx_sp(State8080* state)
{
	state->sp += 1;
}

void dcx_sp(State8080* state)
{
	state->sp -= 1;
}

void aci(State8080* state, unsigned char* op_code)
{
	uint16_t sum = (uint16_t)state->a + (uint16_t)op_code[1] + state->cc.cy;
	uint8_t truncated_sum = sum & 0xff;
	state->cc.z = (truncated_sum == 0);
	state->cc.s = truncated_sum >> 7;
	state->cc.cy = (sum > 0xff);
	state->cc.ac = (truncated_sum & 0xF) == 0;
	state->cc.p = parity(truncated_sum);
	state->a = truncated_sum;
	state->pc++;
}

void cc(State8080* state, unsigned char* op_code)
{
	if (state->cc.cy == 1)
	{
		call(state, op_code);
	}
	else
	{
		state->pc += 2;
	}
}

void rpo(State8080* state)
{
	if (state->cc.p == 0)
	{
		ret(state);
	}
}

void jpo(State8080* state, unsigned char* op_code)
{
	if (state->cc.p == 0)
	{
		state->pc = (op_code[2] << 8) | op_code[1];
	}
	else
	{
		state->pc += 2;
	}
}

void cpo(State8080* state, unsigned char* op_code)
{
	if (state->cc.p == 0)
	{
		call(state, op_code);
	}
	else
	{
		state->pc += 2;
	}
}

void rpe(State8080* state)
{
	if (state->cc.p == 1)
	{
		ret(state);
	}
}

void cpe(State8080* state, unsigned char* op_code)
{
	if (state->cc.p == 1)
	{
		call(state, op_code);
	}
	else
	{
		state->pc += 2;
	}
}

void xri(State8080* state, unsigned char* op_code)
{
	state->a ^= op_code[1];
	state->cc.cy = 0;
	state->cc.ac = 0;
	state->cc.z = (state->a == 0);
	state->cc.s = state->a >> 7;
	state->cc.p = parity(state->a);
	state->pc++;
}

void rp(State8080* state)
{
	if (state->cc.s == 0)
	{
		ret(state);
	}
}

void jp(State8080* state, unsigned char* op_code)
{
	if (state->cc.s == 0)
	{
		state->pc = (op_code[2] << 8) | op_code[1];
	}
	else
	{
		state->pc += 2;
	}
}

void cp(State8080* state, unsigned char* op_code)
{
	if (state->cc.s == 0)
	{
		call(state, op_code);
	}
	else
	{
		state->pc += 2;
	}
}

void cm(State8080* state, unsigned char* op_code)
{
	if (state->cc.s == 1)
	{
		call(state, op_code);
	}
	else
	{
		state->pc += 2;
	}
}