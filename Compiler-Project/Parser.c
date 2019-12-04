

#include "Parser.h"


/* 
*  This package describes the parsing functions for recursive descent parsing.
*  Parsing functions are implements the grammar G of an assignment.
*  Each functions with NEW ending is a function that created after eleminating of left factoring or left recursion.
*  In each function empty word in the rule signed as 'epsilon'.
*/

#ifndef STR_MAX_LENGTH
#define STR_MAX_LENGTH 800
#endif // !STR_MAX_LENGTH


Token *current_token;							/* Current token */
FILE *out_syntactic, *out_semantic;				/* Output to file */
char parser_temp_str[STR_MAX_LENGTH];			/* Temporary string */
int numberOfFiles;								/* Number of files to read */
int back_token_called = 0;						/* Counting how much time back_token function was called */
Follow follow;									/* Holds Follow set of current function on error recovery */
Expected expected;								/* Holds the set of expected tokens for current function on error */


Scope *current_scope = NULL;					/* Current working scope */
Entry *f_entry = NULL;							/* Temporary entry for function definition */
long se_error_count = 0;						/* Count the number of semantic errors */



/* 
*	This function starts parsing routine. 
*/
void parser()
{	
	open_file();	
	parse_PROGRAM();
	match(TOKEN_EOF);
	delete_tables(current_scope);
	print_final_error_message();
	fclose(out_syntactic);
	fclose(out_semantic);
}

/*
*	Function for rule PROGRAM -> program VAR_DEFINITIONS ; STATEMENTS end FUNC_DEFINITIONS 
*/
void parse_PROGRAM()
{	
	current_scope = make_table(NULL);	/* Initialising global symbol table */

	current_token = next_token();
	
	switch(current_token->kind)
	{
		case TOKEN_KEYWORD_PROGRAM:
		{
			print_rule("Rule ( PROGRAM -> program VAR_DEFINITIONS ; STATEMENTS end FUNC_DEFINITIONS )");
			parse_VAR_DEFINITIONS(BLOCK_CALL);
			match(TOKEN_SEMICOLON);
			parse_STATEMENTS(BLOCK_CALL);
			match(TOKEN_KEYWORD_END);
			parse_FUNC_DEFINITIONS();
			break; 
		}

		default: 
		{				
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_EOF;			
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			expected.set[0] = TOKEN_KEYWORD_PROGRAM;
			expected.size = 1;
			
			hold_error(follow, expected);			
			break;	
		}
	}

	current_scope = pop_table(current_scope);
}

/* 
*	Function for rule VAR_DEFINITIONS -> VAR_DEFINITION VAR_DEFINITIONS_NEW 
*/
long parse_VAR_DEFINITIONS(Mode mode)
{
	long count = 0;		/* Number of variables */

	current_token = next_token();

	switch(current_token->kind)
	{
		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		{
			print_rule("Rule ( VAR_DEFINITIONS -> VAR_DEFINITION VAR_DEFINITIONS_NEW )");
			current_token = back_token();
			count += parse_VAR_DEFINITION(mode); 
			count += parse_VAR_DEFINITIONS_NEW(mode);
			break; 
		}

		default:
		{	
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.set[1] = TOKEN_BRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;			
			expected.size = 2;

			hold_error(follow, expected);		
			break;	
		}
	}

	return count;
}

/* 
*	Function for rule VAR_DEFINITION -> TYPE VARIABLES_LIST 
*/
long parse_VAR_DEFINITION(Mode mode)
{		
	Attr_type type = ERROR_TYPE;
	long count = 0;					/* Number of variables */

	current_token = next_token();

	switch(current_token->kind)
	{
		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		{
			print_rule("Rule ( VAR_DEFINITION -> TYPE VARIABLES_LIST )");
			current_token = back_token();
			
			type = parse_TYPE();

			count += parse_VARIABLES_LIST(type, mode);			
			break; 
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.set[1] = TOKEN_BRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;			
			expected.size = 2;

			hold_error(follow, expected);
			break;
		}
	}

	return count;
}

/* 
*	Function for rule TYPE -> real | integer 
*/
Attr_type parse_TYPE()
{	
	Attr_type type = ERROR_TYPE;

	current_token = next_token();
	
	switch (current_token->kind)
	{
		case TOKEN_KEYWORD_REAL:
		{
			print_rule("Rule ( TYPE -> real )");
			type = REAL_TYPE;			
			break; 
		}

		case TOKEN_KEYWORD_INTEGER:
		{
			print_rule("Rule ( TYPE -> integer )");
			type = INTEGER_TYPE;
			break; 
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_ID;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			expected.set[0] = TOKEN_KEYWORD_REAL;
			expected.set[1] = TOKEN_KEYWORD_INTEGER;			
			expected.size = 2;

			hold_error(follow, expected);
			break;
		}
	}

	return type;
}

