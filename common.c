
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>

#include "defs.h"
#include "common.h"
#include "arch.h"

#define RED "\e[0;31m"
#define NC "\e[0m"
extern char * infile;
extern char * outfile;
int32_t trace_level = DEFAULT_TRACE_LEVEL;
extern bool stop_after_syntax;
extern bool stop_after_verif;
bool incomp=0;

void affiche_help(){
	printf("  OPTIONS helper 4 minicc \n\n");
	printf("  Utilisation : ./minicc_ref <options> <infile>\n\n");
	printf("  -b : affiche une banière indiquant le nom du compilateur \n          et des membres du groupe \n");
	printf("  -o <filename> : Définit le nom du fichier assembleur produit \n          (défaut : out.s)\n");
	printf("  -t <int> : Définit le niveau de trace à utiliser entre 0 et 5 \n          (0 = pas de trace ; 5 = toutes les traces defaut = 0)\n");
	printf("  -r <int> : Définit le nombre maximum de registres à utiliser, \n          entre 4 et 8 (défaut : 8)\n");
	printf("  -s : Arrêter la compilation après l’analyse syntaxique \n          (défaut = non)\n");
	printf("  -v : Arrêter la compilation après la passe de vérifications \n          (défaut = non)\n");
	printf("  -h : Afficher la liste des options (fonction d’usage) \n          et arrêter le parsing des arguments\n\n");
}	

void affiche_membres(){
	printf("oooo     oooo ooooo oooo   oooo ooooo        oooooooo8   oooooooo8 \n 8888o   888   888   8888o  88   888       o888     88 o888     88 \n 88 888o8 88   888   88 888o88   888       888         888         \n 88  888  88   888   88   8888   888       888o     oo 888o     oo \no88o  8  o88o o888o o88o    88  o888o       888oooo88   888oooo88  \n                                                              \nKKKKKKKKKKKKKKKKKKKKKKKKKK0KKKKKKKKKKKKKK0000000000000000000000000000KKKKKK\nKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK000000000000000000000000000KKKKKKKK\nKKKKKKKKKKKKXXXXXXKKKKKKKKKKKKKKKKKKK000KKKK00000000000000000000KKKKKKKKKKK\n00KKKKKKXXXXXXXXK0000OO00KKKKK000000KKKKKKKKKKKK0KK000000KKKKKKKKKKKKKKKKXX\ndxxkOO0KKXXXXXX0xl:::;:loxkOOOkxdxkO00KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKXXXXXX\nxddxddodOKKXK0xl;'''''''',::::::::ldxxxk00KKKKKKKKKKKKKKKKKKXXXXXXXXXXXXXNN\nxdxdoxk0KXXKxc,'.....'..........'',;:clodk00KKKKKKKKKKKKXXXXXXXXXXXXXXXXXNN\nxooxk0KKKKXOc'.............  ..''..,;;:loxOO0KXXKXXXXXXXXXXXXXXXXXNNNNNNNNN\ndxk0KXKkcckd,..'..............'','.';::cldkkO0KXXXXXXXXXXXNNNNNNNNNNNNNNWWW\nOKKXXX0d,.....'..,;;;;;;;;,,,,,,,;,',:;;:lxkO0KK00KKXXXXXXNNNNNNNWWWWWWWWWW\nXXXXXX0kl'....';ccccccccc::::;;,,,,''''.';clxxollodxxxkO0XNNNNWWWWWWMMMMWWX\nXXXXXXXOl'...,cllllllllcccccc:;;;;,'......,cdc....,''',;cdkKNWWWMMMMMMWXOkx\nXXXXXXKk:....:lc::;;;::cccc::::;;;;;,,,'.';cl,.   ......,;:cxKWMMMNXK0000Ok\nXXXXNX0o,...:lc:;,'...',;;;;;,,,,,;:ldo;';cc'.....,;:::;;,,;ckXXKKKKXNWMMNO\nXXXXXX0l'..;llcc:;,'..',,;;,......';dOd;,;:'..';:cccccccccc::lOXNWMMMMMMMMX\nXXXXXNOc,.,lllc:,......;:c;'...'''''cxl,,:;..,::cccccc:;::loolOWWKKWMMMMMMW\nXXXNNNxc:,colllc:;,',,,;:c;,.....',,coc,;cc,'';::::c:,''',;cddkX0dxXMMMMMMM\nXXXNNNk:;codolcccc::c:;:cc:,'''..,:cldc;cl:::;,''';cc:,'',:cldkkdodKMMMMMMM\nXNNNNN0lcodoolccc:::;;:cccl:,;;;;:oxxdc;oo,;c;'...,cllccccllloxxolo0WMMMMWX\nXXNNNNNkooooooolc:;,,;;;;,;:;,;::cldoc:oOkl;cc;,,,::;;;;:ccllldxoloOWMMMNOx\nXXXXNNNXOdolooolc:;;;,'''...'',;:lddc;lKWXOdllllcc:,',,;:cccllodoloONMWXkdo\nKKKXXXXXKxlloolc::;;;,'.....',,;codoxOKWWWKOxoollc::;,',,,;:clooolldOK0kdx0\nKKKKKKKK0xlooolc:;,'''......',,;lk0KNWWMMMNO0Odlllc:;,'',;:::coollllodddxKW\nXXXXXKKKKkooool:;;;,,'.......';:dKWMMMMMMMWKOXKOdlc:;;;;;;;;;coolllloooodk0\nKKKXXXXXXOooolc:::::;,'.''''',;o0WMMMMMMMMMN0KWWXOo:;,,'''.'';lllllllooolll\n00KKXXKKOdolllc;;::;;,,''''',;lONMMMMMMMMMMWK0XXXKxl:;,'''',,:lllllllolllll\n0KKKXXKkdooll::;;;::;;,,,,,',oOKNMMMMMWNXK0kO0kocloolc:::;;;;coooooolllllll\nddkkkkxdoolllc;,,,,,'.''',,:o0X0KNWW0kxdolc:lxo,',::cccc:::;;coooooolllllcc\n:cloooooolllccc:,''......';d0K0xdxKN0xxkkOxl:,'....,;::::::;;coooooooollcc:\n;;:cloolllccccc::;;'...',;oOKd;,;lk0XXKK0xc,'........';;::;;;:oooooooollc:;\n,,,;cllllccc:::::;;'.'',,cON0:',;:okOOxl:''............';;;;;:looooollllc:;\n\nProject by Rio & Genty \n");
}

