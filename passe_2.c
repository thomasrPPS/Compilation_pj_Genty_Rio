
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"


// global utility variables
int launch = 1;
int32_t label = 0;
int inFunc_Decl = 0;
int inBlockFunc = 0;
int parsingLoopFor = 0;
int parsingLoopWhile = 0;
int parsingLoopIf = 0;
int parsingLoopDo = 0;
int inLoopFor = 0;
int inLoopWhile = 0;
int current_reg;
int jmpLabelDecl = 0;
int blockParsed = 0;
int inLoopDo = 0;
int inIf = 0;


/* --------------- Instruction creation functions --------------- */

void decl_word_variable(node_t node){
	if (node->opr[1] != NULL && node->opr[1]->value != 0){ 
		inst_create_word(node->opr[0]->ident, node->opr[1]->value);
	}
	else{
		inst_create_word(node->opr[0]->ident, 0); // global variable with non-initialized get a 0 value
	}
}

// Print syscall creation 
void create_print_syscall(node_t node){
	for(int i = 0; i < node->nops; i++){
		// multiple element to print => recursive
		if(node->opr[i]->nature == NODE_LIST){
			create_print_syscall(node->opr[i]);
		}
		// printing a string
		else if(node->opr[i]->nature == NODE_STRINGVAL){
			inst_create_lui(4, 0x1001);
			inst_create_ori(4, 4, node->opr[i]->offset);
			inst_create_ori(2, 0, 0x4);
			inst_create_syscall();
		}
		// printing a ident
		else if(node->opr[i]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(4, 0x1001);
				inst_create_lw(4, node->opr[0]->offset , 4);
			}
			else{
				inst_create_lw(4, node->opr[0]->offset , 29);
			}
			inst_create_ori(2, 0, 0x1);
			inst_create_syscall();
		}
	}	
}