/* 
*	Function for rule VARIABLES_LIST -> VARIABLE VARIABLES_LIST_NEW 
*/
long parse_VARIABLES_LIST(Attr_type type, Mode mode)
{
	long count = 0;			/* Number of variables */

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_ID:
		{
			print_rule("Rule ( VARIABLES_LIST -> VARIABLE VARIABLES_LIST_NEW )");
			current_token = back_token();
			count += parse_VARIABLE(type, mode); 			
			count += parse_VARIABLES_LIST_NEW(type, mode);
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.set[1] = TOKEN_BRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			expected.set[0] = TOKEN_ID;			
			expected.size = 1;

			hold_error(follow, expected);
			break;
		}
	}

	return count;
}

/*
*	Function for rule VARIABLE -> id VARIABLE_NEW 
*/
long parse_VARIABLE(Attr_type type, Mode mode)
{
	long count = 0;			/* Number of variables */

	Entry *entry = NULL;

	current_token = next_token();	

	switch (current_token->kind)
	{
		case TOKEN_ID:
		{
			switch(mode)
			{
				/* This function was called from statement */
				case STMNT_CALL:
				{
					entry = find(current_scope, current_token->lexeme);

					if(!entry)
					{
						print_semantic_error(VAR_NOT_DEFINED, current_token->lineNumber, current_token->lexeme);						
					}

					break;			
				}
				
				/* This function was called from block local or global */
				case BLOCK_CALL:
				{
					entry = insert(current_scope, current_token->lexeme);					
					
					if(entry)
					{
						entry->attr_type = type;
						count++;
					}					
					else
					{
						print_semantic_error(SAME_VAR_NAME, current_token->lineNumber, current_token->lexeme);						
					}
					
					break;				
				}

				/* This function was called from function definition */
				case FUNCT_CALL:
				{
					entry = insert(current_scope, current_token->lexeme);

					if (entry)
					{
						entry->attr_type = type;
						count++;

						if(f_entry)
						{
							f_entry->expression_value.FUNCTION.params_number_total++;
							f_entry->expression_value.FUNCTION.params_number_used++;
							add_param_to_function(f_entry, entry->attr_name, entry->attr_type);
						}
					}
					else
					{
						if (f_entry)
						{
							f_entry->expression_value.FUNCTION.params_number_total++;
						}

						print_semantic_error(SAME_VAR_NAME, current_token->lineNumber, current_token->lexeme);
					}
					
					break;
				}
			}
			
			print_rule("Rule ( VARIABLE -> id VARIABLE_NEW )");			
			count += parse_VARIABLE_NEW(entry, mode);
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);			
			follow.set[0] = TOKEN_COMMA;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			expected.set[0] = TOKEN_ID;			
			expected.size = 1;

			hold_error(follow, expected);
			break;
		}
	}

	return count;
}

/*
*	Function for rule VARIABLE_NEW -> [int_number] | epsilon.
*/
long parse_VARIABLE_NEW(Entry *entry, Mode mode)
{
	long count = 0;		/* Number of variables */

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_BRACKETS_LEFT:
		{			
			unsigned int size = 0;	/* Size of array */

			print_rule("Rule ( VARIABLE_NEW -> [int_number] )");			
				
			if(match(TOKEN_INTEGER))
			{							
				size = atoi(current_token->lexeme);					
			}
			
			if(match(TOKEN_BRACKETS_RIGHT))
			{
				switch(mode)
				{
					case STMNT_CALL:
					{
						if(entry)
						{
							/* If index value not in the array bounds then print error message */
							if(entry->expression_value.ARRAY.size - 1 < size)
							{
								print_semantic_error(ARRAY_OUT_OF_BOUND, current_token->lineNumber, entry->attr_name);								
								count = -1;
							}
						}

						break;
					}

					case BLOCK_CALL:
					{
						if (entry)
						{
							/* Add entry's metadata */
							entry->expression_type = ARRAY_EXPR;
							entry->expression_value.ARRAY.size = (size > 0) ? size : 0;
						}

						break;
					}
				}				
			}

			break;
		}

		case TOKEN_SEMICOLON:
		case TOKEN_COMMA:
		case TOKEN_ASSIGNMENT:
		case TOKEN_BRACES_RIGHT:
		{
			print_rule("Rule ( VARIABLE_NEW -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			follow.set[0] = TOKEN_SEMICOLON;			
			follow.set[1] = TOKEN_COMMA;
			follow.set[2] = TOKEN_ASSIGNMENT;
			follow.size = 3;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 4);
			expected.set[0] = TOKEN_BRACKETS_LEFT;
			expected.set[1] = TOKEN_SEMICOLON;
			expected.set[2] = TOKEN_COMMA;
			expected.set[3] = TOKEN_ASSIGNMENT;			
			expected.size = 4;

			hold_error(follow, expected);
			break;
		}
	}

	return count;
}

