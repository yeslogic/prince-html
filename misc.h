// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include "tree.h"
#include "stack.h"


token_list *replacement_for_null_char(void);

int has_element_in_scope(element_stack *st, unsigned char *element_name, element_node **match_node);
int has_element_in_list_item_scope(element_stack *st, unsigned char *element_name, element_node **match_node);
int has_element_in_button_scope(element_stack *st, unsigned char *element_name, element_node **match_node);
int has_element_in_table_scope(element_stack *st, unsigned char *element_name, element_node **match_node);
int has_element_in_select_scope(element_stack *st, unsigned char *element_name, element_node **match_node);

element_node *back_to_table_context(element_stack **st_ptr);
element_node *back_to_table_body_context(element_stack **st_ptr);
element_node *back_to_table_row_context(element_stack **st_ptr);

int is_in_special_category(element_node *e);
element_node *generate_implied_end_tags(element_stack **st_ptr, unsigned char *to_exclude);
void add_child_to_foster_parent(element_stack *o_e_st, node *child_node);
element_node *get_node_by_name(element_stack *st, unsigned char *element_name);
