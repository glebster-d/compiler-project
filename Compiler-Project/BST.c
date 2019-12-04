

#include "BST.h"




/*
*	This function inserts a new node to BST
*/
BST_Node* insert_node(BST_Node *root, Entry *entry)
{
	BST_Node *node = root;
	int equal;

	// If the tree is empty, return a new node 
	if (!node) 
	{
		node = create_node(entry);
	}
	else
	{		
		equal = strcmp(entry->attr_name, node->entry->attr_name);

		// First string come before the second string.
		if(equal < 0)
		{
			node->left = insert_node(node->left, entry);			
		}
		else if( equal > 0)
		{
			node->right = insert_node(node->right, entry);
		}		
	}	
	
	return node;
}

/*
*	This function searches for the node according to the inserted key. 
*	If the node was found return it, otherwise return NULL.
*/
BST_Node* search_node(BST_Node *root, String key)
{
	BST_Node *node = root;
	int equal;

	if(node)
	{
		equal = strcmp(key, node->entry->attr_name);
		
		if (equal < 0)
		{
			node = search_node(node->left, key);
		}
		else if (equal > 0)
		{
			node = search_node(node->right, key);
		}		
	}

	return node;
}

/* 
*	This function creates a new BST node.
*/
BST_Node *create_node(Entry *e)
{
	BST_Node *new_node = (BST_Node *)malloc(sizeof(BST_Node));

	if (!new_node)
	{
		perror("Error: ");
		exit(EXIT_FAILURE);		
	}

	new_node->entry = e;
	new_node->left = new_node->right = NULL;

	return new_node;
}

/*
*	This function delete the BST tree
*/
void delete_tree(BST_Node *node)
{
	if (!node) 
	{
		return;
	}

	delete_tree(node->right);
	delete_tree(node->left);
	free(node);
}