/* 
*	Function for rule VARIABLES_LIST_NEW -> , VARIABLE VARIABLES_LIST_NEW | epsilon 
*/
long parse_VARIABLES_LIST_NEW(Attr_type type, Mode mode)
{
	long count = 0;		/* Number of variables */

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_COMMA:
		{
			print_rule("Rule ( VARIABLES_LIST_NEW -> , VARIABLE VARIABLES_LIST_NEW )");
			count += parse_VARIABLE(type, mode);			
			count += parse_VARIABLES_LIST_NEW(type, mode);
			break;
		}

		case TOKEN_SEMICOLON:
		case TOKEN_BRACES_RIGHT:
		{
			print_rule("Rule ( VARIABLES_LIST_NEW -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.set[1] = TOKEN_BRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_COMMA;
			expected.set[1] = TOKEN_SEMICOLON;
			expected.set[2] = TOKEN_BRACES_RIGHT;			
			expected.size = 3;

			hold_error(follow, expected);			
			break;
		}
	}

	return count;
}

/* 
*	Function for rule VAR_DEFINITIONS_NEW -> ; VAR_DEFINITIONS | epsilon 
*/
long parse_VAR_DEFINITIONS_NEW(Mode mode)
{
	long count = 0;		/* Number of variables */

	current_token = next_token();

	switch (current_token->kind)
	{
		/* First-Follow conflict happens, hense lookahead one token. */
		case TOKEN_SEMICOLON:
		{
			current_token = next_token();

			if(current_token->kind == TOKEN_KEYWORD_REAL || current_token->kind == TOKEN_KEYWORD_INTEGER)
			{
				print_rule("Rule ( VAR_DEFINITIONS_NEW -> ; VAR_DEFINITIONS )");
				current_token = back_token();
				count += parse_VAR_DEFINITIONS(mode);
			}
			else
			{
				print_rule("Rule ( VAR_DEFINITIONS_NEW -> epsilon )");
				current_token = back_token();
				current_token = back_token();
			}

			break;
		}

		case TOKEN_BRACES_RIGHT: 
		{
			print_rule("Rule ( VAR_DEFINITIONS_NEW -> epsilon )");
			current_token = back_token();
			break; 
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.set[1] = TOKEN_BRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);			
			expected.set[0] = TOKEN_BRACES_RIGHT;
			expected.set[1] = TOKEN_SEMICOLON;
			expected.size = 2;

			hold_error(follow, expected);			
			break;
		}
	}

	return count;
}

/*
*	Function for rule STATEMENTS -> STATEMENT ; STATEMENTS_NEW
*/
Attr_type parse_STATEMENTS(Mode mode)
{
	Attr_type type = ERROR_TYPE;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_CURLYBRACES_LEFT:		
		case TOKEN_ID:
		case TOKEN_KEYWORD_RETURN:
		{
			print_rule("Rule ( STATEMENTS -> STATEMENT ; STATEMENTS_NEW )");
			current_token = back_token();
			type = parse_STATEMENT(mode);
			match(TOKEN_SEMICOLON);
						
			type = parse_STATEMENTS_NEW(mode);
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_KEYWORD_END;
			follow.set[1] = TOKEN_CURLYBRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_CURLYBRACES_LEFT;
			expected.set[1] = TOKEN_ID;
			expected.set[2] = TOKEN_KEYWORD_RETURN;			
			expected.size = 3;

			hold_error(follow, expected);			
			break;
		}
	}

	return type;
}

