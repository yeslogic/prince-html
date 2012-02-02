#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "format.h"



/*-----------------------------------------------------------------------------------------*/
active_formatting_list *active_formatting_list_add_element(element_node *formatting_element,
														   active_formatting_list *tail)
{
	active_formatting_list *list, *curr_list;
	int identical_node_count;

	curr_list = tail;
	identical_node_count = 0;
	while(curr_list != NULL)
	{
		if(curr_list->type == MARKER)
		{
			break;
		}

		if(element_compare(curr_list->formatting_element, formatting_element) == 0)
		{
			identical_node_count += 1;

			if(identical_node_count == 3) 
			{
				//remove the third identical node from the list:
				curr_list->head->tail = curr_list->tail;
				if(curr_list->tail != NULL)
				{
					curr_list->tail->head = curr_list->head;
				}
				free(curr_list);

				break;
			}
		}

		curr_list = curr_list->tail;
	}
	
	//add formatting_element to the list:

	list = (active_formatting_list *)malloc(sizeof(active_formatting_list));

	list->type = FORMATTING_ELEMENT;
	list->formatting_element = formatting_element;
	list->tail = tail;
	list->head = NULL;

	if(tail != NULL)
	{
		tail->head = list;
	}

	return list;
}


active_formatting_list *active_formatting_list_add_marker(active_formatting_list *tail)
{
	active_formatting_list *list;

	list = (active_formatting_list *)malloc(sizeof(active_formatting_list));

	list->type = MARKER;
	list->formatting_element = NULL;
	list->tail = tail;
	list->head = NULL;

	if(tail != NULL)
	{
		tail->head = list;
	}

	return list;
}


/*return pointer to element e if element e is in the list, up to the last marker (if any), or the end if no marker in list */
/*return NULL, otherwise*/
element_node *is_in_active_formatting_list(active_formatting_list *list, element_node *e)
{
	assert(e != NULL);

	while(list != NULL)
	{
		if(list->type == MARKER)
		{
			return NULL;
		}
		else
		{
			if(list->formatting_element == e)
			{
				return list->formatting_element;
			}
			else
			{
				list = list->tail;
			}
		}
	}

	return NULL;
}


/*return pointer to the element if there is an element with the name tag_name in the list, up to the last marker (if any), or the end if no marker in list */
/*return NULL, otherwise*/
element_node *get_formatting_element(active_formatting_list *list, unsigned char *tag_name)
{
	assert(tag_name != NULL);

	while(list != NULL)
	{
		if(list->type == MARKER)
		{
			return NULL;
		}
		else
		{
			if(strcmp(list->formatting_element->name, tag_name) == 0)
			{
				return list->formatting_element;
			}
			else
			{
				list = list->tail;
			}
		}
	}

	return NULL;
}

/*return 1 if successful, otherwise return 0*/
/*successful means element_to_be_replaced is in the list, before any marker, (if any), or the end if no marker in list */
/*and it has been replaced with the "new_element"*/
int replace_element_in_active_formatting_list(active_formatting_list *list, 
													element_node *element_to_be_replaced,
													element_node *new_element)
{
	assert((element_to_be_replaced != NULL) && (new_element != NULL));

	while(list != NULL)
	{
		if(list->type == MARKER)
		{
			return 0;
		}

		if(list->type == FORMATTING_ELEMENT)
		{
			if(list->formatting_element == element_to_be_replaced)
			{
				list->formatting_element = new_element;
				return 1;
			}
		}

		list = list->tail;
	}
	return 0;
}


/*insert a bookmark at the place immediately after the element_before_bookmark and return the list*/
active_formatting_list *insert_bookmark_in_active_formatting_list(active_formatting_list *list, element_node *element_before_bookmark, active_formatting_list **book_mark)
{
	active_formatting_list *bookmark, *temp_list;
	element_node *bookmark_element;

	assert((list != NULL) && (element_before_bookmark != NULL));

	bookmark_element = create_element_node("bookmark", NULL, HTML);

	bookmark = (active_formatting_list *)malloc(sizeof(active_formatting_list));	
	bookmark->type = FORMATTING_ELEMENT;
	bookmark->formatting_element = bookmark_element;

	if(list->formatting_element == element_before_bookmark)
	{
		list->head = bookmark;
		bookmark->tail = list;
		bookmark->head = NULL;

		*book_mark = bookmark;
		return bookmark;	//return list with bookmark inserted at the head.
	}

	temp_list = list;

	temp_list = temp_list->tail;
	while(temp_list != NULL)
	{
		if(temp_list->formatting_element == element_before_bookmark)
		{
			temp_list->head->tail = bookmark;
			bookmark->head = temp_list->head;

			bookmark->tail = temp_list;
			temp_list->head = bookmark;

			*book_mark = bookmark;
			return list;		//return list with bookmark inserted.
		}
		
		temp_list = temp_list->tail;
	}

	*book_mark = NULL;
	return list;		//return list with no bookmark inserted.

}

