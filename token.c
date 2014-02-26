// Copyright (C) 2011-2014 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "token.h"
#include "util.h"


/*------------------------------------------------------------------------*/
token *create_doctype_token(unsigned char ch)
{
	token *tk;
	tk = malloc(sizeof(token));
	
	tk->type = TOKEN_DOCTYPE;
	tk->dt.doctype_name = string_append(NULL, ch);
	tk->dt.doctype_public_identifier = NULL;		//meaning: missing
	tk->dt.doctype_system_identifier = NULL;		//meaning: missing
	tk->dt.force_quirks_flag = OFF;

	return tk;
}

token *create_start_tag_token(unsigned char ch)
{
	token *tk;
	tk = malloc(sizeof(token));
	
	tk->type = TOKEN_START_TAG;
	tk->stt.tag_name = string_append(NULL, ch);
	tk->stt.self_closing_flag = UNSET;
	tk->stt.attributes = NULL;

	return tk;
}

token *create_end_tag_token(unsigned char ch)
{
	token *tk;
	tk = malloc(sizeof(token));
	
	tk->type = TOKEN_END_TAG;
	tk->ett.tag_name = string_append(NULL, ch);

	return tk;
}

token *create_comment_token(void)
{
	token *tk;
	tk = malloc(sizeof(token));
	
	tk->type = TOKEN_COMMENT;
	tk->cmt.comment= string_append(NULL, '\0');

	return tk;
}


token *create_multi_char_token(unsigned char *mch, long char_count)
{
	token *tk;
	tk = malloc(sizeof(token));

	tk->type = TOKEN_MULTI_CHAR;
	tk->mcht.mch = mch;
	tk->mcht.char_count = char_count;

	return tk;
}

token *create_eof_token(void)
{
	token *tk;
	tk = malloc(sizeof(token));
	
	tk->type = TOKEN_EOF;

	return tk;
}

void free_token(token *tk)
{
	//attribute_list *attrs, *temp_ptr;

	if(tk != NULL)
	{
		switch (tk->type)
		{
			case TOKEN_DOCTYPE:
				free(tk->dt.doctype_name);
				free(tk->dt.doctype_public_identifier);
				free(tk->dt.doctype_system_identifier);
				break;

			case TOKEN_START_TAG:
				free(tk->stt.tag_name);
				/*
				attrs = tk->stt.attributes;

				while(attrs != NULL)
				{
					free(attrs->name);
					free(attrs->value);
					
					temp_ptr = attrs;
					attrs = attrs->tail;
					free(temp_ptr);
				}*/
				break;

			case TOKEN_END_TAG:
				free(tk->ett.tag_name);
				break;

			case TOKEN_COMMENT:
				free(tk->cmt.comment);
				break;

			case TOKEN_MULTI_CHAR:
				break;

			case TOKEN_EOF:
				break;
		
			default:
				break;
		}

		free(tk);
	}
}


/*-------------------------------------------------------------*/
void free_attributes(attribute_list *list)
{
	attribute_list *temp_ptr;

	while(list != NULL)
	{
		free(list->name);
		free(list->value);
					
		temp_ptr = list;
		list = list->tail;
		free(temp_ptr);
	}
}
/*-------------------------------------------------------------*/
attribute_list *clone_attributes(attribute_list *list)
{
	attribute_list *clone;
	attribute_list *new_node, *last_node;

	clone = NULL;
	while(list != NULL)
	{	
		new_node = (attribute_list *)malloc(sizeof(attribute_list));
		
		new_node->name = strdup(list->name);
		new_node->value = strdup(list->value);
		new_node->attr_ns = list->attr_ns ;
		new_node->tail = NULL;

		if(clone == NULL)
		{
			clone = new_node;
		}
		else
		{
			last_node->tail = new_node;
		}
		last_node = new_node;
		
		list = list->tail ;
	}
	return clone;
}
/*-------------------------------------------------------------*/
/*add the name-value pair to the end of attr_list*/
attribute_list *html_attribute_list_cons(unsigned char *name, 
										 unsigned char *value,
										 namespace_type attr_name_space,
										 attribute_list *attr_list)