/*
*	Function for rule STATEMENT -> id STATEMENT_NEW_2 | BLOCK | return STATEMENT_NEW_1
*/
Attr_type parse_STATEMENT(Mode mode)
{	
	Attr_type type = ERROR_TYPE;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_CURLYBRACES_LEFT:
		{
			print_rule("Rule ( STATEMENT -> BLOCK )");
			current_token = back_token();

			current_scope = make_table(current_scope);
			type = parse_BLOCK(mode); 
			current_scope = pop_table(current_scope);
			break;
		}
		
		case TOKEN_ID:
		{	
			Entry *entry;

			if(mode == FUNCT_CALL)
			{
				entry = lookup(current_scope, current_token->lexeme);
			}
			else
			{
				entry = find(current_scope, current_token->lexeme);
			}			
			
			if(!entry)
			{
				print_semantic_error(VAR_NOT_DEFINED, current_token->lineNumber, current_token->lexeme);
			}

			print_rule("Rule ( STATEMENT -> id STATEMENT_NEW_2 )");
			parse_STATEMENT_NEW_2(entry, STMNT_CALL);
			break;
		}

		case TOKEN_KEYWORD_RETURN:
		{
			print_rule("Rule ( STATEMENT-> return STATEMENT_NEW_1 )");
			type = parse_STATEMENT_NEW_1();
			
			if (f_entry && mode == FUNCT_CALL)
			{
				if (type != f_entry->attr_type)
				{
					print_semantic_error(F_RETURNED_VALUE_NOT_MATCH, current_token->lineNumber, f_entry->attr_name);
				}
			}

			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_CURLYBRACES_LEFT;
			expected.set[1] = TOKEN_ID;
			expected.set[2] = TOKEN_KEYWORD_RETURN;			
			expected.size = 3;

			hold_error(follow, expected);			
			break;
		}
	}

	return type;
}

/*
*	Function for rule BLOCK -> { VAR_DEFINITIONS ; STATEMENTS }
*/
Attr_type parse_BLOCK(Mode mode)
{
	Attr_type type = ERROR_TYPE;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_CURLYBRACES_LEFT:
		{
			print_rule("Rule ( BLOCK -> { VAR_DEFINITIONS ; STATEMENTS } )");
			parse_VAR_DEFINITIONS(BLOCK_CALL);
			match(TOKEN_SEMICOLON);
			type = parse_STATEMENTS(mode);
			match(TOKEN_CURLYBRACES_RIGHT);
			break; 
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 5);
			follow.set[0] = TOKEN_KEYWORD_VOID;
			follow.set[1] = TOKEN_KEYWORD_INTEGER;
			follow.set[2] = TOKEN_KEYWORD_REAL;
			follow.set[3] = TOKEN_SEMICOLON;
			follow.set[4] = TOKEN_EOF;
			follow.size = 5;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			expected.set[0] = TOKEN_CURLYBRACES_LEFT;			
			expected.size = 1;

			hold_error(follow, expected);			
			break;
		}
	}	

	return type;
}

/*
*	Function for rule EXPRESSION -> int_number | real_number | id EXPRESSION_NEW
*/
Attr_type parse_EXPRESSION()
{
	Attr_type type = ERROR_TYPE;	

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_REAL:
		{	
			print_rule("Rule ( EXPRESSION -> real_number )");
			type = REAL_TYPE;
			break;		
		}

		case TOKEN_INTEGER:
		{
			print_rule("Rule ( EXPRESSION -> int_number )");
			type = INTEGER_TYPE;
			break;
		}

		case TOKEN_ID:
		{
			Entry *entry = find(current_scope, current_token->lexeme);

			if (!entry)
			{
				print_semantic_error(VAR_NOT_DEFINED, current_token->lineNumber, current_token->lexeme);				
			}
			else
			{
				if(entry->expression_type == FUNCT_EXPR)
				{
					print_semantic_error(USING_FUN_IN_EXPRSN, current_token->lineNumber, entry->attr_name);
				}
			}

			print_rule("Rule ( EXPRESSION -> id EXPRESSION_NEW )");
			type = parse_EXPRESSION_NEW(entry);	
			type = (!entry || entry->expression_type == FUNCT_EXPR) ? ERROR_TYPE : type;
			break;		
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_REAL;
			expected.set[1] = TOKEN_INTEGER;
			expected.set[2] = TOKEN_ID;
			expected.size = 3;

			hold_error(follow, expected);			
			break;
		}
	}

	return type;
}

/*
*	Function for rule EXPRESSION_NEW -> ar_op EXPRESSION | VARIABLE_NEW
*/
Attr_type parse_EXPRESSION_NEW(Entry *entry)
{	
	Attr_type type = ERROR_TYPE;
	int equal;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_DIVISION:
		case TOKEN_MULTIPLICATION:
		{
			/* Check zero division error */
			if(current_token->kind == TOKEN_DIVISION)
			{
				current_token = next_token();
				equal = strcmp(current_token->lexeme, "0");

				if(equal == 0)
				{
					print_semantic_error(ZERO_DIVISION, current_token->lineNumber, NULL);
				}

				current_token = back_token();
			}

			if(entry && entry->expression_type == ARRAY_EXPR)
			{
				print_semantic_error(USING_ARR_IN_EXPRSN, current_token->lineNumber, entry->attr_name);
			}

			print_rule("Rule ( EXPRESSION_NEW -> ar_op EXPRESSION )");
			type = parse_EXPRESSION();	
			type = (entry && entry->expression_type == ARRAY_EXPR) ? ERROR_TYPE : type;
			break;
		}

		case TOKEN_BRACKETS_LEFT:
		{
			print_rule("Rule ( EXPRESSION_NEW -> VARIABLE_NEW )");
			current_token = back_token();
			parse_VARIABLE_NEW(entry, STMNT_CALL);
			type = (entry) ? entry->attr_type : ERROR_TYPE;
			break;
		}

		case TOKEN_SEMICOLON:
		{
			print_rule("Rule ( EXPRESSION_NEW -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 4);
			expected.set[0] = TOKEN_DIVISION;
			expected.set[1] = TOKEN_MULTIPLICATION;
			expected.set[2] = TOKEN_BRACKETS_LEFT;
			expected.set[3] = TOKEN_SEMICOLON;
			expected.size = 4;
			
			hold_error(follow, expected);
			break;
		}
	}

	return type;
}

