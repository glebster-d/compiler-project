

#include "Scope.h"




/*
*	This function creates symbol table for this block and links it to the 
*	table of current scope
*/
Scope* make_table(Scope *current)
{

	Scope *scope = (Scope*)malloc(sizeof(Scope));

	if (!scope)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);
	}

	scope->father = current;
	scope->tree_root = NULL;

	return scope;
}

/*
*	This function called when exiting from a nested block and returns pointer
*	to the enclosing block's table
*/
Scope* pop_table(Scope *current)
{	
	Scope *father;

	if (!current)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);
	}

	father = current->father;

	if (current->tree_root)
	{
		delete_tree(current->tree_root);		
	}

	free(current);

	return father;
}

/*
*	This function seraches for a variable name in symbol table of the current scope only
*	and returns pointer to the found entry or NULL
*/
Entry* lookup(Scope *scope, String name)
{
	Entry *entry = NULL;
	BST_Node *node = NULL;

	if(!name || !scope)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);
	}

	if (scope->tree_root)
	{
		node = search_node(scope->tree_root, name);
			
		if(node)
		{
			entry = node->entry;
		}
	}

	return entry;
}

/*
*	This function inserts the variable name into symbol table of the current 
*	scope and returns pointer to the new entry or NULL
*/
Entry* insert(Scope *scope, String name)
{	
	Entry *entry = NULL;

	if (!name)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);
	}

	entry = lookup(scope, name);

	if(!entry)
	{
		entry = (Entry *)malloc(sizeof(Entry));

		if (!entry)
		{
			perror("Error: ");
			exit(EXIT_FAILURE);
		}

		entry->attr_name = name;
		entry->expression_type = BASIC_EXPR;

		scope->tree_root = insert_node(scope->tree_root, entry);		
	}
	else
	{
		entry = NULL;
	}		
	
	return entry;
}

/*
*	This function performs hierarchical search of the variable name, starting
*	from the symbol table of the current scope and returns the found entry pointer
*	or NULL
*/
Entry* find(Scope *current_scope, String name)
{
	Scope *scope = current_scope;
	Entry *entry = NULL;
	
	while(scope)
	{
		entry = lookup(scope, name);
			
		if (entry)
		{
			break;
		}

		scope = scope->father;
	}

	return entry;
}

/*
*	This function deletes all tables
*/
void delete_tables(Scope *current)
{	
	while(current)
	{
		current = pop_table(current);	
	}
}