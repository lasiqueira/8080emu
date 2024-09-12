#include <stdlib.h>

void init();
void quit();
void tst_in(unsigned char *op_code);
void tst_out(unsigned char *op_code);
uint16_t memory_mapping_read(uint16_t address);
void memory_mapping_write(uint16_t address, uint8_t data);