/*
*	Function for rule PARAMETERS_LIST -> VARIABLES_LIST | epsilon
*/
void parse_PARAMETERS_LIST()
{
	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_ID:
		{
			print_rule("Rule ( PARAMETERS_LIST -> VARIABLES_LIST )");
			current_token = back_token();
			parse_VARIABLES_LIST(ERROR_TYPE, STMNT_CALL);
			break;
		}

		case TOKEN_BRACES_RIGHT:
		{
			print_rule("Rule ( PARAMETERS_LIST -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_BRACES_RIGHT;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			expected.set[0] = TOKEN_ID;			
			expected.set[1] = TOKEN_BRACES_RIGHT;
			expected.size = 2;
			
			hold_error(follow, expected);
			break;
		}
	}	
}

/*
*	Function for rule STATEMENT_NEW_1 -> EXPRESSION | epsilon
*/
Attr_type parse_STATEMENT_NEW_1()
{
	Attr_type type = ERROR_TYPE;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_REAL:
		case TOKEN_INTEGER:
		case TOKEN_ID:
		{
			print_rule("Rule ( STATEMENT_NEW_1 -> EXPRESSION )");
			current_token = back_token();
			type = parse_EXPRESSION();
			break;
		}

		case TOKEN_SEMICOLON:
		{
			print_rule("Rule ( STATEMENT_NEW_1 -> epsilon )");
			current_token = back_token();
			type = VOID_TYPE;
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 4);
			expected.set[0] = TOKEN_INTEGER;
			expected.set[1] = TOKEN_REAL;
			expected.set[2] = TOKEN_ID;
			expected.set[3] = TOKEN_SEMICOLON;
			expected.size = 4;
			
			hold_error(follow, expected);
			break;
		}
	}

	return type;
}

/*
*	Function for rule STATEMENT_NEW_2 -> VARIABLE_NEW = EXPRESSION | ( PARAMETERS_LIST )
*/
void parse_STATEMENT_NEW_2(Entry *entry, Mode mode)
{	
	Attr_type type;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_BRACES_LEFT:
		{
			print_rule("Rule ( STATEMENT_NEW_2 -> ( PARAMETERS_LIST ) )");
			parse_PARAMETERS_LIST();
			match(TOKEN_BRACES_RIGHT);
			break;		
		}

		case TOKEN_ASSIGNMENT:
		case TOKEN_BRACKETS_LEFT:
		{
			print_rule("Rule ( STATEMENT_NEW_2 -> VARIABLE_NEW = EXPRESSION )");
			current_token = back_token();
			parse_VARIABLE_NEW(entry, mode);
			match(TOKEN_ASSIGNMENT);
			type = parse_EXPRESSION();

			if(entry)
			{
				if ((type != entry->attr_type && entry->expression_type != FUNCT_EXPR) || type == ERROR_TYPE ||
					(type == REAL_TYPE && entry->attr_type == INTEGER_TYPE))
				{
					print_semantic_error(VAR_TYPE_NOT_MATCH, current_token->lineNumber, entry->attr_name);
				}			
			}			

			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_SEMICOLON;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			expected.set[0] = TOKEN_BRACES_LEFT;
			expected.set[1] = TOKEN_BRACKETS_LEFT;			
			expected.size = 2;
			
			hold_error(follow, expected);
			break;
		}
	}	
}