{
	attribute_list *list;

	if((name == NULL) || (value == NULL))
	{
		return attr_list;
	}

	list = (attribute_list *)malloc(sizeof(attribute_list));

	list->name = strdup(name);
	list->value = strdup(value);
	list->attr_ns = attr_name_space;

	list->tail = NULL;


	if(attr_list == NULL)
	{
		return list;
	}
	else
	{
	    attribute_list *temp_ptr = attr_list;

	    while(temp_ptr->tail != NULL)
	    {
			temp_ptr = temp_ptr->tail;
		}
		temp_ptr->tail = list;

		return attr_list;
	}
}


/*-------------------------------------------------------------*/
/*returns 0 if attr1 and attr2 are identical: 
Attr1 and attr2 have the same number of name-value pairs and the name-value pairs are identical.
The order of the name-value pairs does not matter.
Otherwise, return 1.
*/
int attr_list_comp(attribute_list *attr1, attribute_list *attr2)
{
	attribute_list *temp_attr1, *temp_attr2;

	temp_attr1 = attr1;
	temp_attr2 = attr2;

	while(temp_attr1 != NULL)
	{
		if(attribute_in_list(temp_attr1->name, temp_attr1->value, attr2) != 1)
		{
			return 1;		//attr1 and attr2 NOT identical.
		}

		temp_attr1 = temp_attr1->tail;
	}

	while(temp_attr2 != NULL)
	{
		if(attribute_in_list(temp_attr2->name, temp_attr2->value, attr1) != 1)
		{
			return 1;		//attr1 and attr2 NOT identical.
		}

		temp_attr2 = temp_attr2->tail;
	}


	return 0;	//attr1 and attr2 are identical, including the case where both are NULL.
}

/*-------------------------------------------------------------*/
/*returns 1 if name and value is a name-value pair in list.
Otherwise returns 0.*/

int attribute_in_list(unsigned char *name, unsigned char *value, attribute_list *list)
{
	if((name == NULL) || (value == NULL))
	{
		return 0;
	}

	while(list != NULL)
	{
		if((list->name != NULL) && (list->value != NULL))
		{
			if((strcmp(list->name, name) == 0) && (strcmp(list->value, value) == 0))
			{
				return 1;
			}
		}

		list = list->tail;
	}

	return 0;
}


/*-------------------------------------------------------------*/
/*returns 1 if attr_name exists in list, whatever value it might have*/
/*returns 0 if list is NULL, or attr_name does not exist in list*/
int attribute_name_in_list(unsigned char *attr_name, attribute_list *list)
{
	assert(attr_name != NULL);

	while(list != NULL)
	{
		if(list->name != NULL)
		{
			if(strcmp(list->name, attr_name) == 0)
			{
				return 1;
			}
		}

		list = list->tail;
	}

	return 0;

}

/*-------------------------------------------------------------*/
/*return a copy of the value of attr_name if attr_name exists in list and its value is not NULL*/
/*return NULL if list is NULL, or attr_name does not exist in list, or its value is NULL*/
unsigned char *get_attribute_value(unsigned char *attr_name, attribute_list *list)
{
	assert(attr_name != NULL);

	while(list != NULL)
	{
		if(list->name != NULL)
		{
			if(strcmp(list->name, attr_name) == 0)
			{
				if(list->value != NULL)
				{
					unsigned char *attr_value;
				
					attr_value = strdup(list->value);

					return attr_value;
				}
				else
				{
					return NULL;
				}
			}
		}

		list = list->tail;
	}

	return NULL;
}



/*-------------------------------------------------------------*/
/*if list is NULL, return NULL*/
/*otherwise if attr_name exists in list, remove it and return the list with attr_name removed*/
/*if attr_name does not exist in list, return list unmodified*/
attribute_list *remove_attribute(unsigned char *attr_name, attribute_list *list)
{
	assert(attr_name != NULL);

	if(list == NULL)
	{
		return NULL;
	}
	else
	{
		if((list->name != NULL) && (strcmp(list->name, attr_name) == 0))
		{
			attribute_list *list_tail = list->tail;

			free(list->name);
			free(list->value);
			free(list);

			return list_tail;
		}
		else
		{
			attribute_list *previous_node, *temp_list = list;

			previous_node = temp_list;
			temp_list = temp_list->tail;
			
			while(temp_list != NULL)
			{
				if((temp_list->name != NULL) && (strcmp(temp_list->name, attr_name) == 0))
				{
					free(temp_list->name);
					free(temp_list->value);

					previous_node->tail = temp_list->tail;

					free(temp_list);

					break;
				}

				previous_node = temp_list;
				temp_list = temp_list->tail;
			}

			return list;
		}
	}
}

