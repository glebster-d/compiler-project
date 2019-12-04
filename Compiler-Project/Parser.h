#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Scope.h"


extern Token *current_token;					/* Holds current token */
extern FILE *out_syntactic, *out_semantic;		/* Pointers for output data */


/*
*	This structure holds Follow or Expected sets
*/
typedef struct set
{	
	eTOKENS *set;
	int size;	

} Follow, Expected;



void parser();	
void parse_PROGRAM();	
Attr_type parse_STATEMENTS(Mode);
Attr_type parse_STATEMENTS_NEW(Mode);
void parse_FUNC_DEFINITIONS();
void parse_FUNC_DEFINITIONS_NEW();
long parse_VAR_DEFINITION(Mode);
long parse_VAR_DEFINITIONS(Mode);
long parse_VAR_DEFINITIONS_NEW(Mode);
Attr_type parse_TYPE();
long parse_VARIABLES_LIST(Attr_type, Mode);
long parse_VARIABLES_LIST_NEW(Attr_type, Mode);
long parse_VARIABLE(Attr_type, Mode);
long parse_VARIABLE_NEW(Entry *, Mode);
void parse_FUNC_DEFINITION();
Attr_type parse_RETURNED_TYPE();
long parse_PARAM_DEFINITIONS();
Attr_type parse_BLOCK(Mode);
Attr_type parse_STATEMENT(Mode);
Attr_type parse_STATEMENT_NEW_1();
void parse_STATEMENT_NEW_2(Entry *, Mode);
Attr_type parse_EXPRESSION();
Attr_type parse_EXPRESSION_NEW(Entry *);
void parse_PARAMETERS_LIST();

int match(eTOKENS);

void hold_error(Follow, Expected);
void print_parsing_error(Expected);
void print_rule(char *);
void open_file();
void make_recovery(Follow);
int in_follow_set(Follow, eTOKENS);

char* enum_to_string(eTOKENS);
void add_param_to_function(Entry *, String, Attr_type);
void print_semantic_error(Error, int, String);
void free_funct_params(Entry *);
void print_final_error_message();

extern void print_error(FILE *file, char *str);

#endif // PARSER_H