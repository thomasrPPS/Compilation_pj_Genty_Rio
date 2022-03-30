
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"
#include "utils/miniccutils.h"

extern int trace_level;

void analyse_passe_1(node_t root) {	
	int current_offset;
	//printf("noeud pointer -> %p\r\n", root);
	for(int i = 0; i < root->nops; i++){
		// printf("nbOps : %i\r\n", root->nops);
		// printf("fils 1 pointer -> %p | fils 2 pointer -> %p\r\n", root->opr[0], root->opr[1]);
		if(root->opr[i] != NULL){
			printf("Nature Noeud : %d\n", root->opr[i]->nature);
			switch(root->opr[i]->nature){
				case NODE_IDENT :
					printf("IDENT ici\n");
					///printf("Pointeur de Ident : %p\n", get_decl_node(root->opr[i]->ident));
					current_offset = env_add_element(root->opr[i]->ident, root->opr[i]);
					printf("Offset normalement : %d\n", current_offset);
					if (current_offset > 0){
						root->opr[i]->offset = current_offset;
					}
					else{
						printf("La variable %s à déjà été déclarée.\n", root->opr[i]->ident);
						root->opr[i]->offset += 0; 
					}
					printf("Offset reel : %d\n", root->opr[i]->offset);
					break;
	
				case NODE_FUNC :
					printf("FUNC ici\n");
					root->opr[i]->offset = get_env_current_offset();
					break;
	
				case NODE_DECLS :
					push_context();
					break;
	
				case NODE_DECL :
					break;
	
				case NODE_STRINGVAL :
					root->opr[i]->offset = add_string(root->opr[i]->str);
					printf("Offset reel : %d\n", root->opr[i]->offset);
					break;
	
			}
		}
		printf("Fin analyse\n");
		if(root->opr[i] != NULL){
			analyse_passe_1(root->opr[i]);
		}
	}
}