void testInt(int min, int max, int test){
	if (test <= max && test >= min){}
	else{
		fprintf(stderr,RED "[CRITICAL ERROR]\n");
		fprintf(stderr,NC"option non compatible avec %d \n",test);
		exit(1);	
	}
}
void testStr(char * test, char ASMouC){
	int i=0;
	while (test[i] != '\0'){
		if ((test[i] <= 'z' && test[i] >= '0') || (test[i] == '.' && test[i+1] == ASMouC )){}
		else{
			fprintf(stderr,RED "[CRITICAL ERROR]\n");
			fprintf(stderr,NC"option non compatible avec %s \n",test);
			exit(1);	
			}
		i++;
	}
}

void argIncompatible(char *UN, char *DEUX, char *test){
	if((strcmp(test,UN) || strcmp(test,DEUX)) && incomp == 0){
		incomp =1;
	}
	else if((strcmp(test,UN) || strcmp(test,DEUX)) && incomp == 1){
		fprintf(stderr,RED "[CRITICAL ERROR]\n");
		fprintf(stderr,NC"arguments %s et %s incompatibles \n",UN,DEUX);
		exit(1);
	}
}

void parse_args(int argc, char ** argv) {
	infile = NULL;
    for (int i=1; i<argc; i++){
    	if (argv[i][0] != '-'){
    		testStr(argv[i],'c');
    		infile = argv[i];
    		printf("le nom du fichier sur lequel effectuer la compilation %s \n",argv[i]);
    		//break;
    	}
    	else{
			switch (argv[i][1]){
				case 'h':
					affiche_help();
					exit(1);
					break;
				case 'b':
					affiche_membres();
					exit(1);
					break;
				case 'o':
					//TESTS à faire !!! si pas les bons caractères
					testStr(argv[i+1],'s');
					outfile = argv[i+1];
					printf("le nom du fichier asm produit est %s \n", outfile);
					i++;
					break;
				case 't':
					//TESTS à faire !!! si pas les bons caractères
					testInt(0,5,atoi(argv[i+1]));
					trace_level = atoi(argv[i+1]);
					printf("le trace level est fixé à %d \n",trace_level);
					i++;
					break;
				case 'r':
					//TESTS à faire !!! si pas les bons caractères
					testInt(4,8,atoi(argv[i+1]));
					set_max_registers(atoi(argv[i+1]));
					printf("le nombre de registres est fixé à %d \n",atoi(argv[i+1]));
					i++;
					break;
				case 's':
					argIncompatible("-s","-v",argv[i]);
					stop_after_syntax = 1;
					printf("arrêt après la phase d'analyse syntaxique \n");
					break;
				case 'v':
					argIncompatible("-s","-v",argv[i]);
					stop_after_verif = 1;
					printf("arrêt après la phase de vérification \n");
					break;
				default:
					printf("%s ",argv[i]);
					fprintf(stderr,RED "[CRITICAL ERROR]\n");
					fprintf(stderr,NC "n'est pas une option de minicc \nannulation en cours \n\n");
					affiche_help();
					exit(1);
					break;
			}
		}
    }
	if (infile == NULL){
		fprintf(stderr,RED "[CRITICAL ERROR]\n");
		fprintf(stderr,NC "pas de fichier .c dans la commande \n\n");
		affiche_help();
		exit(1);
	}
}


