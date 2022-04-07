
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "defs.h"
#include "passe_1.h"
#include "utils/miniccutils.h"

extern int trace_level;
int isGlobal = 1;
node_type current_type= 0;

/*

	- Fonction respect des regles 
		4 operations entre int pas de bool => fait
	- global decl doivent etre des affectations pas d'operations
	Regles Page I-20
	
*/

// exemple : la fonction pour verifier les types est appellée avznt la maj des ident (ici sum est bien maj mais i ne l'est pas ligne 9)

void check_add_type(node_t noeud){
	if (noeud->opr[0]->type && noeud->opr[1]->type){		
		if (noeud->opr[0]->type != noeud->opr[1]->type){	
			errno = EINVAL;
			printf("ERROR on line %d \n", noeud->opr[0]->lineno);
			perror("MiniCC can't do a sum on 2 differents types");
			exit(EXIT_FAILURE);
		}	
	}
	else if (noeud->opr[0]->nature == NODE_PLUS){
		check_add_type(noeud->opr[0]);
	}
	else if (noeud->opr[1]->nature == NODE_PLUS){
		check_add_type(noeud->opr[1]);
	}
	noeud->type = noeud->opr[0]->type;
}

void check_minus_type(node_t noeud){
	if (noeud->opr[0]->type && noeud->opr[1]->type){		
		if (noeud->opr[0]->type != noeud->opr[1]->type){	
			errno = EINVAL;
			printf("ERROR on line %d \n", noeud->opr[0]->lineno);
			perror("MiniCC can't do a substraction on 2 differents types");
			exit(EXIT_FAILURE);
		}	
	}
	else if (noeud->opr[0]->nature == NODE_MINUS){
		check_minus_type(noeud->opr[0]);
	}
	else if (noeud->opr[1]->nature == NODE_MINUS){
		check_minus_type(noeud->opr[1]);
	}
	noeud->type = noeud->opr[0]->type;
}

void check_mul_type(node_t noeud){
	if (noeud->opr[0]->type && noeud->opr[1]->type){		
		if (noeud->opr[0]->type != noeud->opr[1]->type){	
			errno = EINVAL;
			printf("ERROR on line %d \n", noeud->opr[0]->lineno);
			perror("MiniCC can't do a multiplication on 2 differents types");
			exit(EXIT_FAILURE);
		}	
	}
	else if (noeud->opr[0]->nature == NODE_MUL){
		check_mul_type(noeud->opr[0]);
	}
	else if (noeud->opr[1]->nature == NODE_MUL){
		check_mul_type(noeud->opr[1]);
	}
	noeud->type = noeud->opr[0]->type;
}

void check_div_type(node_t noeud){
	if (noeud->opr[0]->type && noeud->opr[1]->type){		
		if (noeud->opr[0]->type != noeud->opr[1]->type){	
			errno = EINVAL;
			printf("ERROR on line %d \n", noeud->opr[0]->lineno);
			perror("MiniCC can't do a division on 2 differents types");
			exit(EXIT_FAILURE);
		}	
	}
	else if (noeud->opr[0]->nature == NODE_DIV){
		check_div_type(noeud->opr[0]);
	}
	else if (noeud->opr[1]->nature == NODE_DIV){
		check_div_type(noeud->opr[1]);
	}
	noeud->type = noeud->opr[0]->type;
}


void check_bool_expr(node_t noeud){
	if (noeud->opr[0]->type != TYPE_BOOL){
		errno = EINVAL;
		printf("ERROR on line %d \n", noeud->opr[0]->lineno);
		perror("Argument of IF statement must be boolean");
		exit(EXIT_FAILURE);
	}
}

void check_affect_type(node_t noeud){
	node_type ARG1, ARG2;
	ARG1 = noeud->opr[0]->type;
	ARG2 = noeud->opr[1]->type;
	if (noeud->opr[0]->type && noeud->opr[1]->type){		
		if (noeud->opr[0]->type != noeud->opr[1]->type){	
			errno = EINVAL;
			printf("ERROR on line %d \n", noeud->opr[0]->lineno);
			perror("Can't affect a variable to a type which different from the declared one");
			exit(EXIT_FAILURE);
		}
	}
}

void check_global_decl(node_t noeud){
	if (noeud->opr[0]->global_decl){
		if ((noeud->opr[1]->nature == NODE_PLUS) 
			|| (noeud->opr[1]->nature == NODE_MINUS) 
			|| (noeud->opr[1]->nature == NODE_DIV) 
			|| (noeud->opr[1]->nature == NODE_MUL)){
			errno = EINVAL;
			printf("ERROR on line %d \n", noeud->opr[0]->lineno);
			perror("Can't affect a global variable with an expression");
			exit(EXIT_FAILURE);
		}
	}
}



void analyse_passe_1(node_t root) {	
	node_t variableDecl;
	if (root->nature == NODE_PROGRAM){
		push_global_context();
		isGlobal=1; //permet de mettre à jour le global decl de chaque Ident
	}	
	


	for(int i = 0; i < root->nops; i++){
		if(root->opr[i] != NULL){
			// On stocke le type s'il est non-nul
			if (root->opr[i]->type != 0){
				current_type = root->opr[i]->type;
			}
			switch(root->opr[i]->nature){
				case NODE_IDENT :
					// On verifie si la variable a déjà été déclarée
					variableDecl = (node_t) get_decl_node(root->opr[i]->ident);
					// Si l'identifier est 'main', on l'ignore
					if (!(strcmp(root->opr[i]->ident, "main"))){
						// On met bien le main au type void
						root->opr[i]->type = TYPE_VOID;
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
					break;
				
				case NODE_EQ :
					root->opr[i]->type = TYPE_BOOL;
					break;


				case NODE_NE :
					root->opr[i]->type = TYPE_BOOL;
					break;

				case NODE_LT :
					root->opr[i]->type = TYPE_BOOL;
					break;

				case NODE_GT :
					root->opr[i]->type = TYPE_BOOL;
					break;

				case NODE_LE : 
					root->opr[i]->type = TYPE_BOOL;
					break;

				case NODE_GE :
					root->opr[i]->type = TYPE_BOOL;
					break;

				case NODE_BLOCK :
					isGlobal=0;
					push_context();
					break;

				case NODE_FUNC :
					reset_env_current_offset();
					break;
	
				case NODE_STRINGVAL :
					root->opr[i]->offset = add_string(root->opr[i]->str);
					break;
			}	
		}
		if(root->opr[i] != NULL){
			analyse_passe_1(root->opr[i]);
		}
		if(root->nature == NODE_FUNC){
			root->offset = get_env_current_offset();
		}
		if(root->nature == NODE_PLUS){
			check_add_type(root);
		}
		if(root->nature == NODE_MINUS){
			check_minus_type(root);
		}
		if(root->nature == NODE_DIV){
			check_div_type(root);
		}
		if(root->nature == NODE_MUL){
			check_mul_type(root);
		}
		if(root->nature == NODE_IF){
			check_bool_expr(root);
		}
		if(root->nature == NODE_WHILE){
			check_bool_expr(root);
		}
		if(root->nature == NODE_AFFECT){
			check_affect_type(root);
		}
		if(root->nature == NODE_DECL){
			check_global_decl(root);
		}	
	}
}
