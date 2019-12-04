

#ifndef SCOPE_H
#define SCOPE_H

#include "BST.h"


/*
*	This structure defines the scope with the symbol table
*/
typedef struct scope
{	
	BST_Node *tree_root;	/* Sybmol table */
	struct Scope *father;

} Scope;

/*
*	This enumeration defines the calling mode
*/
typedef enum mode
{	
	BLOCK_CALL,
	FUNCT_CALL,
	STMNT_CALL

} Mode;

/*
*	This enumeration defines the kind of semantic error
*/
typedef enum error
{
	VAR_NOT_DEFINED,				/* Variable used but not defined error*/
	ARRAY_OUT_OF_BOUND,				/* Array out of bounds error*/
	ZERO_DIVISION,					/* Zero division error */
	F_RETURNED_VALUE_NOT_MATCH,		/* Returned value of function not match the declaration */
	SAME_VAR_NAME,					/* Existing variable with the same name */
	VAR_TYPE_NOT_MATCH,				/* Variable type not match the declaration */
	USING_FUN_IN_EXPRSN,			/* Using function in expression is forbidden */
	USING_ARR_IN_EXPRSN				/* Using array without index in expression is forbidden */

} Error;

Scope* make_table(Scope *);		
Scope* pop_table(Scope *);
Entry* lookup(Scope *, String);
Entry* insert(Scope *, String);
Entry* find(Scope *, String);

void delete_tables(Scope *);

#endif // SCOPE_H