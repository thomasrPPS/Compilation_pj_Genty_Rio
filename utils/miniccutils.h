#ifndef _MINICCUTILS_H_
#define _MINICCUTILS_H_

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"



/* For checking the tree constructed */

bool check_program_tree(node_t n);


/* Context related functions */

typedef struct _context_s context_s;
typedef context_s * context_t;

context_t create_context();
void free_context(context_t context);
bool context_add_element(context_t context, char * idf, void * data);
void * get_data(context_t context, char * idf);


/* Environment related functions */

void push_global_context();
void push_context();
void pop_context();
void * get_decl_node(char * ident);
int32_t env_add_element(char * ident, void * node);
void reset_env_current_offset();
int32_t get_env_current_offset();
int32_t add_string(char * str);
int32_t get_global_strings_number();
char * get_global_string(int32_t index);
void free_global_strings();


/* Register allocation related functions */

void push_temporary(int32_t reg);
void pop_temporary(int32_t reg);
bool reg_available();
int32_t get_current_reg();
int32_t get_restore_reg();
void allocate_reg();
void release_reg();
int32_t get_new_label();
void set_temporary_start_offset(int32_t offset);
void set_max_registers(int32_t n);
void reset_temporary_max_offset();
int32_t get_temporary_max_offset();
int32_t get_temporary_curr_offset(); // for debug


/* Program creation related functions */

void inst_create_data_sec();
void inst_create_text_sec();
void inst_create_word(char * label, int32_t init_value);
void inst_create_asciiz(char * label_str, char * str);
void inst_create_label(int32_t label);
void inst_create_label_str(char * label);
void inst_create_comment(char * comment);
void inst_create_lui(int32_t r_dest, int32_t imm);
void inst_create_addu(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_subu(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_slt(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_sltu(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_and(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_or(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_xor(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_nor(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_mult(int32_t r_src_1, int32_t r_src_2);
void inst_create_div(int32_t r_src_1, int32_t r_src_2);
void inst_create_sllv(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_srav(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_srlv(int32_t r_dest, int32_t r_src_1, int32_t r_src_2);
void inst_create_addiu(int32_t r_dest, int32_t r_src_1, int32_t imm);
void inst_create_andi(int32_t r_dest, int32_t r_src_1, int32_t imm);
void inst_create_ori(int32_t r_dest, int32_t r_src_1, int32_t imm);
void inst_create_xori(int32_t r_dest, int32_t r_src_1, int32_t imm);
void inst_create_slti(int32_t r_dest, int32_t r_src_1, int32_t imm);
void inst_create_sltiu(int32_t r_dest, int32_t r_src_1, int32_t imm);
void inst_create_lw(int32_t r_dest, int32_t imm, int32_t r_src_1);
void inst_create_sw(int32_t r_src_1, int32_t imm, int32_t r_src_2);
void inst_create_beq(int32_t r_src_1, int32_t r_src_2, int32_t label);
void inst_create_bne(int32_t r_src_1, int32_t r_src_2, int32_t label);
void inst_create_mflo(int32_t r_dest);
void inst_create_mfhi(int32_t r_dest);
void inst_create_j(int32_t label);
void inst_create_teq(int32_t r_src_1, int32_t r_src_2);
void inst_create_syscall();
void inst_create_stack_allocation();
void inst_create_stack_deallocation(int32_t val);

void create_program();
void free_program();
void dump_mips_program(char * filename);




#endif