// declaration of variable inside the function
void decl_inblock(node_t node){
	if(node->opr[1] != NULL){
		if(node->opr[1]->nature == NODE_INTVAL
		|| node->opr[1]->nature == NODE_BOOLVAL){
			if(reg_available()){
				if (node->opr[1]->value <= 0xffff){
					inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
				}
				else{
					printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
					exit(EXIT_FAILURE);
				}	
				inst_create_sw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else if(node->opr[0]->ident && node->opr[1]->ident){
			if(reg_available){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
				inst_create_sw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
	}
}

// affectation of a variable
void affect_variable(node_t node){
	// affect an ident with an ident
	if(node->opr[0]->nature == NODE_IDENT
	&& node->opr[1]->nature == NODE_IDENT){
		if(reg_available()){
			if(node->opr[0]->global_decl){
				inst_create_lui(current_reg, 0x1001);
				inst_create_lw(current_reg, node->opr[1]->offset , current_reg);
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
			allocate_reg();
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_sw(current_reg, node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_sw(get_current_reg(), node->opr[1]->offset , 29);
			}

			release_reg();
		}
	}
	// affect an ident with a intval
	if((node->opr[0]->nature == NODE_IDENT 
	&& node->opr[0]->type == TYPE_INT)
	&& (node->opr[1]->nature == NODE_INTVAL)){
		if(reg_available()){
			current_reg = get_current_reg();
			allocate_reg();
			if(node->opr[0]->global_decl){
				inst_create_lui(current_reg, 0x0);				
				inst_create_ori(current_reg, current_reg, node->opr[1]->value);
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_sw(current_reg, node->opr[0]->offset , get_current_reg());
			}
			else{
				// we check if the immediate is on 16 bits 
				if (node->opr[1]->value <= 0xffff){
					inst_create_ori(current_reg, 0, node->opr[1]->value);
				}
				else{
					printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				}	
				inst_create_sw(current_reg, node->opr[0]->offset , 29);
			}
			release_reg();
		}
	}
	// affect an ident with a boolval
	if((node->opr[0]->nature == NODE_IDENT 
	&& node->opr[0]->type == TYPE_BOOL)
	&& node->opr[1]->nature == NODE_BOOLVAL){
		if(reg_available()){
			if(node->opr[0]->global_decl){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_sw(current_reg, node->opr[0]->offset , get_current_reg());
			}
			else{
				// we check if the immediate is on 16 bits 
				if (node->opr[1]->value <= 0xffff){
					inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
				}
				else{
					printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				}	
				inst_create_sw(current_reg, node->opr[0]->offset , 29);
			}
		}
	}
}

// create an lt condition
void create_lt_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		// if a variable is global, we affect it with an offset
		if(node->opr[0]->global_decl){
			inst_create_lui(get_current_reg(), 0x1001);
			inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
		}
		else{
			inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		}
		allocate_reg();
		// if the second operand is an INTVAL
		if (node->opr[1]->nature == NODE_INTVAL){
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
			}	
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		// handling experssion such as addition or substraction
		else if (node->opr[1]->nature != NODE_INTVAL){
			switch(node->opr[1]->nature){
				case NODE_PLUS :
					create_plus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MINUS :
					create_minus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_UMINUS :
					create_uminus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_DIV :
					create_div_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MUL :
					create_mul_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MOD :
					create_mod_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BNOT :
					create_bnot_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BAND :
					create_and_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BOR :
					create_or_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BXOR :
					create_bxor_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRL :
					create_srl_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRA :
					create_sra_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SLL :
					create_sll_instr(node->opr[1]);
					release_reg();
					break;
			}
			current_reg = get_current_reg()-1;
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		// if we are in a loop 
		else if (inLoopFor || inLoopWhile) {
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		release_reg();
		
		//creation of branchement
		if(inLoopFor || inLoopWhile){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
		else if(inLoopDo){
			inst_create_bne(get_current_reg(), 0, label);
		}
		else if(inIf){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
	}
}


// creation of le condition
void create_le_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->global_decl){
			inst_create_lui(get_current_reg(), 0x1001);
			inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
		}
		else{
			inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		}
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
			}	
			inst_create_slt(current_reg, get_current_reg(), current_reg);
			inst_create_xori(current_reg,  current_reg , 1);

		}
		else if (node->opr[1]->nature != NODE_INTVAL){
			switch(node->opr[1]->nature){
				case NODE_PLUS :
					create_plus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MINUS :
					create_minus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_UMINUS :
					create_uminus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_DIV :
					create_div_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MUL :
					create_mul_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MOD :
					create_mod_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BNOT :
					create_bnot_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BAND :
					create_and_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BOR :
					create_or_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BXOR :
					create_bxor_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRL :
					create_srl_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRA :
					create_sra_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SLL :
					create_sll_instr(node->opr[1]);
					release_reg();
					break;
			}
			current_reg = get_current_reg()-1;
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		else if (inLoopFor || inLoopWhile) {
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
			inst_create_slt(current_reg, get_current_reg(), current_reg);
			inst_create_xori(current_reg,  current_reg , 1);
		}
		release_reg();
		if(inLoopFor || inLoopWhile){
			inst_create_beq(current_reg, 0, label+1);
		}
		else if(inLoopDo){
			inst_create_bne(current_reg, 0, label);
		}
		else if(inIf){
			inst_create_beq(current_reg, 0, label+1);
		}
	}
}

// creation of eq condition
void create_eq_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->global_decl){
			inst_create_lui(get_current_reg(), 0x1001);
			inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
		}
		else{
			inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		}
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
			}	
			inst_create_xor(current_reg, current_reg, get_current_reg());
			inst_create_sltiu(current_reg, 0, 0x1);
		}
		else if (node->opr[1]->nature != NODE_INTVAL){
			switch(node->opr[1]->nature){
				case NODE_PLUS :
					create_plus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MINUS :
					create_minus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_UMINUS :
					create_uminus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_DIV :
					create_div_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MUL :
					create_mul_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MOD :
					create_mod_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BNOT :
					create_bnot_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BAND :
					create_and_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BOR :
					create_or_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BXOR :
					create_bxor_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRL :
					create_srl_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRA :
					create_sra_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SLL :
					create_sll_instr(node->opr[1]);
					release_reg();
					break;
			}
			current_reg = get_current_reg()-1;
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		else if (inLoopFor || inLoopWhile) {
			inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			inst_create_xor(current_reg, current_reg, get_current_reg());
			inst_create_sltiu(current_reg, current_reg, 0x1);
		}
		release_reg();
		if(inLoopFor || inLoopWhile){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
		else if(inLoopDo){
			inst_create_bne(get_current_reg(), 0, label);
		}
		else if(inIf){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
	}
}

// creation of neq condition 
void create_neq_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->global_decl){
			inst_create_lui(get_current_reg(), 0x1001);
			inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
		}
		else{
			inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		}	
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
			}	
			inst_create_xor(current_reg, current_reg, get_current_reg());
			inst_create_sltu(current_reg, 0, current_reg);
		}
		else if (node->opr[1]->nature != NODE_INTVAL){
			switch(node->opr[1]->nature){
				case NODE_PLUS :
					create_plus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MINUS :
					create_minus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_UMINUS :
					create_uminus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_DIV :
					create_div_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MUL :
					create_mul_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MOD :
					create_mod_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BNOT :
					create_bnot_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BAND :
					create_and_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BOR :
					create_or_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BXOR :
					create_bxor_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRL :
					create_srl_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRA :
					create_sra_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SLL :
					create_sll_instr(node->opr[1]);
					release_reg();
					break;
			}
			current_reg = get_current_reg()-1;
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		else if (inLoopFor || inLoopWhile) {
			inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			inst_create_xor(current_reg, current_reg, get_current_reg());
			inst_create_sltu(current_reg, 0, current_reg);
		}
		release_reg();
		if(inLoopFor || inLoopWhile){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
		else if(inLoopDo){
			inst_create_bne(get_current_reg(), 0, label);
		}
		else if(inIf){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
	}
}


// creation of lt condition 
void create_gt_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->global_decl){
			inst_create_lui(get_current_reg(), 0x1001);
			inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
		}
		else{
			inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		}
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			if (node->opr[1]->value <= 0xffff){
					inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
				}
				else{
					printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				}	
			inst_create_slt(current_reg, current_reg, get_current_reg());
			inst_create_xori(current_reg,  current_reg , 1);

		}
		else if (node->opr[1]->nature != NODE_INTVAL){
			switch(node->opr[1]->nature){
				case NODE_PLUS :
					create_plus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MINUS :
					create_minus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_UMINUS :
					create_uminus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_DIV :
					create_div_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MUL :
					create_mul_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MOD :
					create_mod_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BNOT :
					create_bnot_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BAND :
					create_and_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BOR :
					create_or_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BXOR :
					create_bxor_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRL :
					create_srl_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRA :
					create_sra_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SLL :
					create_sll_instr(node->opr[1]);
					release_reg();
					break;
			}
			current_reg = get_current_reg()-1;
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		else if (inLoopFor || inLoopWhile) {
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
			inst_create_slt(current_reg, get_current_reg(), current_reg);
		}
		release_reg();
		if(inLoopFor || inLoopWhile){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
		else if(inLoopDo){
			inst_create_bne(get_current_reg(), 0, label);
		}
		else if(inIf){
			inst_create_beq(get_current_reg(), 0, label+1);
		}
	}
}

