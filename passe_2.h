#ifndef _PASSE_2_H_
#define _PASSE_2_H_

#include "defs.h"

void gen_code_passe_2(node_t root);

void decl_word_variable(node_t node);
void create_print_syscall(node_t node);
void decl_inblock(node_t node);
void affect_variable(node_t node);
void create_lt_instruction(node_t node);
void create_gt_instruction(node_t node);
void create_plus_instr(node_t node);
#endif

