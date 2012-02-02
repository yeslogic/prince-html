#include "misc.h"


typedef struct active_formatting_list_s active_formatting_list;

typedef enum {
	MARKER,
	FORMATTING_ELEMENT
} active_formatting_list_node_type;


struct active_formatting_list_s {
	active_formatting_list_node_type type;
	element_node *formatting_element;
	active_formatting_list *head;
	active_formatting_list *tail;
};


/*------------------------------------------------------------------------*/

active_formatting_list *active_formatting_list_add_element(element_node *formatting_element,
														   active_formatting_list *tail);
active_formatting_list *active_formatting_list_add_marker(active_formatting_list *tail);
element_node *is_in_active_formatting_list(active_formatting_list *list, element_node *e);
element_node *get_formatting_element(active_formatting_list *list, unsigned char *tag_name);
active_formatting_list *remove_element_from_active_formatting_list(active_formatting_list *list, element_node *e);
node *reconstruct_active_formatting_elements(active_formatting_list *list, node *root, node *current_node);
active_formatting_list *clear_list_up_to_last_marker(active_formatting_list *list);
int replace_element_in_active_formatting_list(active_formatting_list *list, 
													element_node *element_to_be_replaced,
													element_node *new_element);
active_formatting_list *insert_bookmark_in_active_formatting_list(active_formatting_list *list, element_node *element_before_bookmark, active_formatting_list **book_mark);
active_formatting_list *remove_bookmark_from_active_formatting_list(active_formatting_list *list, active_formatting_list *book_mark);

int element_compare(element_node *e1, element_node *e2);
int is_open_element(node *root, node* current_node, element_node *e);
void print_active_formatting_list(active_formatting_list *list);
 
element_node *get_furthest_block(node *root, node *current_node, element_node *formatting_ele);

void adoption_agency_algorithm(node *root, active_formatting_list **list_ptr, node **current_node_ptr, 
								unsigned char *formatting_element_name);
/*------------------------------------------------------------------------*/