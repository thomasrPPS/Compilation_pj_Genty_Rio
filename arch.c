#include <stdint.h>

#include "arch.h"

#define num_arch_registers 8
static int32_t max_regs = num_arch_registers;


void set_max_registers(int32_t n) {
    max_regs = n;
}

int32_t get_num_registers() {
    return max_regs;
}

int32_t get_num_arch_registers() {
    return num_arch_registers;
}

int32_t get_first_reg() {
    return 8;
}

int32_t get_r0() {
    return 0;
}

int32_t get_stack_reg() {
    return 29;
}

int32_t get_data_sec_start_addr() {
    return 0x10010000;
}