/*
*	Function for rule STATEMENTS_NEW -> STATEMENTS | epsilon
*/
Attr_type parse_STATEMENTS_NEW(Mode mode)
{
	Attr_type type = ERROR_TYPE;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_CURLYBRACES_LEFT:
		case TOKEN_ID:
		case TOKEN_KEYWORD_RETURN:
		{
			print_rule("Rule ( STATEMENTS_NEW -> STATEMENTS )");
			current_token = back_token();
			type = parse_STATEMENTS(mode);
			break;
		}

		case TOKEN_KEYWORD_END:
		case TOKEN_CURLYBRACES_RIGHT:
		{
			print_rule("Rule ( STATEMENTS_NEW -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 2);
			follow.set[0] = TOKEN_KEYWORD_END;
			follow.set[1] = TOKEN_CURLYBRACES_RIGHT;
			follow.size = 2;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 5);
			expected.set[0] = TOKEN_CURLYBRACES_LEFT;
			expected.set[1] = TOKEN_KEYWORD_RETURN;
			expected.set[2] = TOKEN_KEYWORD_END;
			expected.set[3] = TOKEN_ID;
			expected.set[4] = TOKEN_CURLYBRACES_RIGHT;
			expected.size = 5;
			
			hold_error(follow, expected);
			break;
		}
	}

	return type;
}

/*
*	Function for rule PARAM_DEFINITIONS -> VAR_DEFINITIONS | epsilon
*/
long parse_PARAM_DEFINITIONS()
{
	long count = 0;		/* Number of variables */

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		{
			print_rule("Rule ( PARAM_DEFINITIONS -> VAR_DEFINITIONS )");
			current_token = back_token();
			count = parse_VAR_DEFINITIONS(FUNCT_CALL);
			break;
		}

		case TOKEN_BRACES_RIGHT:
		{
			print_rule("Rule ( PARAM_DEFINITIONS -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_BRACES_RIGHT;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;
			expected.set[2] = TOKEN_BRACES_RIGHT;			
			expected.size = 3;

			hold_error(follow, expected);
			break;
		}
	}

	return count;
}

/*
*	Function for rule RETURNED_TYPE -> void | TYPE
*/
Attr_type parse_RETURNED_TYPE()
{
	Attr_type type = ERROR_TYPE;

	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_KEYWORD_VOID:
		{
			print_rule("Rule ( RETURNED_TYPE -> void )");
			type = VOID_TYPE;
			break;
		}

		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		{
			print_rule("Rule ( RETURNED_TYPE -> TYPE )");
			current_token = back_token();
			type = parse_TYPE();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_ID;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;
			expected.set[2] = TOKEN_KEYWORD_VOID;			
			expected.size = 3;
			
			hold_error(follow, expected);
			break;
		}
	}

	return type;
}

/*
*	Function for rule FUNC_DEFINITION -> RETURNED_TYPE id ( PARAM_DEFINITIONS ) BLOCK 
*/
void parse_FUNC_DEFINITION()
{
	Attr_type type;
	long count = 0;
	
	current_scope = make_table(current_scope);
	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_KEYWORD_VOID:
		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		{
			print_rule("Rule ( FUNC_DEFINITION -> RETURNED_TYPE id ( PARAM_DEFINITIONS ) BLOCK )");
			current_token = back_token();
			type = parse_RETURNED_TYPE();
						
			if(match(TOKEN_ID))
			{				
				f_entry = insert(current_scope, current_token->lexeme);

				if(f_entry)
				{				
					f_entry->attr_type = type;
					f_entry->expression_type = FUNCT_EXPR;
					f_entry->expression_value.FUNCTION.params = NULL;
					f_entry->expression_value.FUNCTION.params_number_total = 0;
					f_entry->expression_value.FUNCTION.params_number_used = 0;
				}			
			}

			match(TOKEN_BRACES_LEFT);			
			parse_PARAM_DEFINITIONS();			
			match(TOKEN_BRACES_RIGHT);			
			parse_BLOCK(FUNCT_CALL);			
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 4);
			follow.set[0] = TOKEN_EOF;
			follow.set[1] = TOKEN_KEYWORD_INTEGER;
			follow.set[2] = TOKEN_KEYWORD_REAL;
			follow.set[3] = TOKEN_KEYWORD_VOID;
			follow.size = 4;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;
			expected.set[2] = TOKEN_KEYWORD_VOID;			
			expected.size = 3;
			
			hold_error(follow, expected);
			break;
		}
	}
	
	free_funct_params(f_entry);
	current_scope = pop_table(current_scope);
}

/*
*	Function for rule FUNC_DEFINITIONS -> FUNC_DEFINITION FUNC_DEFINITIONS_NEW
*/
void parse_FUNC_DEFINITIONS()
{	
	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		case TOKEN_KEYWORD_VOID:
		{
			print_rule("Rule ( FUNC_DEFINITIONS -> FUNC_DEFINITION FUNC_DEFINITIONS_NEW )");
			current_token = back_token();
			parse_FUNC_DEFINITION();
			parse_FUNC_DEFINITIONS_NEW();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_EOF;
			follow.size = 1;

			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 3);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;
			expected.set[2] = TOKEN_KEYWORD_VOID;			
			expected.size = 3;
					
			hold_error(follow, expected);
			break;
		}
	}	
}

