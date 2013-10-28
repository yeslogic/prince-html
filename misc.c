// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "misc.h"
#include "util.h"


/*-----------------------------------------------------------------*/		
token_list *replacement_for_null_char(void)
{
	token_list *tk_list;
	unsigned char byte_seq[5];
	int i, len;

	utf8_byte_sequence(0xFFFD, byte_seq);
	len = strlen(byte_seq);
	tk_list = NULL;
	for(i = 0; i < len; i++)
	{
		tk_list = html_token_list_cons(create_character_token(byte_seq[i]), tk_list);
	}
	
	return tk_list;
}


/*-----------------------------------------------------------------*/	
/*returns 1 if an element node is found with the same name as element_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_scope(element_stack *st, unsigned char *element_name, element_node **match_node)
{	
	element_node *temp_element_node;

	assert(element_name != NULL);

	while(st != NULL)
	{
		temp_element_node = open_element_stack_top(st);

		if(strcmp(temp_element_node->name, element_name) == 0)
		{
			*match_node = temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "applet") == 0)) ||
			    ((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "caption") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "html") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "table") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "td") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "th") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "marquee") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "object") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mi") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mo") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mn") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "ms") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mtext") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "annotation-xml") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "foreignObject") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "desc") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "title") == 0)))
		{
			return 0;
		}
		else
		{
			st = st->tail;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------*/	
/*returns 1 if an element node is found with the same name as element_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_list_item_scope(element_stack *st, unsigned char *element_name, element_node **match_node)
{	
	element_node *temp_element_node;

	assert(element_name != NULL);

	while(st != NULL)
	{
		temp_element_node = open_element_stack_top(st);

		if(strcmp(temp_element_node->name, element_name) == 0)
		{
			*match_node = temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "applet") == 0)) ||
			    ((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "caption") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "html") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "table") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "td") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "th") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "marquee") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "object") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "ol") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "ul") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mi") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mo") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mn") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "ms") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mtext") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "annotation-xml") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "foreignObject") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "desc") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "title") == 0)))
		{
			return 0;
		}
		else
		{
			st = st->tail;
		}
	}
	return 0;
}


/*-----------------------------------------------------------------*/	
/*returns 1 if an element node is found with the same name as element_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_button_scope(element_stack *st, unsigned char *element_name, element_node **match_node)
{
	element_node *temp_element_node;

	assert(element_name != NULL);

	while(st != NULL)
	{
		temp_element_node = open_element_stack_top(st);

		if(strcmp(temp_element_node->name, element_name) == 0)
		{
			*match_node = temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "applet") == 0)) ||
			    ((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "caption") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "html") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "table") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "td") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "th") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "marquee") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "object") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "button") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mi") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mo") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mn") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "ms") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "mtext") == 0)) ||
				((temp_element_node->name_space == MATHML) && (strcmp(temp_element_node->name, "annotation-xml") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "foreignObject") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "desc") == 0)) ||
				((temp_element_node->name_space == SVG) && (strcmp(temp_element_node->name, "title") == 0)))
		{
			return 0;
		}
		else
		{
			st = st->tail;
		}
	}
	return 0;
}


/*-----------------------------------------------------------------*/	
/*returns 1 if an element node is found with the same name as element_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_table_scope(element_stack *st, unsigned char *element_name, element_node **match_node)
{	
	element_node *temp_element_node;

	assert(element_name != NULL);

	while(st != NULL)
	{
		temp_element_node = open_element_stack_top(st);

		if(strcmp(temp_element_node->name, element_name) == 0)
		{
			*match_node = temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "html") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "table") == 0)))
		{
			return 0;
		}
		else
		{
			st = st->tail;
		}
	}
		
	return 0;
}

/*-----------------------------------------------------------------*/	
/*returns 1 if an element node is found with the same name as element_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_select_scope(element_stack *st, unsigned char *element_name, element_node **match_node)
{	
	element_node *temp_element_node;

	assert(element_name != NULL);

	while(st != NULL)
	{
		temp_element_node = open_element_stack_top(st);

		if(strcmp(temp_element_node->name, element_name) == 0)
		{

			*match_node = temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "optgroup") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "option") == 0)))
	
		{
			st = st->tail;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------*/
