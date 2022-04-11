
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "defs.h"
#include "passe_1.h"
#include "utils/miniccutils.h"

extern int trace_level;

// global variables
int isGlobal = 1;
node_type current_type= 0;
int declaration = 0;

/* --------------- Check-Up functions --------------- */

// check that operations have 2 same types
void check_add_type(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){	

		if (node->opr[0]->type != TYPE_INT || node->opr[1]->type != TYPE_INT){	
			errno = EINVAL;
			printf("ERROR on line %d \n", node->opr[0]->lineno);
			perror("MiniCC can only sum 2 int");
			exit(EXIT_FAILURE);
		}	
	}
	else if (node->opr[0]->nature == NODE_PLUS){
		check_add_type(node->opr[0]);
	}
	else if (node->opr[1]->nature == NODE_PLUS){
		check_add_type(node->opr[1]);
	}
}

void check_minus_type(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){

		if (node->opr[0]->type != TYPE_INT || node->opr[1]->type != TYPE_INT){	
			errno = EINVAL;
			printf("ERROR on line %d \n", node->opr[0]->lineno);
			perror("MiniCC can only substract 2 int");
			exit(EXIT_FAILURE);
		}	
	}
	else if (node->opr[0]->nature == NODE_MINUS){
		check_minus_type(node->opr[0]);
	}
	else if (node->opr[1]->nature == NODE_MINUS){
		check_minus_type(node->opr[1]);
	}
}

void check_mul_type(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){		
		if (node->opr[0]->type != TYPE_INT || node->opr[1]->type != TYPE_INT){	
			errno = EINVAL;
			printf("ERROR on line %d \n", node->opr[0]->lineno);
			perror("MiniCC can only multiply 2 int");
			exit(EXIT_FAILURE);
		}	
	}
	else if (node->opr[0]->nature == NODE_MUL){
		check_mul_type(node->opr[0]);
	}
	else if (node->opr[1]->nature == NODE_MUL){
		check_mul_type(node->opr[1]);
	}
}

void check_div_type(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){		
		if (node->opr[0]->type != TYPE_INT || node->opr[1]->type != TYPE_INT){	
			errno = EINVAL;
			printf("ERROR on line %d \n", node->opr[0]->lineno);
			perror("MiniCC can only divide 2 int");
			exit(EXIT_FAILURE);
		}	
	}
	else if (node->opr[0]->nature == NODE_DIV){
		check_div_type(node->opr[0]);
	}
	else if (node->opr[1]->nature == NODE_DIV){
		check_div_type(node->opr[1]);
	}
}

void check_mod_type(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){		
		if (node->opr[0]->type != TYPE_INT || node->opr[1]->type != TYPE_INT){	
			errno = EINVAL;
			printf("ERROR on line %d \n", node->opr[0]->lineno);
			perror("MiniCC can only modulo divide 2 int");
			exit(EXIT_FAILURE);
		}	
	}
	else if (node->opr[0]->nature == NODE_MOD){
		check_div_type(node->opr[0]);
	}
	else if (node->opr[1]->nature == NODE_MOD){
		check_div_type(node->opr[1]);
	}
}

// check that IF and WHILE 
void check_bool_expr(node_t node){
	if (node->opr[0]->type != TYPE_BOOL){
		errno = EINVAL;
		printf("ERROR on line %d \n", node->opr[0]->lineno);
		perror("Argument of IF and WHILE statement must be boolean");
		exit(EXIT_FAILURE);
	}
}

// check that boolean pperations are between 2 same types
void check_bool_op(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){
		if (node->opr[0]->type != node->opr[1]->type){	
			errno = EINVAL;
			printf("ERROR on line %d \n", node->opr[0]->lineno);
			perror("Two arguments must be the same type");
			exit(EXIT_FAILURE);
		}
	}
}

// check that affect are same types
void check_affect_type(node_t node){
	if (node->opr[1] != NULL){
		if (node->opr[0]->type && node->opr[1]->type){
			if (node->opr[0]->type != node->opr[1]->type){	
				errno = EINVAL;
				printf("ERROR on line %d \n", node->opr[0]->lineno);
				perror("Can't affect a variable to a type which different from the declared one");
				exit(EXIT_FAILURE);
			}
			else{
				node->type = node->opr[0]->type;
			}
		}
	}
}