/*
*	Function for rule FUNC_DEFINITIONS_NEW -> FUNC_DEFINITION FUNC_DEFINITIONS_NEW | epsilon
*/
void parse_FUNC_DEFINITIONS_NEW()
{
	current_token = next_token();

	switch (current_token->kind)
	{
		case TOKEN_KEYWORD_REAL:
		case TOKEN_KEYWORD_INTEGER:
		case TOKEN_KEYWORD_VOID:
		{
			print_rule("Rule ( FUNC_DEFINITIONS_NEW -> FUNC_DEFINITION FUNC_DEFINITIONS_NEW )");
			current_token = back_token();
			parse_FUNC_DEFINITION();
			parse_FUNC_DEFINITIONS_NEW();
			break;
		}

		case TOKEN_EOF:
		{
			print_rule("Rule ( FUNC_DEFINITIONS_NEW -> epsilon )");
			current_token = back_token();
			break;
		}

		default:
		{
			follow.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
			follow.set[0] = TOKEN_EOF;
			follow.size = 1;
			
			expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 4);
			expected.set[0] = TOKEN_KEYWORD_INTEGER;
			expected.set[1] = TOKEN_KEYWORD_REAL;
			expected.set[2] = TOKEN_KEYWORD_VOID;
			expected.set[3] = TOKEN_EOF;
			expected.size = 4;

			hold_error(follow, expected);			
			break;
		}
	}	
}

/*
*	This function checks that the next token is of expected kind t.
*/
int match(eTOKENS t)
{
	int match = 1;

	current_token = next_token();

	if (current_token->kind != t)
	{
		expected.set = (eTOKENS*)calloc(sizeof(eTOKENS), 1);
		expected.set[0] = t;		
		expected.size = 1;

		print_parsing_error(expected);
		match = 0;
	}

	return match;
}

/*
*	This function opens a new file for output.
*/
void open_file()
{
	sprintf(parser_temp_str, "C:\\temp\\test%d_321952012_syntactic.txt", numberOfFiles);
	out_syntactic = fopen(parser_temp_str, "w");

	if (!out_syntactic)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);
	}

	printf("open output file ---> %s\n", parser_temp_str);
	
	sprintf(parser_temp_str, "C:\\temp\\test%d_321952012_semantic.txt", numberOfFiles);
	out_semantic = fopen(parser_temp_str, "w");

	if (!out_semantic)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);
	}

	printf("open output file ---> %s\n", parser_temp_str);
	printf(" \n----------------------------\n\n");
}

/*
*	This function prints a current grammar rule.
*/
void print_rule(char *rule)
{
	printf("%s\n", rule);
	fprintf(out_syntactic, "%s\n", rule);
}

/*
*	This function prints parsing errors.
*/
void print_parsing_error(Expected exp)
{
	int i;
	char *actual_token = enum_to_string(current_token->kind);
	
	if (exp.size == 1)
	{
		sprintf(parser_temp_str, "Expected: token '%s' at line: %d \n Actual token: %s, lexeme: %s ", enum_to_string(exp.set[0]), current_token->lineNumber, actual_token, current_token->lexeme);
	}
	else if (exp.size > 1)
	{		
		char tmp[STR_MAX_LENGTH] = { "" };
		strcat(tmp, enum_to_string(exp.set[0]));

		for (i = 1; i < exp.size; i++)
		{
			strcat(tmp, ",");
			strcat(tmp, enum_to_string(exp.set[i]));
		}

		sprintf(parser_temp_str, "Expected: one of tokens '%s' at line: %d \n Actual token: %s, lexeme: %s \n", 
			tmp, current_token->lineNumber, actual_token, current_token->lexeme);
	}
	else
	{
		fprintf(stderr, "\nError: Wrong params in print_parsing_error()!\n\n");
		exit(EXIT_FAILURE);
	}
	
	print_error(out_syntactic, parser_temp_str);
}

/*
*	This function returns token name in string representation according to eTOKENS value.
*/
char *enum_to_string(eTOKENS kind)
{
	char *enum_names[] = 
	{ 
		"TOKEN_ID",
		"TOKEN_INTEGER",
		"TOKEN_REAL",
		"TOKEN_ASSIGNMENT",
		"TOKEN_MULTIPLICATION",
		"TOKEN_DIVISION",
		"TOKEN_KEYWORD_REAL",
		"TOKEN_KEYWORD_INTEGER",
		"TOKEN_KEYWORD_VOID",
		"TOKEN_KEYWORD_RETURN",
		"TOKEN_KEYWORD_PROGRAM",
		"TOKEN_KEYWORD_END",
		"TOKEN_SEMICOLON",
		"TOKEN_COMMA",
		"TOKEN_CURLYBRACES_LEFT",
		"TOKEN_CURLYBRACES_RIGHT",
		"TOKEN_BRACES_LEFT",
		"TOKEN_BRACES_RIGHT",
		"TOKEN_BRACKETS_LEFT",
		"TOKEN_BRACKETS_RIGHT",
		"TOKEN_EOF" 
	};

	return enum_names[kind];
}