/*-------------------------------------------------------------*/
token_list *html_token_list_nil(void)
{
	return NULL;
}


token_list *html_token_list_cons(token *tk, token_list *tail)
{
	token_list *list;

	list = (token_list *)malloc(sizeof(token_list));

	list->tk = tk;
	list->tail = tail;

	return list;
}

token_list *html_token_list_reverse(token_list *tk_list)
{
	token_list *rev_list = NULL, *temp_ptr;

	while(tk_list != NULL)
	{
		rev_list = html_token_list_cons(tk_list->tk, rev_list);

		temp_ptr = tk_list;
		tk_list = tk_list->tail;

		free(temp_ptr);
	}

	return rev_list;
}

token *html_token_list_get_first_token(token_list *tk_list)
{
	if(tk_list != NULL)
	{
		return tk_list->tk;
	}
	else
	{
		return NULL;
	}
}

token_list *html_token_list_get_tail(token_list *tk_list)
{
	if(tk_list != NULL)
	{
		return tk_list->tail;
	}
	else
	{
			return NULL;
	}
}
/*-------------------------------------------------------------*/
void html_print_token(token *tk)
{
	switch (tk->type)
	{
		case TOKEN_DOCTYPE:
			printf("Token type: DOCTYPE.\n");
			printf("Doctype name: %s\n", tk->dt.doctype_name);
			printf("Public identifier: %s\n", tk->dt.doctype_public_identifier);
			printf("System identifier: %s\n", tk->dt.doctype_system_identifier);
			html_print_force_quirks_flag(tk->dt.force_quirks_flag);
			break;

		case TOKEN_START_TAG:
			printf("Token type: START TAG.\n");
			printf("Start tag name: %s\n", tk->stt.tag_name);
			html_print_self_closing_flag(tk->stt.self_closing_flag);
			html_print_attribute_list(tk->stt.attributes);
			break;

		case TOKEN_END_TAG:
			printf("Token type: END TAG.\n");
			printf("End tag name: %s\n", tk->ett.tag_name);
			break;

		case TOKEN_COMMENT:
			printf("Token type: COMMENT.\n");
			printf("Comment: %s\n", tk->cmt.comment);
			break;

		case TOKEN_MULTI_CHAR:
			printf("Token type: MULTI_CHAR.\n");
			printf("First character: %c\n", *(tk->mcht.mch));
			printf("Character count: %d\n", tk->mcht.char_count);
			break;

		case TOKEN_EOF:
			printf("Token type: END OF FILE.\n");
			break;
		
		default:
			break;

	}
	printf("\n");
}

/*-------------------------------------------------------------*/
void html_print_force_quirks_flag(force_quirks_flag_type flag)
{
	if(flag == ON)
	{
		printf("Force quirks flag: on\n");
	}
	else if(flag == OFF)
	{
		printf("Force quirks flag: off\n");
	}
	else
	{
		printf("Invalid force quirks flag value.\n");
	}
}
/*-------------------------------------------------------------*/
void html_print_attribute_list(attribute_list *attributes)
{
	printf("Attributes: [");

    while (attributes != NULL)
    {
		printf("%s=%s", attributes->name, attributes->value);

		switch (attributes->attr_ns)
		{	
			case HTML:
				printf("(ns-html)");
				break;
			case MATHML:
				printf("(ns-mathml)");
				break;
			case SVG:
				printf("(ns-svg)");
				break;
			case XLINK:
				printf("(ns-xlink)");
				break;
			case XML:
				printf("(ns-xml)");
				break;
			case XMLNS:
				printf("(ns-xmlns)");
				break;
			case DEFAULT:
				printf("(ns-default)");
				break;
			default:
				break;
		}

		if (attributes->tail != NULL)
		{
			printf(", ");
		}
		attributes = attributes->tail;
    }

    putchar(']');
	printf("\n");

}
/*-------------------------------------------------------------*/
void html_print_self_closing_flag(self_closing_flag_type flag)
{
	if(flag == SET)
	{
		printf("Self closing flag: set\n");
	}
	else if(flag == UNSET)
	{
		printf("Self closing flag: unset\n");
	}
	else
	{
		printf("Invalid self closing flag value.\n");
	}

}
/*-------------------------------------------------------------*/
void html_print_token_list(token_list *tk_list)
{
	while (tk_list != NULL)
    {
		html_print_token(tk_list->tk);
		putchar('\n');
		tk_list = tk_list->tail;
    }
}
