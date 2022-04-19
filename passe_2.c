
#include <stdio.h>

#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"

int launch = 1;
//char* current_label;
int32_t label = 0;
int inFunc_Decl = 0;
int inBlockFunc = 0;
int parsingLoop = 0;

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
			inst_create_lui(8, 0x1001);
			inst_create_lw(get_current_reg(), node->opr[1]->offset , get_current_reg());
			inst_create_sw(get_current_reg(), node->opr[0]->offset , 29);
		}
	}
}

void affect_variable(node_t node){
	if(node->opr[0]->nature == NODE_IDENT
	&& node->opr[1]->nature == NODE_IDENT){
		if(reg_available()){
			inst_create_lw(get_current_reg(), node->opr[1]->offset , 29);
			inst_create_sw(get_current_reg(), node->opr[0]->offset , 29);
		}
	}
	if((node->opr[0]->nature == NODE_IDENT 
	&& node->opr[0]->type == TYPE_INT)
	&& node->opr[1]->nature == NODE_INTVAL)
		if(reg_available()){
			inst_create_ori(get_current_reg(), 0, node->opr[1]->value);
			inst_create_sw(get_current_reg(), node->opr[0]->offset , 29);
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
					affect_variable(root->opr[i]);
				
			//instruction determination
			}		
		}
		if(root->opr[i] != NULL){
			gen_code_passe_2(root->opr[i]);
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
		if(root->nature == NODE_FOR
		|| root->nature == NODE_WHILE){
			if (parsingLoop == 0){
				label++;
				inst_create_label(label);
				parsingLoop++;
			}
			else if (parsingLoop == root->nops-1){
				inst_create_j(label);
				label++;
				inst_create_label(label);
				parsingLoop = 0;
			}
			else{
				parsingLoop++;
			}	
		}
	}
}

 