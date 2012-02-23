#include "tree.h"

typedef struct element_stack_s element_stack;


struct element_stack_s {
	element_node *e;
	element_stack *tail;
};

/*if st is empty, return 1, otherwise return 0*/
int open_element_stack_is_empty(element_stack *st);

void open_element_stack_push(element_stack **st, element_node *e);

/*if element_stack is empty, return 0, otherwise pop the top element and return 1 */
int open_element_stack_pop(element_stack **st);

/*pop elements up to and including the element named element_name, or to the end of the stack*/
void pop_elements_up_to(element_stack **st, unsigned char *element_name);

/*pop elements up to and including the element e, or to the end of the stack*/
void pop_ele_up_to(element_stack **st, element_node *e);

/*if element_stack is empty, return NULL, otherwise return the top element*/
element_node *open_element_stack_top(element_stack *st);

/*if e is on the stack, return 1, otherwise return 0*/
int is_on_element_stack(element_stack *st, element_node *e);


/*if st is NULL, return NULL, otherwise return a pointer to the previous node.(it could be NULL)*/
element_stack *previous_stack_node(element_stack *st);

/*if st is NULL, return NULL, otherwise return the stack node before e, (it could be NULL)*/
element_stack *stack_node_before_element(element_stack *st, element_node *e);

/*remove element e from the stack, if it is in the stack*/
void remove_element_from_stack(element_stack **st, element_node *e);

/*replace element e with element replacement, if e is in the stack*/
void replace_element_in_stack(element_stack *st, element_node *e, element_node *replacement);


/*insert element e into the stack immediately below element existing_e*/
/*if existing_e in not in stack, then e will not be inserted at all*/
void insert_into_stack_below_element(element_stack **st, element_node *e, element_node *existing_e);