/*return the list with the bookmark removed*/
active_formatting_list *remove_bookmark_from_active_formatting_list(active_formatting_list *list, active_formatting_list *book_mark)
{
	assert((list != NULL) && (book_mark != NULL));

	if(list == book_mark)	//bookmark is at the head of the list
	{
		list->tail->head = NULL;

		free(book_mark->formatting_element->name);
		free(book_mark->formatting_element);
		free(book_mark);

		return list->tail;
	}
	else		//bookmark is not the head of the list
	{
		book_mark->head->tail = book_mark->tail;

		if(book_mark->tail != NULL)
		{
			book_mark->tail->head = book_mark->head;
		}

		free(book_mark->formatting_element->name);
		free(book_mark->formatting_element);
		free(book_mark);

		return list;
	}
}


/*if element e is in list, up to the last marker (if any), or the end if no marker in list*/
/*then, remove it from the list, return the list with it removed.*/
/*otherwise just return the list unmodified*/
active_formatting_list *remove_element_from_active_formatting_list(active_formatting_list *list, element_node *e)
{
	active_formatting_list *temp_list = list;

	assert(e != NULL);

	while(temp_list != NULL)
	{
		if(temp_list->type == MARKER)
		{
			return list;	//up to the last marker, but have not found the "element", just return the original list.
		}
		else
		{
			if(temp_list->formatting_element == e)	//have found the element on the list
			{
				if(temp_list->head == NULL)		//the node to be removed is the head of the list, so just return its tail.
				{
					active_formatting_list *list_tail = temp_list->tail;

					free(temp_list);

					if(list_tail != NULL)
					{
						list_tail->head = NULL;		//make it the head of the list
					}

					return list_tail;
				}
				else		//the node to be removed is not the head of the list.
				{
					temp_list->head->tail = temp_list->tail;
					if(temp_list->tail != NULL)
					{
						temp_list->tail->head = temp_list->head;
					}

					return list;	//just return the origianl list with the "element" removed
				}
			}
			else
			{
				temp_list = temp_list->tail;	//keep going doen the list.
			}
		}
	}

	return list;	//list exhausted, but "element" not found, just return itself;
}


/*returns 0 if e1 and e2 have the identical tag name, namespace and attributes'
  otherwise return 1.*/
