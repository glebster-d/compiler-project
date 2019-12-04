#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout;
extern int yylex();

extern int back_token_called;	/* Indicates how much times back_token function was called */	

typedef enum eTOKENS
{
	TOKEN_ID,
	TOKEN_INTEGER,	
	TOKEN_REAL,
	TOKEN_ASSIGNMENT,
	TOKEN_MULTIPLICATION,
	TOKEN_DIVISION,
	TOKEN_KEYWORD_REAL,
	TOKEN_KEYWORD_INTEGER,
	TOKEN_KEYWORD_VOID,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_PROGRAM,
	TOKEN_KEYWORD_END,	
	TOKEN_SEMICOLON,
	TOKEN_COMMA,
	TOKEN_CURLYBRACES_LEFT,
	TOKEN_CURLYBRACES_RIGHT,
	TOKEN_BRACES_LEFT,
	TOKEN_BRACES_RIGHT,
	TOKEN_BRACKETS_LEFT,
	TOKEN_BRACKETS_RIGHT,
	TOKEN_EOF
	
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
	
} Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
	
} Node;

void create_and_store_token(eTOKENS kind, char *lexeme, int numOfLine);
Token* next_token();
Token* back_token();

#endif