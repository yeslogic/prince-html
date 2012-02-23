#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tree.h"





element_node *create_element_node(unsigned char *name, attribute_list *attributes, namespace_type name_space)
{
	element_node *e = (element_node *)malloc(sizeof(element_node));

	e->type = ELEMENT_N;
	e->name = strdup(name);
	e->attributes = attributes;
	e->name_space = name_space;
	e->parent = NULL;
	e->first_child = NULL;
	e->last_child = NULL;
	e->previous_sibling = NULL;
	e->next_sibling = NULL;

	return e;
}

text_node *create_text_node(void)
{
	text_node *t = (text_node *)malloc(sizeof(text_node));

	t->type = TEXT_N;
	t->text_data = string_append(NULL, '\0');
	t->parent = NULL;
	t->first_child = NULL;
	t->last_child = NULL;
	t->previous_sibling = NULL;
	t->next_sibling = NULL;
	
	return t;
}


comment_node *create_comment_node(unsigned char *comment)
{
	comment_node *c = (comment_node *)malloc(sizeof(comment_node));

	c->type = COMMENT_N;
	c->comment_data = strdup(comment);
	c->parent = NULL;
	c->first_child = NULL;
	c->last_child = NULL;
	c->previous_sibling = NULL;
	c->next_sibling = NULL;
	
	return c;
}


/*parent must not be NULL, child_node must not be NULL*/
/*child_node will be added as the last child of the parent*/
void add_child_node(element_node *parent, node *child_node)
{
	assert((parent != NULL) && (child_node != NULL));

	if(parent->first_child == NULL)
	{
		parent->first_child = child_node;
		child_node->previous_sibling = NULL;
	}
	else
	{
		parent->last_child->next_sibling = child_node;
		child_node->previous_sibling = parent->last_child;
	}

	parent->last_child = child_node;
	child_node->parent = parent;
	child_node->next_sibling = NULL;
}


/*insert child_node as a child node of parent, immediately before child_2*/
/*child_2 is an existing child of parent*/
void insert_child_node(node *parent, node *child_node, node *child_2)
{
	assert((parent != NULL) && (child_node != NULL) && (child_2 != NULL) && (child_2->parent == parent));

	if(parent->first_child == child_2)
	{
		parent->first_child = child_node;
		child_node->previous_sibling = NULL;
	}
	else
	{
		child_node->previous_sibling = child_2->previous_sibling;
		child_2->previous_sibling->next_sibling = child_node;
	}

	child_node->parent = parent;
	child_node->next_sibling = child_2;
	child_2->previous_sibling = child_node;
}



/*parent must not be NULL*/
/*last child of parent will be removed, if parent has any child nodes*/
/*last child node and all its descendants are free'd */
void remove_last_child_node(node *parent)
{
	assert(parent != NULL);
	if(parent->first_child != NULL)
	{
		if(parent->first_child == parent->last_child)
		{
			free_tree(parent->first_child);
			parent->first_child = NULL;
			parent->last_child = NULL;
		}
		else
		{
			node *node_to_delete = parent->last_child;
			parent->last_child->previous_sibling->next_sibling = NULL;
			parent->last_child = parent->last_child->previous_sibling;
			free_tree(node_to_delete);
		}
	}
}

/*remove child_node from its parent, if it has one.*/
/*also remove its links with its siblings*/
/*child_node and its child nodes, if any, remain unchanged*/
void remove_child_node_from_parent(node *child_node)
{
	if(child_node->parent != NULL)
	{
		if((child_node->parent->first_child == child_node) && (child_node->parent->last_child == child_node))
		{
			child_node->parent->first_child = NULL;
			child_node->parent->last_child = NULL;
		}
		else if(child_node->parent->first_child == child_node)
		{
			child_node->parent->first_child = child_node->next_sibling;
			child_node->next_sibling->previous_sibling = NULL;	//child_node->next_sibling cannot be NULL here.
		}
		else if(child_node->parent->last_child == child_node)
		{
			child_node->parent->last_child = child_node->previous_sibling;
			child_node->previous_sibling->next_sibling = NULL;	//child_node->previous_sibling cannot be NULL here.
		}
		else
		{
			child_node->previous_sibling->next_sibling = child_node->next_sibling;
			child_node->next_sibling->previous_sibling = child_node->previous_sibling;
		}

		child_node->parent = NULL;
		child_node->previous_sibling = NULL;
		child_node->next_sibling = NULL;
	}
}


void free_node(node *n)
{
	if(n != NULL)
	{
		switch (n->type)
		{
			case ELEMENT_N:
				{
					element_node *e = (element_node *)n;
					attribute_list *attrs = e->attributes;
					attribute_list *temp_ptr;

					while(attrs != NULL)
					{
						free(attrs->name);
						free(attrs->value);
					
						temp_ptr = attrs;
						attrs = attrs->tail;
						free(temp_ptr);
					}
					free(e->name);
					free(e);
				}
				break;
			case TEXT_N:
				{
					text_node *t = (text_node *)n;
					free(t->text_data);
					free(t);
				}
				break;
			case COMMENT_N:
				{
					comment_node *c = (comment_node *)n;
					free(c->comment_data);
					free(c);
				}
				break;
			default:
				break;
		}
	}
}


void print_node(node *n)
{
	if(n != NULL)
	{
		switch (n->type)
		{
			case ELEMENT_N:
				{
					element_node *e = (element_node *)n;
					printf("Node type: ELEMENT.\n");
					printf("Element name: %s\n", e->name);
					html_print_attribute_list(e->attributes);
					print_parent_node(n);
					print_child_nodes(n);
				}
				break;
			case TEXT_N:
				{
					text_node *t = (text_node *)n;
					printf("Node type: TEXT.\n");
					printf("Text data: %s\n", t->text_data);
					print_parent_node(n);
				}
				break;
			case COMMENT_N:
				{
					comment_node *c = (comment_node *)n;
					printf("Node type: COMMENT.\n");
					printf("Comment data: %s\n", c->comment_data);
					print_parent_node(n);
				}
				break;
			default:
				break;
		}
		printf("-----------------------------------------------------------------------------------------\n");
	}
}

void print_parent_node(node *n)
{
	if((n != NULL) && (n->parent != NULL))
	{
		element_node *e = (element_node *)(n->parent);
		printf("Parent node name: %s\n", e->name);
	}
}

void print_child_nodes(node *n)
{
	node *temp_node;

	assert(n != NULL);

	temp_node = n->first_child;

	if(temp_node != NULL)
	{
		printf("Child nodes: ");

		while(temp_node != NULL)
		{
			if(temp_node->type == ELEMENT_N)
			{
				element_node *e = (element_node *)temp_node;
				printf("element node[%s]; ", e->name);
			}
			else if(temp_node->type == TEXT_N)
			{
				printf("text node; ");
			}
			else if(temp_node->type == COMMENT_N)
			{
				printf("comment node; ");
			}
			else
			{
				;
			}
			temp_node = temp_node->next_sibling;
		}
		printf("\n\n");
	}
}


void print_tree(node *root)
{
	node *temp_node;

	if(root != NULL)
	{
		print_node(root);	

		temp_node = root->first_child;

		while(temp_node != NULL)
		{	
			print_tree(temp_node);
			temp_node = temp_node->next_sibling;
		}	
	}
}

void free_tree(node *root)
{
	node *child_node, *next_sibling;

	if(root != NULL)
	{
		child_node = root->first_child;	

		while(child_node != NULL)
		{	
			next_sibling = child_node->next_sibling;

			free_tree(child_node);

			child_node = next_sibling;
		}

		free_node(root);
	}
}

