
#include <stdio.h>

#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"


// global utility variables
int launch = 1;
int32_t label = 0;
// int32_t LoopLabel;
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

void decl_word_variable(node_t node){
	if (node->opr[1] != NULL && node->opr[1]->value != 0){ 
		inst_create_word(node->opr[0]->ident, node->opr[1]->value);
	}
	else{
		inst_create_word(node->opr[0]->ident, 0); // global variable with non-initialized get a 0 value
	}
}


void create_print_syscall(node_t node){
	for(int i = 0; i < node->nops; i++){
		if(node->opr[i]->nature == NODE_LIST){
			create_print_syscall(node->opr[i]);
		}
		else if(node->opr[i]->nature == NODE_STRINGVAL){
			inst_create_lui(4, 0x1001);
			inst_create_ori(4, 4, node->opr[i]->offset);
			inst_create_ori(2, 0, 0x4);
			inst_create_syscall();
		}
		else if(node->opr[i]->nature == NODE_IDENT){
			inst_create_lw(4, node->opr[i]->offset , 29);
			inst_create_ori(2, 0, 0x1);
			inst_create_syscall();
		}
	}	
}


void decl_inblock(node_t node){
	if(node->opr[1] != NULL){
		if(node->opr[1]->nature == NODE_INTVAL
		|| node->opr[1]->nature == NODE_BOOLVAL){
			if(reg_available()){
				inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
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


void affect_variable(node_t node){
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
	if((node->opr[0]->nature == NODE_IDENT 
	&& node->opr[0]->type == TYPE_INT)
	&& (node->opr[1]->nature == NODE_INTVAL)){
		if(reg_available()){
			if(node->opr[0]->global_decl){
				inst_create_ori(current_reg, 0, node->opr[1]->value);
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_sw(current_reg, node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_ori(current_reg, 0, node->opr[1]->value);
				inst_create_sw(current_reg, node->opr[0]->offset , 29);
			}
			release_reg();
		}
	}
	if((node->opr[0]->nature == NODE_IDENT 
	&& node->opr[0]->type == TYPE_BOOL)
	&& node->opr[1]->nature == NODE_BOOLVAL){
		if(reg_available()){
			if(node->opr[0]->global_decl){
				inst_create_ori(current_reg, 0, node->opr[1]->value);
				inst_create_lui(get_current_reg(), 0x1001);
				inst_create_sw(current_reg, node->opr[0]->offset , get_current_reg());
			}
			else{
				inst_create_ori(current_reg, 0, node->opr[1]->value);
				inst_create_sw(current_reg, node->opr[0]->offset , 29);
			}
			release_reg();
		}
	}
	
}


void create_lt_instr(node_t node){
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
			inst_create_ori(get_current_reg(), 0 , node->opr[1]->value);
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
		release_reg();
	}
}

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
			inst_create_ori(get_current_reg(), 0 , node->opr[1]->value);
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

void create_eq_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			inst_create_ori(get_current_reg(), 0 , node->opr[1]->value);
			inst_create_xor(current_reg, current_reg, get_current_reg());
			inst_create_sltiu(current_reg, 0, 0x1);
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

void create_neq_instr(node_t node){
	if(reg_available()){
		current_reg = get_current_reg();
		inst_create_lw(get_current_reg(), node->opr[0]->offset , 29);
		allocate_reg();
		if (node->opr[1]->nature == NODE_INTVAL){
			inst_create_ori(get_current_reg(), 0 , node->opr[1]->value);
			inst_create_xor(current_reg, current_reg, get_current_reg());
			inst_create_sltu(current_reg, 0, current_reg);
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
			inst_create_ori(get_current_reg(), 0 , node->opr[1]->value);
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
			inst_create_ori(get_current_reg(), 0 , node->opr[1]->value);
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
		else{
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_addu(current_reg, current_reg, get_current_reg());
	}
}

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
		else{
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_subu(current_reg, current_reg, get_current_reg());
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
		}
		inst_create_subu(current_reg, 0, current_reg);
	}
}

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
		else{
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_mult(current_reg, get_current_reg());
		inst_create_mflo(current_reg);
	}
}

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
			release_reg();
		}
		else{
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_div(current_reg, get_current_reg());
		inst_create_teq(get_current_reg(), 0);
		inst_create_mflo(current_reg);
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_and(current_reg, current_reg, get_current_reg());
	}
}

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
		else{
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_div(current_reg, get_current_reg());
		inst_create_teq(get_current_reg(), 0);
		inst_create_mfhi(current_reg);
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_or(current_reg, current_reg, get_current_reg());
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
		}
		inst_create_nor(current_reg, 0, current_reg);
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
		}
		inst_create_xori(current_reg, current_reg, 1);
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_srav(current_reg, current_reg, get_current_reg());
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_sllv(current_reg, current_reg, get_current_reg());
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_srlv(current_reg, current_reg, get_current_reg());
	}
}

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
			inst_create_ori(current_reg, 0, node->opr[0]->value);
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
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
		}
		inst_create_xor(current_reg, current_reg, get_current_reg());
	}
}


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

				case NODE_WHILE :
					inLoopWhile = 1;
					label++;
					inst_create_label(label);
					break;

				case NODE_DOWHILE :
					label++;
					inst_create_label(label);
					break;

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
		if(root->nature == NODE_PRINT){
			create_print_syscall(root);	
		}
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

 