// creation of ge condition 
void create_ge_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->global_decl){
			inst_create_lui(get_current_reg(), 0x1001);
			inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
		}
		else{
			inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		}
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
			}
			inst_create_slt(current_reg, current_reg, get_current_reg());
			inst_create_xori(current_reg,  current_reg , 1);

		}
		else if (node->opr[1]->nature != NODE_INTVAL){
			switch(node->opr[1]->nature){
				case NODE_PLUS :
					create_plus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MINUS :
					create_minus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_UMINUS :
					create_uminus_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_DIV :
					create_div_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MUL :
					create_mul_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_MOD :
					create_mod_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BNOT :
					create_bnot_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BAND :
					create_and_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BOR :
					create_or_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_BXOR :
					create_bxor_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRL :
					create_srl_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SRA :
					create_sra_instr(node->opr[1]);
					release_reg();
					break;
				case NODE_SLL :
					create_sll_instr(node->opr[1]);
					release_reg();
					break;
			}
			current_reg = get_current_reg()-1;
			inst_create_slt(current_reg, current_reg, get_current_reg());
		}
		else if (inLoopFor || inLoopWhile) {
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
			inst_create_slt(current_reg, current_reg, get_current_reg());
			inst_create_xori(current_reg,  current_reg , 1);
		}
		release_reg();
		if(inLoopFor || inLoopWhile){
			inst_create_beq(current_reg, 0, label+1);
		}
		else if(inLoopDo){
			inst_create_bne(current_reg, 0, label);
		}
		else if(inIf){
			inst_create_beq(current_reg, 0, label+1);
		}
	}
}

