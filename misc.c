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
/*returns 1 if target node is found with same name as end_tag_token_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node)
{	
	element_node *temp_element_node;
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	temp_element_node = (element_node *)current_node;

	while((node *)temp_element_node != root)
	{
		if(strcmp(temp_element_node->name, end_tag_token_name) == 0)
		{
			*match_node = (node *)temp_element_node;
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
			temp_element_node = (element_node *)temp_element_node->parent;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------*/	
/*returns 1 if target node is found with same name as end_tag_token_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_list_item_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node)
{	
	element_node *temp_element_node;

	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	temp_element_node = (element_node *)current_node;

	while((node *)temp_element_node != root)
	{
		if(strcmp(temp_element_node->name, end_tag_token_name) == 0)
		{
			*match_node = (node *)temp_element_node;
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
			temp_element_node = (element_node *)temp_element_node->parent;
		}
	}
	return 0;
}


/*-----------------------------------------------------------------*/	
/*returns 1 if target node is found with same name as end_tag_token_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_button_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node)
{	
	element_node *temp_element_node;
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	temp_element_node = (element_node *)current_node;

	while((node *)temp_element_node != root)
	{
		if(strcmp(temp_element_node->name, end_tag_token_name) == 0)
		{
			*match_node = (node *)temp_element_node;
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
			temp_element_node = (element_node *)temp_element_node->parent;
		}
	}
	return 0;
}


/*-----------------------------------------------------------------*/	
/*returns 1 if target node is found with same name as end_tag_token_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_table_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node)
{	
	element_node *temp_element_node;
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	temp_element_node = (element_node *)current_node;

	while((node *)temp_element_node != root)
	{
		if(strcmp(temp_element_node->name, end_tag_token_name) == 0)
		{
			*match_node = (node *)temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "html") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "table") == 0)))
		{
			return 0;
		}
		else
		{
			temp_element_node = (element_node *)temp_element_node->parent;
		}
	}
		
	return 0;
}

/*-----------------------------------------------------------------*/	
/*returns 1 if target node is found with same name as end_tag_token_name, pointer stored in match_node*/
/*otherwise, return 0*/
int has_element_in_select_scope(node *root, node *current_node, unsigned char *end_tag_token_name, node **match_node)
{	
	element_node *temp_element_node;
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	temp_element_node = (element_node *)current_node;

	while((node *)temp_element_node != root)
	{
		if(strcmp(temp_element_node->name, end_tag_token_name) == 0)
		{
			*match_node = (node *)temp_element_node;
			return 1;
		}
		else if(((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "optgroup") == 0)) ||
				((temp_element_node->name_space == HTML) && (strcmp(temp_element_node->name, "option") == 0)))
	
		{
			temp_element_node = (element_node *)temp_element_node->parent;
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
node *generate_implied_end_tags(node *current_node)
{
	element_node *curr_element_node;

	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	curr_element_node = (element_node *)current_node;

	if((strcmp(curr_element_node->name, "dd") == 0) ||
	   (strcmp(curr_element_node->name, "dt") == 0) ||
	   (strcmp(curr_element_node->name, "li") == 0) ||
	   (strcmp(curr_element_node->name, "option") == 0) ||
	   (strcmp(curr_element_node->name, "optgroup") == 0) ||
	   (strcmp(curr_element_node->name, "p") == 0) ||
	   (strcmp(curr_element_node->name, "rp") == 0) ||
	   (strcmp(curr_element_node->name, "rt") == 0))
	{
		return current_node->parent;
	}
	else
	{
		return current_node;
	}
}

/*-----------------------------------------------------------------*/
void add_child_to_foster_parent(node *root, node *current_node, node *child_node)
{
	element_node *curr_element;

	assert(current_node != NULL);
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	while(current_node != root)
	{
		curr_element = (element_node *)current_node;

		if(strcmp(curr_element->name, "table") == 0)
		{
			insert_child_node(current_node->parent, child_node, current_node);
			break;
		}
		else if(strcmp(curr_element->name, "html") == 0)
		{
			add_child_node(current_node, child_node);
			break;
		}
		else
		{
			current_node = current_node ->parent;
		}
	}
}


/*-----------------------------------------------------------------*/
 //current_node must not be root node of the tree.
//function returns the first "table" node in the upward path from the current node,
//or returns the html node, if no "table" node is found.
node *back_to_table_context(node *root, node *current_node)
{
 	element_node *temp_element_node;

	assert(current_node != root);
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}
					
	while(current_node != root)
	{
		temp_element_node = (element_node *)current_node;

		if((strcmp(temp_element_node->name, "table") == 0)||
		   (strcmp(temp_element_node->name, "html") == 0))
		{
			return current_node;
		}
		else
		{
			current_node = current_node->parent;
		}
	}
	
	return NULL;			
}


/*-----------------------------------------------------------------*/
//current_node must not be root node of the tree.
//function returns the first "tbody", "tfoot", or "thead" node in the upward path from the current node,
//or returns the html node, if none of the above nodes is found.
node *back_to_table_body_context(node *root, node *current_node)
{
 	element_node *temp_element_node;

	assert(current_node != root);					
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}
					
	while(current_node != root)
	{
		temp_element_node = (element_node *)current_node;

		if((strcmp(temp_element_node->name, "tbody") == 0)||
		   (strcmp(temp_element_node->name, "tfoot") == 0)||
		   (strcmp(temp_element_node->name, "thead") == 0)||
		   (strcmp(temp_element_node->name, "html") == 0))
		{
			return current_node;
		}
		else
		{
			current_node = current_node->parent;
		}
	}
	
	return NULL;			
}

/*-----------------------------------------------------------------*/
//current_node must not be root node of the tree.
//function returns the first "tr" node in the upward path from the current node,
//or returns the html node, if no "tr" node is found.
node *back_to_table_row_context(node *root, node *current_node)
{
	element_node *temp_element_node;

	assert(current_node != root);
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}
					
	while(current_node != root)
	{
		temp_element_node = (element_node *)current_node;

		if((strcmp(temp_element_node->name, "tr") == 0)||
		   (strcmp(temp_element_node->name, "html") == 0))
		{
			return current_node;
		}
		else
		{
			current_node = current_node->parent;
		}
	}
	
	return NULL;	

}


/*-----------------------------------------------------------------*/
/*returns the element node that has the name node_name and is located between current_node and root*/
/*returns NULL if no such element exists, or node_name is NULL*/
element_node *get_node_by_name(node *root, node *current_node, unsigned char *node_name)
{
	element_node *curr_element_node;
	
	if(node_name == NULL)
	{
		return NULL;
	}

	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	while(current_node != root)
	{
		curr_element_node = (element_node *)current_node;

		if(strcmp(curr_element_node->name, node_name) == 0)
		{	
			return curr_element_node;
		}

		current_node = current_node->parent;
	}

	return NULL;
}


