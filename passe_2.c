
#include <stdio.h>

#include "defs.h"
#include "passe_2.h"
#include "utils/miniccutils.h"

int launch = 1;

void decl_word_variable(node_t node){
	if (node->opr[1] != NULL && node->opr[1] ->value != 0){ 
		inst_create_word(node->opr[0]->ident, node->opr[1]->value);
	}
	else{
		inst_create_word(node->opr[0]->ident, 0);

	}
}

void decl_asciiz_string(node_t node){
	printf("string : %s\n", node->str);
	inst_create_asciiz(NULL, node->str);
}



void gen_code_passe_2(node_t root) {
	if (launch){
		inst_create_data_sec();
		launch = 0;
	}
	for(int i = 0; i < root->nops; i++){
		if (root->opr[i] != NULL){
			//printf("nature : %d\n", root->opr[i]->nature);
			switch(root->opr[i]->nature){

			case NODE_DECL :
				if(root->opr[i]->opr[0]->global_decl){
					decl_word_variable(root->opr[i]);
				}
				break;

			case NODE_FUNC :
				inst_create_text_sec();
				inst_create_label_str(".main");
				break;		
			
			case NODE_STRINGVAL : 
				//decl_asciiz_string(root->opr[i]);
				break;
			}		
		}
		if(root->opr[i] != NULL){
			gen_code_passe_2(root->opr[i]);
		}
	}
}

 