// create an addition
void create_plus_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		// handling multiples operations
		else if (node->opr[0]->nature == NODE_PLUS){
			create_plus_instr(node->opr[0]);
			release_reg();
			current_reg = get_current_reg() ;
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else if (node->opr[1]->nature == NODE_PLUS){
			create_plus_instr(node->opr[1]);
			release_reg();
			current_reg = get_current_reg() - 1;
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), get_current_reg(), node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_addu(current_reg, current_reg, get_current_reg());
		
	}
}

// create an substraction
void create_minus_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else if (node->opr[0]->nature == NODE_MINUS){
			create_minus_instr(node->opr[0]);
			release_reg();
			current_reg = get_current_reg() ;
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else if (node->opr[1]->nature == NODE_MINUS){
			create_minus_instr(node->opr[1]);
			release_reg();
			current_reg = get_current_reg() -1;
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_subu(current_reg, current_reg, get_current_reg());
	}
}

// create an unary substraction
void create_uminus_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_subu(current_reg, 0, current_reg);
	}
}

// create a multiplication
void create_mul_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else if (node->opr[0]->nature == NODE_MUL){
			create_mul_instr(node->opr[0]);
			release_reg();
			current_reg = get_current_reg() ;
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else if (node->opr[1]->nature == NODE_MUL){
			create_mul_instr(node->opr[1]);
			release_reg();
			current_reg = get_current_reg() - 1;
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_mult(current_reg, get_current_reg());
		inst_create_mflo(current_reg);
	}
}

// create a division
void create_div_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else if (node->opr[0]->nature == NODE_DIV){
			create_div_instr(node->opr[0]);
			release_reg();
			current_reg = get_current_reg() ;
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else if (node->opr[1]->nature == NODE_DIV){
			create_div_instr(node->opr[1]);
			release_reg();
			current_reg = get_current_reg() - 1;
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_div(current_reg, get_current_reg());
		inst_create_teq(get_current_reg(), 0);
		inst_create_mflo(current_reg);
	}
}

// create an and statement
void create_and_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){			
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_and(current_reg, current_reg, get_current_reg());
	}
}

// create a modulo
void create_mod_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else if (node->opr[0]->nature == NODE_MOD){
			create_mod_instr(node->opr[0]);
			release_reg();
			current_reg = get_current_reg() ;
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else if (node->opr[1]->nature == NODE_MOD){
			create_mod_instr(node->opr[1]);
			release_reg();
			current_reg = get_current_reg() - 1;
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_div(current_reg, get_current_reg());
		inst_create_teq(get_current_reg(), 0);
		inst_create_mfhi(current_reg);
	}
}

