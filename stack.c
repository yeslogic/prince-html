// Copyright (C) 2011-2015 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stack.h"
#include "util.h"



/*if st is empty, return 1, otherwise return 0*/
int html_element_stack_is_empty(element_stack *st)
{
	if(st == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void open_element_stack_push(element_stack **st, element_node *e)
{
	element_stack *new_node = malloc(sizeof(element_stack));
	new_node->e = e;
	new_node->tail = *st;

	*st = new_node;
}


/*if element_stack is empty, return 0, otherwise pop the top element and return 1 */
int open_element_stack_pop(element_stack **st)
{
	element_stack *temp_st_ptr;

	if(*st != NULL)
	{
		temp_st_ptr = (*st)->tail;
		free(*st);
		*st = temp_st_ptr;
		return 1;
	}
	else
	{
		return 0;
	}
}


/*free the stack of open elements*/
void free_element_stack(element_stack *st)
{
	element_stack *temp_ptr;

	while(st != NULL)
	{
		temp_ptr = st;
		st = st->tail;

		free(temp_ptr);
	}
}

/*pop elements off the stack up to and including the element named element_name.*/
/*element_name must not be NULL. All elements will be popped off if no element on stack with that name*/
/*caller must check that name against stack before calling*/
void pop_elements_up_to(element_stack **st, unsigned char *element_name)
{
	element_node *curr_element;
	int found_element = 0;

	assert(element_name != NULL);
	
	while(*st != NULL)
	{
		curr_element = open_element_stack_top(*st);

		if(strcmp(curr_element->name, element_name) == 0)
		{
			found_element = 1;
		}

		open_element_stack_pop(st);

		if(found_element == 1)
		{
			break;
		}
	}
}


/*pop elements off the stack up to and including element e, e must not be NULL*/
/*all elements will be popped off if e is not in stack*/
/*caller must check to make sure e is in stack befoer calling*/
void pop_ele_up_to(element_stack **st, element_node *e)
{
	element_node *curr_element;
	int found_element = 0;

	assert(e != NULL);
	
	while(*st != NULL)
	{
		curr_element = open_element_stack_top(*st);

		if(curr_element == e)
		{
			found_element = 1;
		}

		open_element_stack_pop(st);

		if(found_element == 1)
		{
			break;
		}
	}

}

/*if element_stack is empty, return NULL, otherwise return the top element*/
element_node *open_element_stack_top(element_stack *st)

{
	if(st != NULL)
	{
		return st->e;
	}
	else
	{
		return NULL;
	}
}


/*returns 1 if e is in st AND is the last element in st, otherwise returns 0.*/  
int is_last_element_in_stack(element_stack *st, element_node *e)
{
	assert(e != NULL);

	while(st != NULL)
	{
		if((st->e == e) && (st->tail == NULL))
		{
			return 1;
		}
		
		st = st->tail;
	}

	return 0;
}


/*if e is on the stack, return 1, otherwise return 0*/
int is_on_element_stack(element_stack *st, element_node *e)
{
	assert(e != NULL);

	while(st != NULL)
	{
		if(st->e == e)
		{
			return 1;
		}

		st = st->tail;
	}

	return 0;
}


/*if st is NULL, return NULL, otherwise return a pointer to the previous node.(it could be NULL)*/
element_stack *previous_stack_node(element_stack *st)
{
	if(st != NULL)
	{
		return st->tail;
	}
	else
	{
		return NULL;
	}
}


/*e must not be NULL, and must be in the stack, caller should check before calling*/
/*if st is NULL, return NULL, otherwise return the stack node before e, (it could be NULL)*/
element_stack *stack_node_before_element(element_stack *st, element_node *e)
{
	assert(e != NULL);

	while(st != NULL)
	{
		if(st->e == e)
		{
			return st->tail;
		}

		st = st->tail;
	}

	return NULL;
}


/*remove element e from the stack, if it is in the stack*/
void remove_element_from_stack(element_stack **st, element_node *e)
{
	element_stack *temp_stack_node, *last_node;

	assert(e != NULL);

	if((*st)->e == e)
	{
		temp_stack_node = *st;

		*st = (*st)->tail;

		free(temp_stack_node);
	}
	else
	{
		temp_stack_node = (*st)->tail;
		last_node = *st;
		while(temp_stack_node != NULL)
		{
			if(temp_stack_node->e == e)
			{
				last_node->tail = temp_stack_node->tail;
			
				free(temp_stack_node);

				break;
			}
		
			last_node = temp_stack_node;
			temp_stack_node = previous_stack_node(temp_stack_node);
		}
	}
}


/*replace element e with element replacement, if e is in the stack*/
void replace_element_in_stack(element_stack *st, element_node *e, element_node *replacement)
{
	assert(e != NULL);
	assert(replacement != NULL);

	while(st != NULL)
	{
		if(st->e == e)
		{
			st->e = replacement;
			break;
		}

		st = st->tail;
	}
}


/*insert element e into the stack immediately below element existing_e*/
/*if existing_e in not in stack, then e will not be inserted at all*/
void insert_into_stack_below_element(element_stack **st, element_node *e, element_node *existing_e)
{
	element_stack *temp_stack_node, *last_node;

	assert(e != NULL);
	assert(existing_e != NULL);

	if((*st)->e == existing_e)
	{
		open_element_stack_push(st, e);
	}
	else
	{
		element_stack *new_stack_node;
		
		temp_stack_node = (*st)->tail;
		last_node = *st;
		while(temp_stack_node != NULL)
		{
			if(temp_stack_node->e == existing_e)
			{
				new_stack_node = malloc(sizeof(element_stack));
				new_stack_node->e = e;
				new_stack_node->tail = temp_stack_node;

				last_node->tail = new_stack_node;

				break;
			}
		
			last_node = temp_stack_node;
			temp_stack_node = previous_stack_node(temp_stack_node);
		}
	}
}

/************************************************************/
