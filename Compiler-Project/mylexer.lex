%option noyywrap

%{	
	#include "Token.h"
	#include "Parser.h"

	#ifndef FILE_NUM
	#define FILE_NUM 2
	#endif // !FILE_NUM

	#ifndef STR_MAX_LENGTH
	#define STR_MAX_LENGTH 600
	#endif // !STR_MAX_LENGTH
		
	void print_token(char *token_name);					/* Printing token */
	extern void print_error(FILE *file, char *str);		/* Printing compiler errors */

	char lexer_temp_str[STR_MAX_LENGTH]; 				/* Path to file */		
	int lineNumber;										/* Line counter */
	extern int numberOfFiles = FILE_NUM; 				/* Number of files to read */			
%}

%x comments

LINEBREAK		[\n]
WHITESPACES		[ \t]
DIGIT			[0-9]
NONZERO			[1-9]
ALPHA			[a-zA-Z]
ALPHANUM		[a-zA-Z0-9]

NAME			{ALPHA}("_"?{ALPHANUM})*
INT_NUMBER		({NONZERO}{DIGIT}*|0)
REAL_NUMBER		({NONZERO}{DIGIT}*|0)"."{DIGIT}+

%%

{LINEBREAK}		{
					lineNumber++;
				}

{WHITESPACES}	{
					/* do nothing */;
				}
				
"--" 				BEGIN(comments);
<comments>[^\n]		{/* do nothing */;}
<comments>\n		{ lineNumber++; BEGIN(0); }

"*"				{
					create_and_store_token(TOKEN_MULTIPLICATION, yytext, lineNumber); 
					print_token("TOKEN_MULTIPLICATION");	
					return TOKEN_MULTIPLICATION;
				}

"/"				{
					create_and_store_token(TOKEN_DIVISION, yytext, lineNumber); 
					print_token("TOKEN_DIVISION");
					return TOKEN_DIVISION;
				}

"="				{
					create_and_store_token(TOKEN_ASSIGNMENT, yytext, lineNumber);
					print_token("TOKEN_ASSIGNMENT");
					return TOKEN_ASSIGNMENT;
				}
				
"integer"		{
					create_and_store_token(TOKEN_KEYWORD_INTEGER, yytext, lineNumber); 
					print_token("TOKEN_KEYWORD_INTEGER");
					return TOKEN_KEYWORD_INTEGER;
				}

"real"			{
					create_and_store_token(TOKEN_KEYWORD_REAL, yytext, lineNumber);
					print_token("TOKEN_KEYWORD_REAL");
					return TOKEN_KEYWORD_REAL;
				}

"void"			{
					create_and_store_token(TOKEN_KEYWORD_VOID, yytext, lineNumber);
					print_token("TOKEN_KEYWORD_VOID");
					return TOKEN_KEYWORD_VOID;
				}

"return"		{
					create_and_store_token(TOKEN_KEYWORD_RETURN, yytext, lineNumber); 
					print_token("TOKEN_KEYWORD_RETURN");
					return TOKEN_KEYWORD_RETURN;
				}

"program" 		{
					create_and_store_token(TOKEN_KEYWORD_PROGRAM, yytext, lineNumber);
					print_token("TOKEN_KEYWORD_PROGRAM");
					return TOKEN_KEYWORD_PROGRAM;
				}

"end"			{
					create_and_store_token(TOKEN_KEYWORD_END, yytext, lineNumber);
					print_token("TOKEN_KEYWORD_END");
					return TOKEN_KEYWORD_END;
				}

";"				{
					create_and_store_token(TOKEN_SEMICOLON, yytext, lineNumber); 
					print_token("TOKEN_SEMICOLON");
					return TOKEN_SEMICOLON;
				}

","				{
					create_and_store_token(TOKEN_COMMA, yytext, lineNumber);
					print_token("TOKEN_COMMA");
					return TOKEN_COMMA;
				}

"["				{
					create_and_store_token(TOKEN_BRACKETS_LEFT, yytext, lineNumber); 
					print_token("TOKEN_BRACKETS_LEFT");	
					return TOKEN_BRACKETS_LEFT;
				}

"]"				{
					create_and_store_token(TOKEN_BRACKETS_RIGHT, yytext, lineNumber);
					print_token("TOKEN_BRACKETS_RIGHT");
					return TOKEN_BRACKETS_RIGHT;					
				}

"{"				{
					create_and_store_token(TOKEN_CURLYBRACES_LEFT, yytext, lineNumber);
					print_token("TOKEN_CURLYBRACES_LEFT");
					return TOKEN_CURLYBRACES_LEFT;					
				}

"}"				{
					create_and_store_token(TOKEN_CURLYBRACES_RIGHT, yytext, lineNumber);
					print_token("TOKEN_CURLYBRACES_RIGHT");
					return TOKEN_CURLYBRACES_RIGHT;
				}
				
"("				{
					create_and_store_token(TOKEN_BRACES_LEFT, yytext, lineNumber); 
					print_token("TOKEN_BRACES_LEFT");
					return TOKEN_BRACES_LEFT;
				}

")"				{
					create_and_store_token(TOKEN_BRACES_RIGHT, yytext, lineNumber);
					print_token("TOKEN_BRACES_RIGHT");
					return TOKEN_BRACES_RIGHT;
				}
				
{REAL_NUMBER} 	{
					create_and_store_token(TOKEN_REAL, yytext, lineNumber);
					print_token("TOKEN_REAL"); 
					return TOKEN_REAL;
				}

{INT_NUMBER}	{
					create_and_store_token(TOKEN_INTEGER, yytext, lineNumber);
					print_token("TOKEN_INTEGER");
					return TOKEN_INTEGER;					
				}				

{NAME}			{
					create_and_store_token(TOKEN_ID, yytext, lineNumber);
					print_token("TOKEN_ID");
					return TOKEN_ID;				
				}
				
.				{						
					sprintf(lexer_temp_str, "Character '%s' at line: %d does not begin any legal token in the language!", yytext, lineNumber);	
					print_error(yyout, lexer_temp_str);
				}

<<EOF>>			{
					create_and_store_token(TOKEN_EOF, "EOF", lineNumber);
					print_token("TOKEN_EOF");										
					yyterminate();
					return TOKEN_EOF;					
				}

%%

int main(int argc, char **argv )
{		
	while (numberOfFiles > 0)
	{	
		printf(" \n----------------------------\n\n");		
		
		sprintf(lexer_temp_str, "C:\\temp\\test%d.txt", numberOfFiles);			
		printf("open input file  ---> %s\n", lexer_temp_str);
		yyin = fopen(lexer_temp_str, "r");
		
		sprintf(lexer_temp_str, "C:\\temp\\test%d_321952012_lex.txt", numberOfFiles);	
		printf("open output file ---> %s\n", lexer_temp_str);		
		yyout = fopen(lexer_temp_str, "w");
						
		if(!yyin || !yyout)
		{
			perror("Error: ");
			exit(EXIT_FAILURE);
		}
		lineNumber = 1;
				
		parser();
		numberOfFiles--;
		
		printf("\n\n");
		fclose(yyin);
		fclose(yyout);
	}
		
	system("pause");
	exit(EXIT_SUCCESS);
}

void print_token(char *token_name)
{
	printf("Token \"%s\" was found. Lexeme: %s \n", token_name, yytext); 
	fprintf(yyout,"Token of kind %s was found at line: %d, lexeme: %s.\n", token_name, lineNumber, yytext);
}

void print_error(FILE *file, char *str)
{
	fprintf(stderr, "\nError: %s\n\n", str);
	fprintf(file,"\nError: %s\n\n", str);
}