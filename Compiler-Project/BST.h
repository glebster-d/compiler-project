#ifndef BST_H
#define BST_H

#include <stdlib.h>
#include <string.h>
#include "Types.h"

/*
*	This structure represents the entry for semantic analysis
*/
typedef struct entry
{	
	String attr_name;				/* Name of type expression */
	Attr_type attr_type;			/* Type of values in array or basic. Return type in function */
	Exp_type expression_type;		/* Type expression: basic, array, function*/
	Exp_value expression_value;		/* Values that type espression holds */

} Entry;

/*
*	This structure represent single node in BST
*/
typedef struct bst_node
{
	Entry *entry;
	struct BST_Node *right;
	struct BST_Node *left;

} BST_Node;


BST_Node* create_node(Entry *);
BST_Node* insert_node(BST_Node *, Entry *);
BST_Node* search_node(BST_Node *, String);
void delete_tree(BST_Node *);

#endif // BST_H