void free_nodes(node_t n) {
    // A implementer
}


char * strdupl(char * s) {
    char * r = malloc(strlen(s) + 1);
    strcpy(r, s);
    return r;
}


static int32_t dump_tree2dot_rec(FILE * f, node_t n, int32_t node_num) {

    if (n == NULL) {
        fprintf(f, "    N%d [shape=record, label=\"{{NULL}}\"];\n", node_num);
        return node_num;
    }

    switch (n->nature) {
        case NODE_IDENT:
            {
                node_t decl_node = n->decl_node;
                fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}|{<decl>Decl      |Ident: %s|Offset: %d}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), n->ident, n->offset);
                if (decl_node != NULL && decl_node != n) {
                    fprintf(f, "    edge[tailclip=false];\n");
                    fprintf(f, "    \"N%d\":decl:c -> \"N%d\" [style=dashed]\n", node_num, decl_node->node_num);
                }
                break;
            }
        case NODE_INTVAL:
        case NODE_BOOLVAL:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}|{Value: %" PRId64 "}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), n->value);
            break;
        case NODE_STRINGVAL:
            {
                char str[32];
                int32_t i = 1;
                while (true) {
                    str[i - 1] = n->str[i];
                    i += 1;
                    if (n->str[i] == '"') {
                        str[i - 1] = '\0';
                        break;
                    }
                }
                fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}|{val: %s}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), str);
            }
            break;
        case NODE_TYPE:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type));
            break;
        case NODE_LIST:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE LIST}}\"];\n", node_num);
            break;
        case NODE_PROGRAM:
        case NODE_BLOCK:
        case NODE_DECLS:
        case NODE_DECL:
        case NODE_IF:
        case NODE_WHILE:
        case NODE_FOR:
        case NODE_DOWHILE:
        case NODE_PRINT:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Nb. ops: %d}}\"];\n", node_num, node_nature2string(n->nature), n->nops);
            break;
        case NODE_FUNC:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Nb. ops: %d}|{offset: %d}}\"];\n", node_num, node_nature2string(n->nature), n->nops, n->offset);
            break;
        case NODE_PLUS:
        case NODE_MINUS:
        case NODE_MUL:
        case NODE_DIV:
        case NODE_MOD:
        case NODE_LT:
        case NODE_GT:
        case NODE_LE:
        case NODE_GE:
        case NODE_EQ:
        case NODE_NE:
        case NODE_AND:
        case NODE_OR:
        case NODE_BAND:
        case NODE_BOR:
        case NODE_BXOR:
        case NODE_SRA:
        case NODE_SRL:
        case NODE_SLL:
        case NODE_NOT:
        case NODE_BNOT:
        case NODE_UMINUS:
        case NODE_AFFECT:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s|Nb. ops: %d}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), n->nops);
            break;
        default:
            printf("*** Error in %s: unknow nature : %s\n", __func__, node_nature2string(n->nature));
            assert(false);
    }

    n->node_num = node_num;

    int32_t curr_node_num = node_num + 1;
    for (int32_t i = 0; i < n->nops; i += 1) {
        int32_t new_node_num = dump_tree2dot_rec(f, n->opr[i], curr_node_num);

        fprintf(f, "    edge[tailclip=true];\n");
        fprintf(f, "    N%d -> N%d\n", node_num, curr_node_num);
        curr_node_num = new_node_num + 1;
    }

    return curr_node_num - 1;
}