/*returns 1 if the element is in the special category, otherwise return 0*/
int is_in_special_category(element_node *e)
{
	assert(e!= NULL);

	if(
	   ((e->name_space == HTML) &&
	   ((strcmp(e->name, "address") == 0) ||
	    (strcmp(e->name, "applet") == 0) ||
	    (strcmp(e->name, "area") == 0) ||
	    (strcmp(e->name, "article") == 0) ||
	    (strcmp(e->name, "aside") == 0) ||
	    (strcmp(e->name, "base") == 0) ||
	    (strcmp(e->name, "basefont") == 0) ||
	    (strcmp(e->name, "bgsound") == 0) ||
	    (strcmp(e->name, "blockquote") == 0) ||
	    (strcmp(e->name, "body") == 0) ||
	    (strcmp(e->name, "br") == 0) ||
	    (strcmp(e->name, "button") == 0) ||
	    (strcmp(e->name, "caption") == 0) ||
	    (strcmp(e->name, "center") == 0) ||
	    (strcmp(e->name, "col") == 0) ||
	    (strcmp(e->name, "colgroup") == 0) ||
	    (strcmp(e->name, "command") == 0) ||
	    (strcmp(e->name, "dd") == 0) ||
	    (strcmp(e->name, "details") == 0) ||
	    (strcmp(e->name, "dir") == 0) ||
	    (strcmp(e->name, "div") == 0) ||
	    (strcmp(e->name, "dl") == 0) ||
	    (strcmp(e->name, "dt") == 0) ||
	    (strcmp(e->name, "embed") == 0) ||
	    (strcmp(e->name, "fieldset") == 0) ||
	    (strcmp(e->name, "figcaption") == 0) ||
	    (strcmp(e->name, "figure") == 0) ||
	    (strcmp(e->name, "footer") == 0) ||
	    (strcmp(e->name, "form") == 0) ||
	    (strcmp(e->name, "frame") == 0) ||
	    (strcmp(e->name, "frameset") == 0) ||
	    (strcmp(e->name, "h1") == 0) ||
	    (strcmp(e->name, "h2") == 0) ||
	    (strcmp(e->name, "h3") == 0) ||
	    (strcmp(e->name, "h4") == 0) ||
	    (strcmp(e->name, "h5") == 0) ||
	    (strcmp(e->name, "h6") == 0) ||
	    (strcmp(e->name, "head") == 0) ||
	    (strcmp(e->name, "header") == 0) ||
	    (strcmp(e->name, "hgroup") == 0) ||
	    (strcmp(e->name, "hr") == 0) ||
	    (strcmp(e->name, "html") == 0) ||
	    (strcmp(e->name, "iframe") == 0) ||
	    (strcmp(e->name, "img") == 0) ||
	    (strcmp(e->name, "input") == 0) ||
	    (strcmp(e->name, "isindex") == 0) ||
	    (strcmp(e->name, "li") == 0) ||
	    (strcmp(e->name, "link") == 0) ||
	    (strcmp(e->name, "listing") == 0) ||
	    (strcmp(e->name, "marquee") == 0) ||
	    (strcmp(e->name, "menu") == 0) ||
	    (strcmp(e->name, "meta") == 0) ||
	    (strcmp(e->name, "nav") == 0) ||
	    (strcmp(e->name, "noembed") == 0) ||
	    (strcmp(e->name, "noframes") == 0) ||
	    (strcmp(e->name, "noscript") == 0) ||
	    (strcmp(e->name, "object") == 0) ||
	    (strcmp(e->name, "ol") == 0) ||
	    (strcmp(e->name, "p") == 0) ||
	    (strcmp(e->name, "param") == 0) ||
	    (strcmp(e->name, "plaintext") == 0) ||
	    (strcmp(e->name, "pre") == 0) ||
	    (strcmp(e->name, "script") == 0) ||
	    (strcmp(e->name, "section") == 0) ||
	    (strcmp(e->name, "select") == 0) ||
	    (strcmp(e->name, "style") == 0) ||
	    (strcmp(e->name, "summary") == 0) ||
	    (strcmp(e->name, "table") == 0) ||
	    (strcmp(e->name, "tbody") == 0) ||
	    (strcmp(e->name, "td") == 0) ||
	    (strcmp(e->name, "textarea") == 0) ||
	    (strcmp(e->name, "tfoot") == 0) ||
	    (strcmp(e->name, "th") == 0) ||
	    (strcmp(e->name, "thead") == 0) ||
	    (strcmp(e->name, "title") == 0) ||
	    (strcmp(e->name, "tr") == 0) ||
	    (strcmp(e->name, "ul") == 0) ||
	    (strcmp(e->name, "wbr") == 0) ||
	    (strcmp(e->name, "xmp") == 0)))
		||
	    ((e->name_space == MATHML) && 
		 ((strcmp(e->name, "mi") == 0) ||
		  (strcmp(e->name, "mo") == 0) ||
		  (strcmp(e->name, "mn") == 0) ||
		  (strcmp(e->name, "ms") == 0) ||
		  (strcmp(e->name, "mtext") == 0) ||
		  (strcmp(e->name, "annotation-xml") == 0)))
		||
	    ((e->name_space == SVG) && 
		 ((strcmp(e->name, "foreignObject") == 0) ||
		  (strcmp(e->name, "desc") == 0) ||
		  (strcmp(e->name, "title") == 0)))  
		)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*-----------------------------------------------------------------*/
/*This function could pop elements off the stack of open elements.
  The return value must be assigned to the current_node everytime
  the function is called.
  The caller should pass in NULL for to_exclude if no element is to
  be excluded from the list.
  */
element_node *generate_implied_end_tags(element_stack **st_ptr, unsigned char *to_exclude)
{
	element_node *curr_element_node;

	assert(*st_ptr != NULL);
	
	curr_element_node = open_element_stack_top(*st_ptr);

	while((strcmp(curr_element_node->name, "dd") == 0) ||
		  (strcmp(curr_element_node->name, "dt") == 0) ||
	      (strcmp(curr_element_node->name, "li") == 0) ||
	      (strcmp(curr_element_node->name, "option") == 0) ||
	      (strcmp(curr_element_node->name, "optgroup") == 0) ||
	      (strcmp(curr_element_node->name, "p") == 0) ||
	      (strcmp(curr_element_node->name, "rp") == 0) ||
	      (strcmp(curr_element_node->name, "rt") == 0))
	{
		if((to_exclude != NULL) && (strcmp(curr_element_node->name, to_exclude) == 0))
		{
			break;
		}

		open_element_stack_pop(st_ptr);

		curr_element_node = open_element_stack_top(*st_ptr);
	}

	return open_element_stack_top(*st_ptr);
}

/*-----------------------------------------------------------------*/
void add_child_to_foster_parent(element_stack *o_e_st, node *child_node)
{
	element_node *temp_element;
	element_stack *temp_stack = o_e_st;

	assert(child_node != NULL);
	
	while(temp_stack != NULL)
	{
		temp_element = open_element_stack_top(temp_stack);

		if(strcmp(temp_element->name, "table") == 0)
		{
			if(child_node->type == TEXT_N)
			{
				if((temp_element->previous_sibling != NULL) && 
				   (temp_element->previous_sibling->type == TEXT_N))
				{
					int len;
					unsigned char *str1;
					text_node *t1 = (text_node *)temp_element->previous_sibling;
					text_node *t2 = (text_node *)child_node;

					str1 = t1->text_data;

					len = strlen(t1->text_data) + strlen(t2->text_data);	//text_data is never NULL.

					t1->text_data = malloc(len + 1);

					t1->text_data = strcpy(t1->text_data, str1);
					t1->text_data = strcat(t1->text_data, t2->text_data);
	
					free(str1);
					free_node(child_node);
				}
				else
				{
					insert_child_node(temp_element->parent, child_node, (node *)temp_element);
				}
			}
			else if(child_node->type == ELEMENT_N)
			{
				insert_child_node(temp_element->parent, child_node, (node *)temp_element);
			}
			else
			{
				;
			}
			break;
		}
		else if(strcmp(temp_element->name, "html") == 0)
		{
			add_child_node(temp_element, child_node);
			break;
		}
		else
		{
			temp_stack = previous_stack_node(temp_stack);
		}
	}
}


/*-----------------------------------------------------------------*/
//function returns the first "table" or "template"element seen on the stack, 
//or the "html" element if neither of them is found.
//it is unlikely to return a NULL. (no need for caller to check for NULL return value)
element_node *back_to_table_context(element_stack **st_ptr)
{
 	element_node *temp_element_node;
				
	while(*st_ptr != NULL)
	{
		temp_element_node = open_element_stack_top(*st_ptr);

		if((strcmp(temp_element_node->name, "table") == 0) ||
		   (strcmp(temp_element_node->name, "template") == 0) ||
		   (strcmp(temp_element_node->name, "html") == 0))
		{
			return temp_element_node;
		}
		else
		{
			open_element_stack_pop(st_ptr);
		}
	}
	
	return NULL;			
}


/*-----------------------------------------------------------------*/
//function returns the first "tbody", "tfoot", "thead" or "template" element seen on the stack. 
//or the "html" element if none of the above elements is found.
//it is unlikely to return a NULL. (no need for caller to check for NULL return value)
element_node *back_to_table_body_context(element_stack **st_ptr)
{
 	element_node *temp_element_node;

	while(*st_ptr != NULL)
	{
		temp_element_node = open_element_stack_top(*st_ptr);

		if((strcmp(temp_element_node->name, "tbody") == 0)||
		   (strcmp(temp_element_node->name, "tfoot") == 0)||
		   (strcmp(temp_element_node->name, "thead") == 0)||
		   (strcmp(temp_element_node->name, "template") == 0)||
		   (strcmp(temp_element_node->name, "html") == 0))
		{
			return temp_element_node;
		}
		else
		{
			open_element_stack_pop(st_ptr);
		}
	}
	
	return NULL;			
}

/*-----------------------------------------------------------------*/
//function returns the first "tr" or "template" element seen on the stack,
//or the "html" element, if neither element is found.
//it is unlikely to return a NULL. (no need for caller to check for NULL return value)
element_node *back_to_table_row_context(element_stack **st_ptr)
{
 	element_node *temp_element_node;

	while((*st_ptr) != NULL)
	{
		temp_element_node = open_element_stack_top(*st_ptr);

		if((strcmp(temp_element_node->name, "tr") == 0) ||
		   (strcmp(temp_element_node->name, "template") == 0) ||
		   (strcmp(temp_element_node->name, "html") == 0))
		{
			return temp_element_node;
		}
		else
		{
			open_element_stack_pop(st_ptr);
		}
	}
	
	return NULL;	

}


/*-----------------------------------------------------------------*/
/*returns the element node that has the name element_name in the stack*/
/*returns NULL if no such element exists*/
element_node *get_node_by_name(element_stack *st, unsigned char *element_name)
{
	element_node *e;

	assert(element_name != NULL);

	while(st != NULL)
	{
		e = open_element_stack_top(st);

		if(strcmp(e->name, element_name) == 0)
		{
			return e;
		}

		st = previous_stack_node(st);
	}

	return NULL;

}


