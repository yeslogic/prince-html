// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)

#include <stdlib.h>

#include "template.h"


void mode_stack_push(mode_stack **mst, insertion_mode m)
{
	mode_stack *new_node = malloc(sizeof(mode_stack));
	
	new_node->m = m;
	new_node->tail = *mst;

	*mst = new_node;
}


/*if mode_stack is empty, return 0, otherwise pop the top item and return 1 */
int mode_stack_pop(mode_stack **mst)
{
	mode_stack *temp_ptr;

	if(*mst != NULL)
	{
		temp_ptr = (*mst)->tail;
		free(*mst);
		*mst = temp_ptr;
		return 1;
	}
	else
	{
		return 0;
	}
}


/*returns 1 is stack is empty, otherwise returns 0  */
int mode_stack_is_empty(mode_stack *mst)
{
	if(mst == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*return the top item in the mode_stack. 
  caller needs to make sure mst is not empty.*/
insertion_mode mode_stack_top(mode_stack *mst)
{
	return mst->m;
}


/*free the stack of insertion modes */
void free_mode_stack(mode_stack *mst)
{
	mode_stack *temp_ptr;

	while(mst != NULL)
	{
		temp_ptr = mst;
		mst = mst->tail;

		free(temp_ptr);
	}
}