/*
*	This function prints semantic errors.
*/
void print_semantic_error(Error error_type, int line_num, String name)
{
	switch (error_type) 
	{
		case ZERO_DIVISION:
		{
			sprintf(parser_temp_str, "Line %d:	Zero division is forbidden! \n ", line_num);
			break;
		}

		case F_RETURNED_VALUE_NOT_MATCH:
		{
			sprintf(parser_temp_str, "Line %d:	Returned type of function \"%s\" does not match the type of returned statement ! \n ", line_num, name);
			break;
		}

		case SAME_VAR_NAME:
		{
			sprintf(parser_temp_str, "Line %d:	Variables with the same name \"%s\"! \n ", line_num, name);
			break;
		}

		case VAR_TYPE_NOT_MATCH:
		{
			sprintf(parser_temp_str, "Line %d:	Type of rigth hand variable \"%s\" does not match with the expression type! \n ", line_num, name);
			break;
		}

		case ARRAY_OUT_OF_BOUND:
		{
			sprintf(parser_temp_str, "Line %d:	Array \"%s\" out of bounds! \n ", line_num, name);
			break;
		}

		case VAR_NOT_DEFINED:
		{
			sprintf(parser_temp_str, "Line %d:	Using variable \"%s\" that was not defined is forbidden! \n ", line_num, name);
			break;		
		}

		case USING_FUN_IN_EXPRSN: 
		{
			sprintf(parser_temp_str, "Line %d:	Using function \"%s\" in expression is forbidden! \n ", line_num, name);
			break;
		}

		case USING_ARR_IN_EXPRSN:
		{
			sprintf(parser_temp_str, "Line %d:	Using array \"%s\" without index in expression is forbidden! \n ", line_num, name);
			break;
		}
	}

	print_error(out_semantic, parser_temp_str);
	se_error_count++;
}

/*
*	This function makes recovery operation when parsing error occurs.
*/
void make_recovery(Follow fol)
{
	while (!in_follow_set(fol, current_token->kind) && current_token->kind != TOKEN_EOF)
	{
		current_token = next_token();
	}

	current_token = back_token();
}

/*
*	This function checks if current token exist in function's follow set.
*/
int in_follow_set(Follow follow, eTOKENS kind)
{
	int i, exist = 0;

	int size = follow.size;

	for (i = 0; i < size; i++)
	{
		if(follow.set[i] == kind)
		{
			exist = 1;
		}		
	}

	return exist;
}

/*
*	This function holds error in parsing function.
*/
void hold_error(Follow follow, Expected expected)
{	
	print_parsing_error(expected);
	make_recovery(follow);
	free(follow.set);
	free(expected.set);
}

/*
*	This function inserts parameter data to function entry.
*/
void add_param_to_function(Entry *function, String param_name, Attr_type param_type)
{
	int size = 0;
	FUNC_PARAM *param = NULL;

	if(function)
	{	
		size = function->expression_value.FUNCTION.params_number_total;

		if(size == 1)
		{
			param = (FUNC_PARAM *)malloc(sizeof(FUNC_PARAM));

			if (!param)
			{
				perror("Error: ");
				exit(EXIT_FAILURE);
			}

			param->param_name = param_name;
			param->param_type = param_type;					
		}
		else
		{
			param = (FUNC_PARAM *)realloc(function->expression_value.FUNCTION.params, sizeof(FUNC_PARAM) * size);

			if (!param)
			{
				perror("Error: ");
				exit(EXIT_FAILURE);
			}

			param[size - 1].param_name = param_name;
			param[size - 1].param_type = param_type;
		}

		function->expression_value.FUNCTION.params = param;
	}
}

/*
*	This function used to free memory
*/
void free_funct_params(Entry *function)
{	
	free(function->expression_value.FUNCTION.params);
	free(function);	
}

/*
*	This function prints final number of errors
*/
void print_final_error_message()
{
	fprintf(stderr, "\nNumber of semantic errors: %d\n", se_error_count);
	fprintf(out_semantic, "\nNumber of semantic errors: %d\n", se_error_count);

	//fprintf(stderr, "\nNumber of syntactic errors: %d\n", sy_error_count);
	//fprintf(out_syntactic, "\nNumber of syntactic errors: %d\n", sy_error_count);
}