int element_compare(element_node *e1, element_node *e2)
{
	if((strcmp(e1->name, e2->name) == 0) &&
	   (e1->name_space == e2->name_space) &&
	   (attr_list_comp(e1->attributes, e2->attributes) == 0))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/*returns 1 if e is a node on the path from current_node up to root.
  otherwise return 0. 
*/
int is_open_element(node *root, node* current_node, element_node *e)
{
	while(current_node != root)
	{	
		if(current_node == (node *)e)
		{
			return 1;
		}

		current_node = current_node->parent;
	}

	return 0;
}


/*return the new current node after reconstructing the active formatting list*/
/*or return the old current node if nothing to be reconstructed*/
node *reconstruct_active_formatting_elements(active_formatting_list *list, node *root, node *current_node)
{
	if(list == NULL)
	{
		return current_node;	//nothing to be reconstructed
	}
	else
	{
		active_formatting_list *entry;
		element_node *entry_element, *new_element;
		node *curr_node = current_node;;

		if((list->type == MARKER) || 
			((list->type == FORMATTING_ELEMENT) && (is_open_element(root, current_node, list->formatting_element) == 1)))
		{
			return current_node;		//nothing to be reconstructed
		}

		entry = list;

		while((entry->tail) != NULL)		//loop until there are no entries before "entry", then jump to step 8 in the spec, p648.
		{
			entry = entry->tail;	//let "entry" be the entry one earlier than "entry" in the list of active formatting elements .
			
			//if entry is neither a marker nor an element that is also in the stack of open elements , go back to beginning of loop.
			if((entry->type == MARKER) || 
				((entry->type == FORMATTING_ELEMENT) && (is_open_element(root, current_node, entry->formatting_element) == 1)))
			{
				//let "entry" be the element one later than "entry" in the list of active formatting elements.
				entry = entry->head;
				break;
			}
		}

		//after the loop, "entry" should be:
		//last entry in the list:  entry->tail == NULL
		//or, the entry before a marker
		//or, the entry before an open element.
			
		//Step 8.
		do
		{
			entry_element = entry->formatting_element;	//"entry" could not be a marker, so this could not be NULL.

			new_element = create_element_node(entry_element->name, entry_element->attributes, entry_element->name_space);

			//Append new element to the current node and push it onto the stack of open elements so that it is the new current node .
			add_child_node(curr_node, (node *)new_element);
			curr_node = (node *)new_element;
			
			//Replace the entry for entry in the list with an entry for new element.
			entry->formatting_element = new_element;

			entry = entry->head;

		}while(entry != NULL);
		
		return curr_node;
	}
}


/*clears the active formatting list up to (and including) the last marker*/
/*or until the list is empty*/
active_formatting_list *clear_list_up_to_last_marker(active_formatting_list *list)
{
	while(list != NULL)
	{
		if(list->type == MARKER)
		{
			active_formatting_list *tail;

			tail = list->tail;
			if(tail != NULL)
			{
				tail->head = NULL;
			}
			
			free(list);
			return tail; 
		}
		else
		{
			active_formatting_list *temp;

			temp = list;

			list = list->tail;
			if(list != NULL)
			{
				list->head = NULL;
			}

			free(temp);
		}
	}

	return NULL;
}


void print_active_formatting_list(active_formatting_list *list)
{
	if(list != NULL)
	{
		if(list->type == MARKER)
		{	
			printf("Node type: MARKER\n\n");
		}
		else
		{
			printf("Node type: FORMATTING_ELEMENT\n");
			printf("Element name: %s\n\n", list->formatting_element->name);
		}

		print_active_formatting_list(list->tail);
	}
}

/*-----------------------------------------------------------------------------------------*/

//the furthest block is the topmost node in the stack of open elements that is lower in
//the stack than the formatting element, and is an element in the special category.
//if there is one, return a pointer to it, otherwise return NULL.

element_node *get_furthest_block(node *root, node *current_node, element_node *formatting_ele)
{
	element_node *topmost_lower_than_formatting_ele;

	if(is_open_element(root, current_node, formatting_ele) == 0)
	{
		return NULL;	//formatting_ele in not in the stack of open elements.
	}

	if(current_node == (node *)formatting_ele)
	{
		return NULL;	//no nodes lower than formatting_ele in the stack.
	}

	topmost_lower_than_formatting_ele = NULL;
	while(current_node != (node *)formatting_ele)
	{	
		if(current_node->type == ELEMENT_N)
		{
			if(is_in_special_category((element_node *)current_node) == 1)
			{
				topmost_lower_than_formatting_ele = (element_node *)current_node;

				//the topmost element below formatting_ele that is in the special category
				//will be the one assigned to topmost_lower_than_formatting_ele at the end
				//if there is none satisfies the condition, it will be NULL.
			}
		}

		current_node = current_node->parent;
	}

	return topmost_lower_than_formatting_ele;
}




/*---------------------------//adoption agency algorithm--------------------------------------*/
void adoption_agency_algorithm(node *root, active_formatting_list **list_ptr, node **current_node_ptr, 
								unsigned char *formatting_element_name)
{
	element_node *formatting_ele;
	node *match_node;
	int i, j;

	//outer loop
	for(i = 0; i < 8; i++)
	{
		formatting_ele = get_formatting_element(*list_ptr, formatting_element_name);
		if(formatting_ele == NULL)
		{
			//there is no such node, abort these steps and instead act as described in the "any other end tag" entry below.
			/*-------------------------------------------------------*/
			node *temp_node = *current_node_ptr;
						
			if(temp_node->type != ELEMENT_N)
			{
				temp_node = temp_node->parent;
			}

			while(temp_node != root)
			{
				element_node *temp_element = (element_node *)temp_node;
						
				if(strcmp(temp_element->name, formatting_element_name) == 0)
				{
					//pop all the nodes from current_node upward, including "temp_element"
					*current_node_ptr = temp_element->parent;
					break;	 
				}
				else if(is_in_special_category(temp_element) == 1)
				{
					break;	//parse error, ignore the token
				}
				else
				{
					temp_node = temp_node->parent;
				}
			}

			/*--------------------------------------------------------*/
			break;
		}
		else if(is_open_element(root, *current_node_ptr, formatting_ele) == 0)
		{
			//there is such a node, but that node is not in the stack of open elements
			//this is a parse error ; remove the element from the list, and abort these steps.
			*list_ptr = 
			remove_element_from_active_formatting_list(*list_ptr, formatting_ele);
			break;
		}
		else if(has_element_in_scope(root, *current_node_ptr, formatting_ele->name, &match_node) == 0)
		{
			//there is such a node, and that node is also in the stack of open elements , 
			//but the element is not in scope, 
			//then this is a parse error ; ignore the token, and abort these steps.
			break;
		}
		else
		{
			element_node *furthest_block;

			furthest_block = get_furthest_block(root, *current_node_ptr, formatting_ele);

			if(furthest_block == NULL)
			{
				//there is no furthest block, then the UA must skip the subsequent steps and instead just pop
				//all the nodes from the bottom of the stack of open elements , from the current node up
				//to and including the formatting element, and remove the formatting element from the list of
				//active formatting elements
				*current_node_ptr = formatting_ele->parent;
				*list_ptr = remove_element_from_active_formatting_list(*list_ptr, formatting_ele);
				break;		//breaking out of the outer loop
			}
			else
			{
				element_node *common_ancestor = (element_node *)formatting_ele->parent;
				element_node *this_node = furthest_block, *last_node = furthest_block;
				element_node *new_element, *new_element_for_formatting_ele;
				active_formatting_list *book_mark;
								
				*list_ptr = insert_bookmark_in_active_formatting_list(*list_ptr, formatting_ele, &book_mark);

				//inner loop
				for(j = 0; j < 3; j++)
				{
					this_node = (element_node *)this_node->parent;

					if(is_in_active_formatting_list(*list_ptr, this_node) == NULL)
					{
						//if this_node is not in the list of active formatting elements, then remove this_node from the
						//stack of open elements and then go back to the step labeled "inner loop".
						//we basically skip this_node and go to the next node up, in next loop.
						;
					}
					else if(this_node == formatting_ele)
					{
						//if node is the formatting element, then go to the next step in the overall algorithm.
						break;	//breaking out of the inner loop
					}
					else
					{
						new_element = create_element_node(this_node->name, this_node->attributes, this_node->name_space);
									
						//replace entry for this_node with the new_element.
						//this_node must be in the list of active formatting elements, at this point.
						replace_element_in_active_formatting_list(*list_ptr, this_node, new_element);
										
						if(last_node == furthest_block)
						{
							*list_ptr = remove_bookmark_from_active_formatting_list(*list_ptr, book_mark);

							*list_ptr = insert_bookmark_in_active_formatting_list(*list_ptr, new_element, &book_mark);
						}

						//Insert last_node into new_element, first removing it from its previous parent node if any.
						remove_child_node_from_parent((node *)last_node);
										
						add_child_node((node *)new_element, (node *)last_node);

						last_node = new_element;

					}
				}

				remove_child_node_from_parent((node *)last_node);

				if((strcmp(common_ancestor->name, "table") == 0) ||
						  (strcmp(common_ancestor->name, "tbody") == 0) ||
						  (strcmp(common_ancestor->name, "tfoot") == 0) ||
					      (strcmp(common_ancestor->name, "thead") == 0) ||
			              (strcmp(common_ancestor->name, "tr") == 0))
				{
					add_child_to_foster_parent(root, (node *)common_ancestor, (node *)last_node);		
				}
				else
				{
					add_child_node((node *)common_ancestor, (node *)last_node);
				}

				//Create an element for the token for which the formatting element was created.
				new_element_for_formatting_ele = create_element_node(formatting_ele->name, 
																	  formatting_ele->attributes, 
																	  formatting_ele->name_space);

				//Take all of the child nodes of the furthest block and append them to the element created.
						
				if(furthest_block->first_child != NULL)
				{
					node *child_node = furthest_block->first_child;
					node *next_sibling_of_child_node;

					while(child_node != NULL)
					{
						next_sibling_of_child_node = child_node->next_sibling;

						add_child_node((node *)new_element_for_formatting_ele, child_node);
										
						child_node = next_sibling_of_child_node;;
					}

					furthest_block->first_child = NULL;
					furthest_block->last_child = NULL;
				}

				//Append that new element to the furthest block
				add_child_node((node *)furthest_block, (node *)new_element_for_formatting_ele);
								
				//Remove the formatting element from the list of active formatting elements
				*list_ptr = remove_element_from_active_formatting_list(*list_ptr, formatting_ele);

				//insert the new element into the list of active formatting elements at the position of the aforementioned bookmark.
				free(book_mark->formatting_element->name);
				free(book_mark->formatting_element);

				book_mark->formatting_element = new_element_for_formatting_ele;
								
				//Step 15, make new_element_for_formatting_ele the current_node;
				*current_node_ptr = (node *)new_element_for_formatting_ele;

			}
		}
	}
}
/*-----------------------------------------------------------------*/