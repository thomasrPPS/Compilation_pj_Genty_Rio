#ifndef _ARCH_H_
#define _ARCH_H_


void set_max_registers(int32_t n);
int32_t get_num_registers();
int32_t get_num_arch_registers();
int32_t get_first_reg();
int32_t get_r0();
int32_t get_stack_reg();
int32_t get_data_sec_start_addr();


#endif