// check that global declaration are not expression
void check_global_decl(node_t node){
	if (node->opr[1] != NULL){
		if (node->opr[0]->global_decl){
			if ((node->opr[1]->nature == NODE_PLUS) 
				|| (node->opr[1]->nature == NODE_MINUS) 
				|| (node->opr[1]->nature == NODE_DIV) 
				|| (node->opr[1]->nature == NODE_MUL)){
				errno = EINVAL;
				printf("ERROR on line %d \n", node->opr[0]->lineno);
				perror("Can't affect a global variable with an expression");
				exit(EXIT_FAILURE);
			}
		}
	}
}



/* --------------- Tree parsing --------------- */

void analyse_passe_1(node_t root) {	
	node_t variableDecl;
	if (root->nature == NODE_PROGRAM){
		push_global_context();
		isGlobal=1; //flag to update the global_decl attribute
	}	

	// parsing the tree
	for(int i = 0; i < root->nops; i++){
		if(root->opr[i] != NULL){
			//printf("Nature node : %d\n", root->opr[i]->nature);
			switch(root->opr[i]->nature){
				case NODE_TYPE :
					if (root->opr[i]->type != 0){
						current_type = root->opr[i]->type;
					}
					break;
				case NODE_IDENT :
					// On verifie si la variable a déjà été déclarée
					variableDecl = (node_t) get_decl_node(root->opr[i]->ident);

					// If ident == 'main' => setup the type to void and jump to the next node
					if (!(strcmp(root->opr[i]->ident, "main"))){
						root->opr[i]->type = TYPE_VOID;
						break;
					}

					// Offset update
					if (variableDecl == NULL && declaration){
						//If undeclared, we add it to the environnement
						root->opr[i]->offset = env_add_element(root->opr[i]->ident, root->opr[i]);
						root->opr[i]->type = current_type;
						//current_type = TYPE_NONE;
					}
					else {
						// Else we get the adress of declaration and associate it with the current variable
						if (variableDecl != NULL){
							root->opr[i]->offset = variableDecl->offset;
							root->opr[i]->type = variableDecl->type;
						}
						else{
							errno = EINVAL;
							printf("ERROR on line %d \n", root->opr[i]->lineno);
							perror("Undeclared variable");
							exit(EXIT_FAILURE);
						}
					}

					// Update of the global_decl
					if(isGlobal){
						root->opr[i]->global_decl = true;
					}
					else{
						root->opr[i]->global_decl = false;
					}

					// Update of decl_node
					root->opr[i]->decl_node = get_decl_node(root->opr[i]->ident);
					break;

					
				// Operators that returns int expressions
				case NODE_PLUS :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_MINUS :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_UMINUS :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_DIV :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_MUL :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_MOD :
					root->opr[i]->type = TYPE_INT;
					break;

				// Opreators that returns boolean expressions
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
				case NODE_AND :
					root->opr[i]->type = TYPE_BOOL;
					break;
				case NODE_OR :
					root->opr[i]->type = TYPE_BOOL;
					break;
				case NODE_NOT :
					root->opr[i]->type = TYPE_BOOL;
					break;

				case NODE_DECL :
					declaration = 1;
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

		//Recursion 
		if(root->opr[i] != NULL){
			analyse_passe_1(root->opr[i]);
		}
		// get offset of the function
		if(root->nature == NODE_FUNC){
			root->offset = get_env_current_offset();
		}
		// check that operations are between INT
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
		if(root->nature == NODE_MOD){
			check_mod_type(root);
		}
		// check that condition returns boolean 
		if(root->nature == NODE_IF){
			check_bool_expr(root);
		}
		if(root->nature == NODE_WHILE){
			check_bool_expr(root);
		}
		if(root->nature == NODE_AFFECT){
			// chek the coherence of type affectation
			check_affect_type(root);
		}
		if(root->nature == NODE_DECL ){
			// check the global declaration : not an expression and check the coherence of the types
			check_global_decl(root);
			check_affect_type(root);
			declaration = 0;
		}

		// check if the type is the same for the two arguments
		if(	root->nature == NODE_EQ ||
			root->nature == NODE_NE ||
			root->nature == NODE_LT ||
			root->nature == NODE_GT ||
			root->nature == NODE_LE ||
			root->nature == NODE_GE ||
			root->nature == NODE_AND ||
			root->nature == NODE_OR){
			check_bool_op(root);
		}	
	}
}
