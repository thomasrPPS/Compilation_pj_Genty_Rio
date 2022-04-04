
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "passe_1.h"
#include "utils/miniccutils.h"

extern int trace_level;
int isGlobal = 1;
node_type current_type= 0;

void analyse_passe_1(node_t root) {	
	int cmpMain;
	node_t variableDecl;
	if (root->nature == NODE_PROGRAM){
		push_global_context();
		printf("push_global_context\n");
		isGlobal=1; //permet de mettre à jour le global decl de chaque Ident
	}	
	


	for(int i = 0; i < root->nops; i++){
		if(root->opr[i] != NULL){
			// On stocke le type s'il est non-nul
			if (root->opr[i]->type != 0){
				current_type = root->opr[i]->type;
			}
			printf("Nature Noeud : %d\n", root->opr[i]->nature);
			switch(root->opr[i]->nature){
				case NODE_IDENT :
					printf("IDENT ici\n");
					// On verifie si la variable a déjà été déclarée
					variableDecl = (node_t) get_decl_node(root->opr[i]->ident);
					printf("%s\n", root->opr[i]->ident);

					// Si l'identifier est 'main', on sort
					if (!(strcmp(root->opr[i]->ident, "main"))){
						break;
					}

					// Maj de l'offset
					if (variableDecl == NULL){
						// Non = on ajoute la variable à l'environnement et on associe son type à l'identificateur
						root->opr[i]->offset = env_add_element(root->opr[i]->ident, root->opr[i]);
						root->opr[i]->type = current_type;
					}
					else{
						// Sinon on recupère l'offset de sa déclaration et son type, qu'on associe à l'identifiant
						root->opr[i]->offset = variableDecl->offset;
						root->opr[i]->type = variableDecl->type;
					}

					// Maj de global_decl
					if(isGlobal){
						root->opr[i]->global_decl = true;
					}
					else{
						root->opr[i]->global_decl = false;
					}

					// Maj de decl_node
					root->opr[i]->decl_node = get_decl_node(root->opr[i]->ident);
					
					// Debug prints 
					printf("Variable déclaré à : %p\n", root->opr[i]->decl_node);
					printf("Le global_decl est après chgmt : %d\n", root->opr[i]->global_decl);
					printf("Le type est après chgmt : %d\n", root->opr[i]->type);
					break;
				
				case NODE_BLOCK :
					isGlobal=0;
					push_context();
					printf("salut jsuis dans block\n");
					break;

				case NODE_FUNC :
					printf("FUNC ici\n");
					reset_env_current_offset();
					break;
	
				case NODE_DECLS :
					printf("DECLS ici\n");
					break;
	
				case NODE_DECL :
					printf("DECL ici\n");
					break;
	
				case NODE_STRINGVAL :
					printf("STRINGVAL ici\n");
					root->opr[i]->offset = add_string(root->opr[i]->str);
					break;
				
	

			}	
		}
		//printf("Fin analyse\n");
		if(root->opr[i] != NULL){
			analyse_passe_1(root->opr[i]);
		}
		if(root->nature == NODE_FUNC){
			root->offset = get_env_current_offset();
			
			printf("allo\n");
		}
		if(root->nature == NODE_FUNC){
			pop_context();
		}
	}
}