// create an or statement
void create_or_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){			
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_or(current_reg, current_reg, get_current_reg());
	}
}

// create a bnot statement
void create_bnot_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_nor(current_reg, 0, current_reg);
	}
}

// create a not statement
void create_not_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_xori(current_reg, current_reg, 1);
	}
}

// create a bit shift to the right
void create_sra_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){			
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_srav(current_reg, current_reg, get_current_reg());
	}
}

// create a bit shift to the left
void create_sll_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){			
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_sllv(current_reg, current_reg, get_current_reg());
	}
}

// // create a srl statement
void create_srl_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){			
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_srlv(current_reg, current_reg, get_current_reg());
	}
}

// create a bxor statement
void create_bxor_instr(node_t node){
	if (reg_available()){
		current_reg = get_current_reg();
		if(node->opr[0]->nature == NODE_IDENT){
			if(node->opr[0]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
			}
		}
		else{
			if (node->opr[0]->value <= 0xffff){
				inst_create_ori(current_reg, 0, node->opr[0]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		allocate_reg();
		if(node->opr[1]->nature == NODE_IDENT){			
			if(node->opr[1]->global_decl){
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			}
			else{
				inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			}
		}
		else{
			if (node->opr[1]->value <= 0xffff){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			}
			else{
				printf("Error line %d: immediate must be a 16 bits integer\n", node->opr[1]->lineno);
				exit(EXIT_FAILURE);
			}
		}
		inst_create_xor(current_reg, current_reg, get_current_reg());
	}
}


/* --------------- Main functions --------------- */

void gen_code_passe_2(node_t root) {
	if (launch){
		// launching the program
		create_program();
		inst_create_data_sec();
		launch = 0;
	}
	for(int i = 0; i < root->nops; i++){
		if (root->opr[i] != NULL){
			switch(root->opr[i]->nature){
				case NODE_DECL :
				// .data declaration of variables
					if(root->opr[i]->opr[0]->global_decl){
						decl_word_variable(root->opr[i]);
					}
					else{
						decl_inblock(root->opr[i]);
					}
					break;

				case NODE_FUNC :
				// FUNC initilization 
					inst_create_label_str("main");
					inst_create_stack_allocation();
					break;		

				case NODE_AFFECT :
				// in 'for' loop block must be parsed before incrementing the index
					if (root->opr[i+1] != NULL && root->opr[i+1]->nature == NODE_BLOCK){
						gen_code_passe_2(root->opr[i+1]);
						blockParsed = 1;
					}
				// instruction creation
					if (root->opr[i]->opr[1]->nature == NODE_PLUS){
						create_plus_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_MINUS){
						create_minus_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_UMINUS){
						create_uminus_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								allocate_reg();
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
								release_reg();
							}
							else{		
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset, 29);
							}
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_MUL){
						create_mul_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();

						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_DIV){
						create_div_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_MOD){
						create_mod_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_BAND){
						create_and_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_BOR){
						create_or_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_BXOR){
						create_bxor_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_BNOT){
						create_not_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								allocate_reg();
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
								release_reg();
							}
							else{		
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset, 29);
							}
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_AND){
						create_and_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_OR){
						create_or_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_NOT){
						create_not_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								allocate_reg();
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
								release_reg();
							}
							else{		
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset, 29);
							}
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_SRA){
						create_sra_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_SLL){
						create_sll_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					if (root->opr[i]->opr[1]->nature == NODE_SRL){
						create_srl_instr(root->opr[i]->opr[1]);
						if(reg_available()){
							if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
								inst_create_lui(get_current_reg(), 0x1001);
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , get_current_reg());
							}
							else{
								inst_create_sw(current_reg, root->opr[i]->opr[0]->offset , 29);
							}	
							release_reg();
						}
					}
					
					else{
						affect_variable(root->opr[i]);
					}
					break;
				

			//loop instruction determination
				case NODE_LT :
					create_lt_instr(root->opr[i]);
					break;
				case NODE_GT :
					create_gt_instr(root->opr[i]);
					break;
				case NODE_EQ :
					create_eq_instr(root->opr[i]);
					break;
				case NODE_NE :
					create_neq_instr(root->opr[i]);
					break;
				case NODE_GE :
					create_ge_instr(root->opr[i]);
					break;
				case NODE_LE :
					create_le_instr(root->opr[i]);
					break;

			// case if the FOR index initialisation is a ident not an affectation
				case NODE_FOR :
					if (root->opr[i]->opr[0]->nature == NODE_IDENT){
						if(root->opr[i]->opr[0]->global_decl || root->opr[i]->opr[1]->global_decl){
							inst_create_lui(get_current_reg(), 0x1001);
							inst_create_lw(get_current_reg(), root->opr[i]->opr[0]->offset , get_current_reg());
						}
						else{
							inst_create_lw(get_current_reg(), root->opr[i]->opr[0]->offset , 29);
						}
					}
					break;

			// creation of the While loop
				case NODE_WHILE :
					inLoopWhile = 1;
					label++;
					inst_create_label(label);
					break;

			// creation of the While loop
				case NODE_DOWHILE :
					label++;
					inst_create_label(label);
					break;

			// creation of the If Statement
				case NODE_IF :
					inIf = 1;
					break;	
			}		
		}
		if(root->opr[i] != NULL){
			if (!(root->opr[i]->nature == NODE_BLOCK && blockParsed)){
				gen_code_passe_2(root->opr[i]);
			}
		}
		if(root->nature == NODE_PROGRAM){
			// End the program, else go to the .text section
			if (inFunc_Decl){
				if(root->opr[1] != NULL){
					inst_create_stack_deallocation(root->opr[1]->offset);
				}
				inst_create_ori(2, 0, 10);
				inst_create_syscall();
			}
			else{
				// declare the string in the .data section
				for (int i = 0; i < get_global_strings_number(); i++){
					inst_create_asciiz(NULL, get_global_string(i));
				}
				inst_create_text_sec();
				inFunc_Decl = 1;
			}
		}
		// create a syscall for a print
		if(root->nature == NODE_PRINT){
			create_print_syscall(root);	
		}
		// creation of label for 'for' loop
		if(root->nature == NODE_FOR){
			if( parsingLoopFor == 0){
				label++;
				inst_create_label(label);
				parsingLoopFor++;
				inLoopFor = 1;
			}
			else if (parsingLoopFor == root->nops - 1){
				inst_create_j(label);
				label++;
				inst_create_label(label);
				parsingLoopFor = 0;
				inLoopFor = 0;
				blockParsed = 0;
			}
			else{
				parsingLoopFor++;
			}
		}
		// creation of label for 'while' loop
		if (root->nature == NODE_WHILE){
			if(parsingLoopWhile == root->nops - 1){
				inst_create_j(label);
				label++;
				inst_create_label(label);
				parsingLoopWhile = 0;
				inLoopWhile = 0;
				blockParsed = 0;
			}
			else{
				parsingLoopWhile++;
			}
		}
		// creation of label for 'dowhile' loop
		if(root->nature == NODE_DOWHILE){
			inLoopDo = 1;
			if(parsingLoopDo == root->nops - 1){
				parsingLoopDo = 0;
				inLoopDo = 0;
			}
			else{
				parsingLoopDo++;
			}			
		}
		// creation of label for 'if' statement
		if(root->nature == NODE_IF){
			if(parsingLoopIf == root->nops-1){
				label++;
				inst_create_label(label);	
				parsingLoopIf = 0;
				inIf = 0;
			}
			else{
				parsingLoopIf++;
			}
		}
	}
}

 