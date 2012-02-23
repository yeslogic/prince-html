#ifndef TREE_H
#define TREE_H

#include "token.h"   

typedef struct node_s node;
typedef struct element_node_s element_node;
typedef struct text_node_s text_node;
typedef struct comment_node_s comment_node;


/*--------------------------------------------------------------------------*/
typedef enum {
	HTML,
	MATHML,
	SVG,
	XLINK,
	XML,
	XMLNS
} namespace_type;


typedef enum {
	ELEMENT_N,
	TEXT_N,
	COMMENT_N
} node_type;

struct node_s {
	node_type type;
	node *parent;
	node *previous_sibling;
	node *next_sibling;
	node *first_child;
	node *last_child;
};

struct element_node_s {
	node_type type;
	node *parent; 
	node *previous_sibling;
	node *next_sibling;
	node *first_child;
	node *last_child;
	unsigned char *name;
	attribute_list *attributes;
	namespace_type name_space;
};
 
 
struct text_node_s {
	node_type type; 
	node *parent; 
	node *previous_sibling;
	node *next_sibling;
	node *first_child;
	node *last_child;
	unsigned char *text_data;
};
 

struct comment_node_s {
	node_type type; 
	node *parent; 
	node *previous_sibling;
	node *next_sibling;
	node *first_child;
	node *last_child;
	unsigned char *comment_data;
};


/*--------------------------------------------------------------------------*/

element_node *create_element_node(unsigned char *name, attribute_list *attributes, namespace_type name_space);
text_node *create_text_node(void);
comment_node *create_comment_node(unsigned char *comment);

void add_child_node(element_node *parent, node *child_node);
void insert_child_node(node *parent, node *child_node, node *child_2);
void remove_last_child_node(node *parent);
void remove_child_node_from_parent(node *child_node);
void print_parent_node(node *n);
void print_child_nodes(node *n);
void print_node(node *n);
void free_node(node *n);
void print_tree(node *root);
void free_tree(node *root);

#endif