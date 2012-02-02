#include "tree.h"


token_list *replacement_for_null_char(void);

int has_element_in_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node);
int has_element_in_list_item_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node);
int has_element_in_button_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node);
int has_element_in_table_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node);
int has_element_in_select_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node);

node *back_to_table_context(node *root, node *current_node);
node *back_to_table_body_context(node *root, node *current_node);
node *back_to_table_row_context(node *root, node *current_node);

int is_in_special_category(element_node *e);
node *generate_implied_end_tags(node *current_node);
void add_child_to_foster_parent(node *root, node *current_node, node *child_node);
element_node *get_node_by_name(node *root, node *current_node, unsigned char *node_name);
