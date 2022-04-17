
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
			if(node->opr[0]->type != TYPE_INT){
				printf("Error line %d: operator '+' cannot have left operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
			else if(node->opr[1]->type != TYPE_INT){
				printf("Error line %d: operator '+' cannot have right operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
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
			if(node->opr[0]->type != TYPE_INT){
				printf("Error line %d: operator '-' cannot have left operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
			else if(node->opr[1]->type != TYPE_INT){
				printf("Error line %d: operator '-' cannot have right operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
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
			if(node->opr[0]->type != TYPE_INT){
				printf("Error line %d: operator '*' cannot have left operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
			else if(node->opr[1]->type != TYPE_INT){
				printf("Error line %d: operator '*' cannot have right operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
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
			if(node->opr[0]->type != TYPE_INT){
				printf("Error line %d: operator '/' cannot have left operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
			else if(node->opr[1]->type != TYPE_INT){
				printf("Error line %d: operator '/' cannot have right operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
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
			if(node->opr[0]->type != TYPE_INT){
				printf("Error line %d: operator '%%' cannot have left operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
			else if(node->opr[1]->type != TYPE_INT){
				printf("Error line %d: operator '%%' cannot have right operand of type TYPE BOOL\n", node->opr[0]->lineno);
			}
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

// check that IF, DOWHILE, WHILE and FOR gives boolean type expression
void check_bool_expr(node_t node, int positionnal){
	if (node->opr[positionnal]->type != TYPE_BOOL && node->opr[positionnal]->type != TYPE_NONE){
		if (node->nature == NODE_IF){
			printf("Error line %d: 'if' condition does not have a boolean type\n", node->opr[0]->lineno);
			exit(EXIT_FAILURE);
		}
		else if (node->nature == NODE_WHILE){
			printf("Error line %d: 'while' condition does not have a boolean type\n", node->opr[0]->lineno);
			exit(EXIT_FAILURE);
		}
		else if (node->nature == NODE_DOWHILE){
			printf("Error line %d: 'do ... while' condition does not have a boolean type\n", node->opr[0]->lineno);
			exit(EXIT_FAILURE);
		}
		else if (node->nature == NODE_FOR){
			printf("Error line %d: 'for' condition does not have a boolean type\n", node->opr[0]->lineno);
			exit(EXIT_FAILURE);
		}
	}
}

// check that boolean pperations are between 2 same types
void check_bool_op(node_t node){
	if (node->opr[0]->type && node->opr[1]->type){
		if (node->opr[0]->type != node->opr[1]->type){	
			printf("Error line %d: two arguments must be the same type\n", node->opr[0]->lineno);
			exit(EXIT_FAILURE);
		}
	}
}

// check that affect are same types
void check_affect_type(node_t node){
	if (node->opr[1] != NULL){
		if (node->opr[0]->type && node->opr[1]->type){
			if (node->opr[0]->type != node->opr[1]->type){	
				printf("Error line %d: cannot affect a variable to a type which different from the declared one\n", node->opr[0]->lineno);
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
				printf("Error line %d: global variables can only be initialized with a constant value\n", node->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
		}
	}
}


void check_intval_domain(node_t node){
	if(node->value > 0xffffffff7fffffff){
		printf("Error line %d: integer out of range\n", node->lineno);
		exit(EXIT_FAILURE);
	}
}


/* --------------- Tree parsing --------------- */

void analyse_passe_1(node_t root) {	
	node_t variableDecl;
	if (root->nature == NODE_PROGRAM){
		push_global_context();
		isGlobal=1; //flag to update the global_decl attribute
	}	

	// parsing the tree}
	for(int i = 0; i < root->nops; i++){
		if(root->opr[i] != NULL){
			switch(root->opr[i]->nature){
				case NODE_TYPE :
					if (root->opr[i]->type != 0){
						current_type = root->opr[i]->type;
					}
					break;
				case NODE_INTVAL :
					check_intval_domain(root->opr[i]);
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
					}
					else {
						// Else we get the adress of declaration and associate it with the current variable
						if (variableDecl != NULL){
							root->opr[i]->offset = variableDecl->offset;
							root->opr[i]->type = variableDecl->type;
						}
						else {
							printf("Error line %d : undeclared variable '%s'\n", root->opr[i]->lineno, root->opr[i]->ident);
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
				case NODE_BNOT :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_BAND :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_BOR :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_BXOR :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_SRL :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_SRA :
					root->opr[i]->type = TYPE_INT;
					break;
				case NODE_SLL :
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
		//printf("type neoud : %d\n", root->type);
		// get offset of the function
		if(root->nature == NODE_FUNC){
			root->offset = get_env_current_offset();
			if(root->opr[0]->type != TYPE_VOID){
				printf("Error line %d: 'main()' declaration must have a 'void' return type\n", root->opr[0]->lineno);
				exit(EXIT_FAILURE);
			}
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
		if(	root->nature == NODE_DOWHILE ||
			root->nature == NODE_FOR){
			check_bool_expr(root, 1);
			//the do_while bool expression is the second son
		}
		if (root->nature == NODE_WHILE ||
			root->nature == NODE_IF ){
			check_bool_expr(root, 0);
			//the if & while bool expression is the first son
		}
		// check the coherence of type affectation
		if(root->nature == NODE_AFFECT){
			check_affect_type(root);
		}
		if(root->nature == NODE_DECLS){
			if (root->opr[0]->type && root->opr[1]->type){
				if (root->opr[0]->type != root->opr[1]->type){
					printf("Error line %d: variable already declared\n", root->lineno);
					exit(EXIT_FAILURE);
				}	
			}
		}
		if(root->nature == NODE_DECL ){
			// check the global declaration : not an expression and check the coherence of the types
			root->type = root->opr[0]->type;
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
			root->nature == NODE_OR ){
			check_bool_op(root);
		}	
	}
}
