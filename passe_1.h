
#ifndef _PASSE_1_
#define _PASSE_1_

#include "defs.h"


void analyse_passe_1(node_t root);
void check_add_type(node_t node);
void check_minus_type(node_t node);
void check_mul_type(node_t node);
void check_div_type(node_t node);
void check_bool_expr(node_t node, int positionnal);
void check_main_void(node_t node);
void check_affect_type(node_t node);
void check_global_decl(node_t node);
void check_intval_domain(node_t node);


#endif

