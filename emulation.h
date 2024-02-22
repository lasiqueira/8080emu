#include <stdlib.h>
#include "cpu.h"

//EMU
int disassemble_8080_op(unsigned char *code_buffer, int pos);
void unimplemented_instruction(State8080* state);
int emulate_8080_op(State8080* state);
int parity(int x, int size);

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