static void dump_tree2dot(FILE * f, node_t root) {
    assert(root->nature == NODE_PROGRAM);

    int32_t curr_node_num = 1;
    dump_tree2dot_rec(f, root, curr_node_num);
}


void dump_tree(node_t prog_root, const char * dotname) {

    FILE * f;

    f = fopen(dotname, "w");
    fprintf(f, "digraph global_vars {\n");
    dump_tree2dot(f, prog_root);
    fprintf(f, "}");    
    fclose(f);
}


const char * node_type2string(node_type t) {
    switch (t) {
        case TYPE_NONE:
            return "TYPE NONE";
        case TYPE_INT:
            return "TYPE INT";
        case TYPE_BOOL:
            return "TYPE BOOL";
        case TYPE_VOID:
            return "TYPE VOID";
        default:
            assert(false);
    }
}


const char * node_nature2string(node_nature t) {
    switch (t) {
        case NONE:
            return "NONE";
        case NODE_PROGRAM:
            return "PROGRAM";
        case NODE_BLOCK:
            return "BLOCK";
        case NODE_LIST:
            return "LIST";
        case NODE_DECLS:
            return "DECLS";
        case NODE_DECL:
            return "DECL";
        case NODE_IDENT:
            return "IDENT";
        case NODE_TYPE:
            return "TYPE";
        case NODE_INTVAL:
            return "INTVAL";
        case NODE_BOOLVAL:
            return "BOOLVAL";
        case NODE_STRINGVAL:
            return "STRINGVAL";
        case NODE_FUNC:
            return "FUNC";
        case NODE_IF:
            return "IF";
        case NODE_WHILE:
            return "WHILE";
        case NODE_FOR:
            return "FOR";
        case NODE_DOWHILE:
            return "DOWHILE";
        case NODE_PLUS:
            return "PLUS";
        case NODE_MINUS:
            return "MINUS";
        case NODE_MUL:
            return "MUL";
        case NODE_DIV:
            return "DIV";
        case NODE_MOD:
            return "MOD";
        case NODE_LT:
            return "LT";
        case NODE_GT:
            return "GT";
        case NODE_LE:
            return "LE";
        case NODE_GE:
            return "GE";
        case NODE_EQ:
            return "EQ";
        case NODE_NE:
            return "NE";
        case NODE_AND:
            return "AND";
        case NODE_OR:
            return "OR";
        case NODE_BAND:
            return "BAND";
        case NODE_BOR:
            return "BOR";
        case NODE_BXOR:
            return "BXOR";
        case NODE_SLL:
            return "SLL";
        case NODE_SRA:
            return "SRA";
        case NODE_SRL:
            return "SRL";
        case NODE_NOT:
            return "NOT";
        case NODE_BNOT:
            return "BNOT";
        case NODE_UMINUS:
            return "UMINUS";
        case NODE_AFFECT:
            return "AFFECT";
        case NODE_PRINT:
            return "PRINT";
        default:
            fprintf(stderr, "*** Error in %s: Unknown node nature: %d\n", __func__, t);
            exit(1);
    }
}




const char * node_nature2symb(node_nature t) {
    switch (t) {
        case NONE:
        case NODE_PLUS:
            return "+";
        case NODE_MINUS:
            return "-";
        case NODE_MUL:
            return "*";
        case NODE_DIV:
            return "/";
        case NODE_MOD:
            return "%";
        case NODE_LT:
            return "<";
        case NODE_GT:
            return ">";
        case NODE_LE:
            return "<=";
        case NODE_GE:
            return ">=";
        case NODE_EQ:
            return "==";
        case NODE_NE:
            return "!=";
        case NODE_AND:
            return "&&";
        case NODE_OR:
            return "||";
        case NODE_BAND:
            return "&";
        case NODE_BOR:
            return "|";
        case NODE_BXOR:
            return "^";
        case NODE_SRA:
            return ">>";
        case NODE_SRL:
            return ">>>";
        case NODE_SLL:
            return "<<";
        case NODE_NOT:
            return "!";
        case NODE_BNOT:
            return "~";
        case NODE_UMINUS:
            return "-";
        default:
            fprintf(stderr, "*** Error in %s: Unknown node nature: %d\n", __func__, t);
            exit(1);
    }
}




