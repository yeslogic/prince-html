#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "parser.h"
#include "util.h"
#include "format.h"




typedef enum {
	DATA_STATE,
	CHARACTER_REFERENCE_IN_DATA_STATE,
	RCDATA_STATE,
	CHARACTER_REFERENCE_IN_RCDATA_STATE,
	RAWTEXT_STATE,
	SCRIPT_DATA_STATE,
	PLAINTEXT_STATE,
	TAG_OPEN_STATE,
	END_TAG_OPEN_STATE,
	TAG_NAME_STATE,
	RCDATA_LESS_THAN_SIGN_STATE,
	RCDATA_END_TAG_OPEN_STATE,
	RCDATA_END_TAG_NAME_STATE,
	RAWTEXT_LESS_THAN_SIGN_STATE,
	RAWTEXT_END_TAG_OPEN_STATE,
	RAWTEXT_END_TAG_NAME_STATE,
	SCRIPT_DATA_LESS_THAN_SIGN_STATE,
	SCRIPT_DATA_END_TAG_OPEN_STATE,
	SCRIPT_DATA_END_TAG_NAME_STATE,
	SCRIPT_DATA_ESCAPE_START_STATE,
	SCRIPT_DATA_ESCAPE_START_DASH_STATE,
	SCRIPT_DATA_ESCAPED_STATE,
	SCRIPT_DATA_ESCAPED_DASH_STATE,
	SCRIPT_DATA_ESCAPED_DASH_DASH_STATE,
	SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE,
	SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE,
	SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE,
	SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE,
	SCRIPT_DATA_DOUBLE_ESCAPED_STATE,
	SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE,
	SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE,
	SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE,
	SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE,
	BEFORE_ATTRIBUTE_NAME_STATE,
	ATTRIBUTE_NAME_STATE,
	AFTER_ATTRIBUTE_NAME_STATE,
	BEFORE_ATTRIBUTE_VALUE_STATE,
	ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE,
	ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE,
	ATTRIBUTE_VALUE_UNQUOTED_STATE,
	CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE_STATE,
	AFTER_ATTRIBUTE_VALUE_QUOTED_STATE,
	SELF_CLOSING_START_TAG_STATE,
	BOGUS_COMMENT_STATE,
	MARKUP_DECLARATION_OPEN_STATE,
	COMMENT_START_STATE,
	COMMENT_START_DASH_STATE,
	COMMENT_STATE,
	COMMENT_END_DASH_STATE,
	COMMENT_END_STATE,
	COMMENT_END_BANG_STATE,
	DOCTYPE_STATE,
	BEFORE_DOCTYPE_NAME_STATE,
	DOCTYPE_NAME_STATE,
	AFTER_DOCTYPE_NAME_STATE,
	AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE,
	BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE,
	DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE,
	DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE,
	AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE,
	BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE,
	AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE,
	BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE,
	DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE,
	DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE,
	AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE,
	BOGUS_DOCTYPE_STATE,
	CDATA_SECTION_STATE,
	NUM_TOKENIZATION_STATES
} tokenization_state;



typedef enum {
	RECONSUME,
	NOT_RECONSUME
} character_consumption_type;


typedef enum {
	REPROCESS,
	NOT_REPROCESS
} token_process_type;


typedef enum {
	INITIAL,
	BEFORE_HTML,
	BEFORE_HEAD,
	IN_HEAD,
	IN_HEAD_NOSCRIPT,
	AFTER_HEAD,
	IN_BODY,
	TEXT,
	IN_TABLE,
	IN_TABLE_TEXT,
	IN_CAPTION,
	IN_COLUMN_GROUP,
	IN_TABLE_BODY,
	IN_ROW,
	IN_CELL,
	IN_SELECT,
	IN_SELECT_IN_TABLE,
	AFTER_BODY,
	IN_FRAMESET,
	AFTER_FRAMESET,
	AFTER_AFTER_BODY,
	AFTER_AFTER_FRAMESET,
	NUM_INSERTION_MODES
} insertion_mode;


/*----------------TOKENISER STATE TRANSITION FUNCTIONS--------------------*/
/*------------------------------------------------------------------------*/
token_list *data_state_s1(unsigned char *ch);
token_list *character_reference_in_data_state_s2(unsigned char *ch);
token_list *rcdata_state_s3(unsigned char *ch);
token_list *character_reference_in_rcdata_state_s4(unsigned char *ch);
token_list *rawtext_state_s5(unsigned char *ch);
token_list *script_data_state_s6(unsigned char *ch);
token_list *plaintext_state_s7(unsigned char *ch);
token_list *tag_open_state_s8(unsigned char *ch);
token_list *end_tag_open_state_s9(unsigned char *ch);
token_list *tag_name_state_s10(unsigned char *ch);
token_list *rcdata_less_than_sign_state_s11(unsigned char *ch);
token_list *rcdata_end_tag_open_state_s12(unsigned char *ch);
token_list *rcdata_end_tag_name_state_s13(unsigned char *ch);
token_list *rawtext_less_than_sign_state_s14(unsigned char *ch);
token_list *rawtext_end_tag_open_state_s15(unsigned char *ch);
token_list *rawtext_end_tag_name_state_s16(unsigned char *ch);
token_list *script_data_less_than_sign_state_s17(unsigned char *ch);
token_list *script_data_end_tag_open_state_s18(unsigned char *ch);
token_list *script_data_end_tag_name_state_s19(unsigned char *ch);
token_list *script_data_escape_start_state_s20(unsigned char *ch);
token_list *script_data_escape_start_dash_state_s21(unsigned char *ch);
token_list *script_data_escaped_state_s22(unsigned char *ch);
token_list *script_data_escaped_dash_state_s23(unsigned char *ch);
token_list *script_data_escaped_dash_dash_state_s24(unsigned char *ch);
token_list *script_data_escaped_less_than_sign_state_s25(unsigned char *ch);
token_list *script_data_escaped_end_tag_open_state_s26(unsigned char *ch);
token_list *script_data_escaped_end_tag_name_state_s27(unsigned char *ch);
token_list *script_data_double_escape_start_state_s28(unsigned char *ch);
token_list *script_data_double_escaped_state_s29(unsigned char *ch);
token_list *script_data_double_escaped_dash_state_s30(unsigned char *ch);
token_list *script_data_double_escaped_dash_dash_state_s31(unsigned char *ch);
token_list *script_data_double_escaped_less_than_sign_state_s32(unsigned char *ch);
token_list *script_data_double_escape_end_state_s33(unsigned char *ch);
token_list *before_attribute_name_state_s34(unsigned char *ch);
token_list *attribute_name_state_s35(unsigned char *ch);
token_list *after_attribute_name_state_s36(unsigned char *ch);
token_list *before_attribute_value_state_s37(unsigned char *ch);
token_list *attribute_value_double_quoted_state_s38(unsigned char *ch);
token_list *attribute_value_single_quoted_state_s39(unsigned char *ch);
token_list *attribute_value_unquoted_state_s40(unsigned char *ch);
token_list *ch_reference_in_attribute_value_state_s41(unsigned char *ch);
token_list *after_attribute_value_quoted_state_s42(unsigned char *ch);
token_list *self_closing_start_tag_state_s43(unsigned char *ch);
token_list *bogus_comment_state_s44(unsigned char *ch);
token_list *markup_declaration_open_state_s45(unsigned char *ch);
token_list *comment_start_state_s46(unsigned char *ch);
token_list *comment_start_dash_state_s47(unsigned char *ch);
token_list *comment_state_s48(unsigned char *ch);
token_list *comment_end_dash_state_s49(unsigned char *ch);
token_list *comment_end_state_s50(unsigned char *ch);
token_list *comment_end_bang_state_s51(unsigned char *ch);
token_list *doctype_state_s52(unsigned char *ch);
token_list *before_doctype_name_state_s53(unsigned char *ch);
token_list *doctype_name_state_s54(unsigned char *ch);
token_list *after_doctype_name_state_s55(unsigned char *ch);
token_list *after_doctype_public_keyword_state_s56(unsigned char *ch);
token_list *before_doctype_public_identifier_state_s57(unsigned char *ch);
token_list *doctype_public_identifier_double_quoted_state_s58(unsigned char *ch);
token_list *doctype_public_identifier_single_quoted_state_s59(unsigned char *ch);
token_list *after_doctype_public_identifier_state_s60(unsigned char *ch);
token_list *between_doctype_public_and_system_identifiers_state_s61(unsigned char *ch);
token_list *after_doctype_system_keyword_state_s62(unsigned char *ch);
token_list *before_doctype_system_identifier_state_s63(unsigned char *ch);
token_list *doctype_system_identifier_double_quoted_state_s64(unsigned char *ch);
token_list *doctype_system_identifier_single_quoted_state_s65(unsigned char *ch);
token_list *after_doctype_system_identifier_state_s66(unsigned char *ch);
token_list *bogus_doctype_state_s67(unsigned char *ch);
token_list *cdata_section_state_s68(unsigned char *ch);
/*------------------------------------------------------------------------*/


/*---------------------TREE CONSTRUCTION FUNCTIONS------------------------*/
/*------------------------------------------------------------------------*/
void initial_mode(token *tk);
void before_html_mode(token *tk);
void before_head_mode(token *tk);
void in_head_mode(token *tk);
void in_head_noscript_mode(token *tk);
void after_head_mode(token *tk);
void in_body_mode(token *tk);
void text_mode(token *tk);
void in_table_mode(token *tk);
void in_table_text_mode(token *tk);
void in_caption_mode(token *tk);
void in_column_group_mode(token *tk);
void in_table_body_mode(token *tk);
void in_row_mode(token *tk);
void in_cell_mode(token *tk);
void in_select_mode(token *tk);
void in_select_in_table_mode(token *tk);
void after_body_mode(token *tk);
void in_frameset_mode(token *tk);
void after_frameset_mode(token *tk);
void after_after_body_mode(token *tk);
void after_after_frameset_mode(token *tk);

/*------------------------------------------------------------------------*/

/*the caller of this function must instantiate the global variable: doc_root
  in the full document case: "starting_node" should be doc_root
							 "starting_state" should be "DATA_STATE".
							 "starting_mode" should be "INITIAL"
  in the fragment case: "starting_node" should be "html" node.
						"staring_state" and "starting_mode" should be determined by the "context" element.
*/
void html_parse_memory_1(unsigned char *file_buffer, long buffer_length, node *starting_node,
						 tokenization_state starting_state, insertion_mode starting_mode);


insertion_mode reset_insertion_mode(node *root, node *current_node);
insertion_mode reset_insertion_mode_fragment(unsigned char *context_element_name);


/*--------------------TOKENISER STATE MACHINE-----------------------------*/
/*------------------------------------------------------------------------*/
token_list *(*const tokeniser_state_functions [NUM_TOKENIZATION_STATES]) (unsigned char *ch) = {
	data_state_s1,
	character_reference_in_data_state_s2,
	rcdata_state_s3,
	character_reference_in_rcdata_state_s4,
	rawtext_state_s5,
	script_data_state_s6,
	plaintext_state_s7,
	tag_open_state_s8,
	end_tag_open_state_s9,
	tag_name_state_s10,
	rcdata_less_than_sign_state_s11,
	rcdata_end_tag_open_state_s12,
	rcdata_end_tag_name_state_s13,
	rawtext_less_than_sign_state_s14,
	rawtext_end_tag_open_state_s15,
	rawtext_end_tag_name_state_s16,
	script_data_less_than_sign_state_s17,
	script_data_end_tag_open_state_s18,
	script_data_end_tag_name_state_s19,
	script_data_escape_start_state_s20,
	script_data_escape_start_dash_state_s21,
	script_data_escaped_state_s22,
	script_data_escaped_dash_state_s23,
	script_data_escaped_dash_dash_state_s24,
	script_data_escaped_less_than_sign_state_s25,
	script_data_escaped_end_tag_open_state_s26,
	script_data_escaped_end_tag_name_state_s27,
	script_data_double_escape_start_state_s28,
	script_data_double_escaped_state_s29,
	script_data_double_escaped_dash_state_s30,
	script_data_double_escaped_dash_dash_state_s31,
	script_data_double_escaped_less_than_sign_state_s32,
	script_data_double_escape_end_state_s33,
	before_attribute_name_state_s34,
	attribute_name_state_s35,
	after_attribute_name_state_s36,
	before_attribute_value_state_s37,
	attribute_value_double_quoted_state_s38,
	attribute_value_single_quoted_state_s39,
	attribute_value_unquoted_state_s40,
	ch_reference_in_attribute_value_state_s41,
	after_attribute_value_quoted_state_s42,
	self_closing_start_tag_state_s43,
	bogus_comment_state_s44,
	markup_declaration_open_state_s45,
	comment_start_state_s46,
	comment_start_dash_state_s47,
	comment_state_s48,
	comment_end_dash_state_s49,
	comment_end_state_s50,
	comment_end_bang_state_s51,
	doctype_state_s52,
	before_doctype_name_state_s53,
	doctype_name_state_s54,
	after_doctype_name_state_s55,
	after_doctype_public_keyword_state_s56,
	before_doctype_public_identifier_state_s57,
	doctype_public_identifier_double_quoted_state_s58,
	doctype_public_identifier_single_quoted_state_s59,
	after_doctype_public_identifier_state_s60,
	between_doctype_public_and_system_identifiers_state_s61,
	after_doctype_system_keyword_state_s62,
	before_doctype_system_identifier_state_s63,
	doctype_system_identifier_double_quoted_state_s64,
	doctype_system_identifier_single_quoted_state_s65,
	after_doctype_system_identifier_state_s66,
	bogus_doctype_state_s67,
	cdata_section_state_s68
};


/*-------------------TREE CONSTRUCTION STATE MACHINE----------------------*/
/*------------------------------------------------------------------------*/
void (*const tree_cons_state_functions [NUM_INSERTION_MODES]) (token *tk) = {
	initial_mode,
	before_html_mode,
	before_head_mode,
	in_head_mode,
	in_head_noscript_mode,
	after_head_mode,
	in_body_mode,
	text_mode,
	in_table_mode,
	in_table_text_mode,
	in_caption_mode,
	in_column_group_mode,
	in_table_body_mode,
	in_row_mode,
	in_cell_mode,
	in_select_mode,
	in_select_in_table_mode,
	after_body_mode,
	in_frameset_mode,
	after_frameset_mode,
	after_after_body_mode,
	after_after_frameset_mode
};


/*----------------------------global variables--------------------------------*/

tokenization_state current_state;
insertion_mode original_insertion_mode;
insertion_mode current_mode;
node *current_node;
element_node *form_element_ptr = NULL, *head_element_ptr = NULL;
active_formatting_list *active_formatting_elements = NULL;

character_consumption_type character_consumption = NOT_RECONSUME;
token_process_type token_process = NOT_REPROCESS;

token *curr_token = NULL;		//instantiate a token object and free that object after it is processed;
attribute_list *curr_token_attr_list = NULL;
unsigned char *curr_attr_name = NULL;
unsigned char *curr_attr_value = NULL;
unsigned char *temp_buf;
unsigned char *last_start_tag_name = NULL;
unsigned char *pending_table_characters = NULL;	
int apply_foster_parenting = 0;	//(1 - apply foster parenting, 0 - not apply foster parenting)
int previous_attribute_value_state_bookmark = 0;
//(1 - ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE, 2 - ATTIBUTE_VALUE_SINGLE_QUOTED_STATE, 3 - ATTRIBUTE_VALUE_UNQUOTED_STATE)

long curr_buffer_index;
long buffer_len;

int character_skip = 0;

element_node *doc_root;
token *doc_type;


/*----------------------------------------------------------------------------*/
/*return 1 if successful, otherwise return 0*/
int html_parse_file(unsigned char *file_name, node **root_ptr, token **doctype_ptr)
{
	unsigned char *file_buffer;
	long buffer_length;

	if((file_buffer = read_file(file_name, &buffer_length)) == NULL)
	{
		printf("Error in read_file()\n");
		return 0;
	}

	html_parse_memory(file_buffer, buffer_length, root_ptr, doctype_ptr);
	//html_parse_memory_fragment(file_buffer, buffer_length, "div", root_ptr);

	return 1;
}


/*----------------------------------------------------------------------------*/
void html_parse_memory_fragment(unsigned char *string_buffer, long string_length, unsigned char *context, node **root_ptr)
{	
	element_node *html_node;
	tokenization_state start_state;
	insertion_mode start_mode;

	assert(string_buffer != NULL);
	assert(context != NULL);

	doc_root = create_element_node("DOC", NULL, HTML);
	html_node = create_element_node("html", NULL, HTML);
	add_child_node((node *)doc_root, (node *)html_node);

	form_element_ptr = NULL;

	if((strcmp(context, "title") == 0) ||
	   (strcmp(context, "textarea") == 0))
	{
		start_state = RCDATA_STATE;
	}
	else if((strcmp(context, "style") == 0) ||
			(strcmp(context, "xmp") == 0) ||
			(strcmp(context, "iframe") == 0) ||
			(strcmp(context, "noembed") == 0) ||
			(strcmp(context, "noframes") == 0))
	{
		start_state = RAWTEXT_STATE;
	}
	else if(strcmp(context, "script") == 0)
	{
		start_state = SCRIPT_DATA_STATE;
	}
	else if(strcmp(context, "noscript") == 0)
	{
		start_state = RAWTEXT_STATE;
	}
	else if(strcmp(context, "plaintext") == 0)
	{
		start_state = PLAINTEXT_STATE;
	}
	else
	{
		start_state = DATA_STATE;
	}

	start_mode = reset_insertion_mode_fragment(context);

	html_parse_memory_1(string_buffer, string_length, (node *)html_node, start_state, start_mode);

	*root_ptr = (node *)doc_root;
	
}



/*----------------------------------------------------------------------------*/
void html_parse_memory(unsigned char *file_buffer, long buffer_length, node **root_ptr, token **doctype_ptr)
{
	//create a document root element
	doc_root = create_element_node("DOC", NULL, HTML);

	html_parse_memory_1(file_buffer, buffer_length, (node *)doc_root, DATA_STATE, INITIAL);
	
	*root_ptr = (node *)doc_root;
	*doctype_ptr = doc_type;
}




/*----------------------------------------------------------------------------*/
/*the caller of this function must instantiate the global variable: doc_root
  in the full document case: "starting_node" should be doc_root
							 "starting_state" should be "DATA_STATE".
							 "starting_mode" should be "INITIAL"
  in the fragment case: "starting_node" should be "html" node.
						"staring_state" and "starting_mode" should be determined by the "context" element.
*/
void html_parse_memory_1(unsigned char *file_buffer, long buffer_length, node *starting_node,
						tokenization_state starting_state, insertion_mode starting_mode)
{
	unsigned char *curr_char;
	long curr_index;
	token_list *tk_list, *temp_ptr;

	assert(file_buffer != NULL);
	assert(starting_node != NULL);

	//create a DOCTYPE token object
	doc_type = malloc(sizeof(token));

	current_node = starting_node;
	current_state = starting_state;
	current_mode = starting_mode;

	buffer_len = buffer_length;
	curr_buffer_index = 0;

	curr_index = 0;
	while(curr_index < buffer_length)
	{
		curr_char = &file_buffer[curr_index];
		
		assert((current_state >= 0) && (current_state < NUM_TOKENIZATION_STATES));
		
		tk_list = tokeniser_state_functions[current_state](curr_char);
		tk_list = html_token_list_reverse(tk_list);	

		assert((current_mode >= 0) && (current_mode < NUM_INSERTION_MODES));
				
		while(tk_list != NULL)
		{
			do
			{
				token_process = NOT_REPROCESS;
				tree_cons_state_functions[current_mode](tk_list->tk);

			}while(token_process == REPROCESS);

			free_token(tk_list->tk);
			temp_ptr = tk_list;
			tk_list = tk_list->tail;

			free(temp_ptr);
		}

	
		if(character_consumption == RECONSUME)
		{
			assert((current_state >= 0) && (current_state < NUM_TOKENIZATION_STATES));
			
			tk_list = tokeniser_state_functions[current_state](curr_char);
			tk_list = html_token_list_reverse(tk_list);
				
			assert((current_mode >= 0) && (current_mode < NUM_INSERTION_MODES));
			while(tk_list != NULL)
			{
				do
				{
					token_process = NOT_REPROCESS;
					tree_cons_state_functions[current_mode](tk_list->tk);

				}while(token_process == REPROCESS);

				free_token(tk_list->tk);
				temp_ptr = tk_list;
				tk_list = tk_list->tail;

				free(temp_ptr);
			}

			character_consumption = NOT_RECONSUME; 
		}

		
		curr_index += (1 + character_skip);
		character_skip = 0;

		curr_buffer_index = curr_index;
	}
	
}




/*----------------------------------------------------------------------------*/
token_list *data_state_s1(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == AMPERSAND)
	{
		current_state = CHARACTER_REFERENCE_IN_DATA_STATE;
	}
	else if( c == LESS_THAN_SIGN)
	{
		current_state = TAG_OPEN_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		curr_token = create_eof_token();
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token = create_character_token(c);
		return html_token_list_cons(curr_token, NULL);
	}
	
	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *character_reference_in_data_state_s2(unsigned char *ch)
{
	int chars_consumed;
	int is_attribute = 0;
	int has_additional_allowed_char = 0;	//no additional allowed char
	unsigned char additional_allowed_char = '\0';	//this will be ignored
	unsigned char *char_ref;
	token_list *tk_list;

	current_state = DATA_STATE;

	char_ref = html_character_reference(ch,is_attribute, 
				has_additional_allowed_char, additional_allowed_char, &chars_consumed, curr_buffer_index, buffer_len);

	if(char_ref == NULL)
	{
		character_consumption = RECONSUME;

		curr_token = create_character_token(AMPERSAND);
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{	
		int i, len = strlen(char_ref);
		
		tk_list = NULL;
		for(i = 0; i < len; i++)
		{
			tk_list = html_token_list_cons(create_character_token(char_ref[i]), tk_list);
		}

		free(char_ref);
		character_skip = chars_consumed - 1;

		return tk_list;
	}
}

/*----------------------------------------------------------------------------*/
token_list *rcdata_state_s3(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == AMPERSAND)
	{
		current_state = CHARACTER_REFERENCE_IN_RCDATA_STATE;
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = RCDATA_LESS_THAN_SIGN_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		curr_token = create_eof_token();
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token = create_character_token(c);
		return html_token_list_cons(curr_token, NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *character_reference_in_rcdata_state_s4(unsigned char *ch)
{
	int chars_consumed;
	int is_attribute = 0;
	int has_additional_allowed_char = 0;	//no additional allowed char
	unsigned char additional_allowed_char = '\0';	//this will be ignored
	unsigned char *char_ref;
	token_list *tk_list;

	current_state = RCDATA_STATE;
	char_ref = html_character_reference(ch,is_attribute, 
				has_additional_allowed_char, additional_allowed_char, &chars_consumed, curr_buffer_index, buffer_len);

	if(char_ref == NULL)
	{
		character_consumption = RECONSUME;

		curr_token = create_character_token(AMPERSAND);
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{	
		int i, len = strlen(char_ref);
		
		tk_list = NULL;
		for(i = 0; i < len; i++)
		{
			tk_list = html_token_list_cons(create_character_token(char_ref[i]), tk_list);
		}

		free(char_ref);
		character_skip = chars_consumed - 1;

		return tk_list;
	}
}


/*----------------------------------------------------------------------------*/
token_list *rawtext_state_s5(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == LESS_THAN_SIGN)
	{
		current_state = RAWTEXT_LESS_THAN_SIGN_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		curr_token = create_eof_token();
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		return html_token_list_cons(create_character_token(c), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_state_s6(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_LESS_THAN_SIGN_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		curr_token = create_eof_token();
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token = create_character_token(c);
		return html_token_list_cons(curr_token, NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *plaintext_state_s7(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == NULL_CHARACTER)
	{
		//parse error
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		curr_token = create_eof_token();
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token = create_character_token(c);
		return html_token_list_cons(curr_token, NULL);
	}
	
}


/*----------------------------------------------------------------------------*/
token_list *tag_open_state_s8(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == EXCLAMATION_MARK)
	{
		current_state = MARKUP_DECLARATION_OPEN_STATE;
	}
	else if(c == SOLIDUS)
	{
		current_state = END_TAG_OPEN_STATE;
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		curr_token = create_start_tag_token(c);
			
		current_state = TAG_NAME_STATE;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token = create_start_tag_token(c);

		current_state = TAG_NAME_STATE;
	}
	else if(c == QUESTION_MARK)
	{
		//parse error
		current_state = BOGUS_COMMENT_STATE;
	}
	else
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);		
	}


	return NULL;

}


/*----------------------------------------------------------------------------*/
token_list *end_tag_open_state_s9(unsigned char *ch)
{
	token_list *tk_list;
	unsigned char c = *ch;

	if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		curr_token = create_end_tag_token(c);

		current_state = TAG_NAME_STATE;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token = create_end_tag_token(c);
	
		current_state = TAG_NAME_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		current_state = DATA_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);

		character_consumption = RECONSUME;
		current_state = DATA_STATE;

		return tk_list;
	}
	else
	{
		//parse error
		current_state = BOGUS_COMMENT_STATE;
	}

	return NULL;
}

/*----------------------------------------------------------------------------*/
token_list *tag_name_state_s10(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;
		current_state = BEFORE_ATTRIBUTE_NAME_STATE;
	}
	else if(c == SOLIDUS)
	{
		current_state = SELF_CLOSING_START_TAG_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate
			
			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);

		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_token->stt.tag_name = string_append(curr_token->stt.tag_name, c);
		}
		else if(curr_token->type == TOKEN_END_TAG)
		{
			curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		}
		else
		{
			;
		}
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		//Append a U+FFFD REPLACEMENT CHARACTER 
		//character to the current tag token's tag name.
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			if(curr_token->type == TOKEN_START_TAG)
			{
				curr_token->stt.tag_name = string_append(curr_token->stt.tag_name, byte_seq[i]);
			}
			else if(curr_token->type == TOKEN_END_TAG)
			{
				curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, byte_seq[i]);
			}
			else
			{
				;
			}
		}
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		character_consumption = RECONSUME;
		current_state = DATA_STATE;
	}
	else
	{
		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_token->stt.tag_name = string_append(curr_token->stt.tag_name, c);
		}
		else if(curr_token->type == TOKEN_END_TAG)
		{
			curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		}
		else
		{
			;
		}
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *rcdata_less_than_sign_state_s11(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if (c == SOLIDUS)
	{
		free(temp_buf);
		temp_buf = NULL;

		temp_buf = string_append(NULL, '\0');
		current_state = RCDATA_END_TAG_OPEN_STATE;
	}
	else
	{
		current_state = RCDATA_STATE;
		character_consumption = RECONSUME;

		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL); 
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *rcdata_end_tag_open_state_s12(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = RCDATA_END_TAG_NAME_STATE;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = RCDATA_END_TAG_NAME_STATE;
	}
	else
	{
		current_state = RCDATA_STATE;
		character_consumption = RECONSUME;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);

		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *rcdata_end_tag_name_state_s13(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			;	//in the spec: "switch to the before attribute name state"
				//but end tags should not have attributes,
				//this has not been implemented
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = RCDATA_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		}
	}
	else if(c == SOLIDUS)
	{
		//if(strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0)
		//{
		//	;	//in the spec: "switch to the self-closing start tag state"
				//but end tags should not be self-closing,
				//this has not been implemented
		//}
		//else
		//{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}


			current_state = RCDATA_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		//}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;
			return html_token_list_cons(curr_token, NULL);
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = RCDATA_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		}
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else
	{
		int i, len;
			
		if(temp_buf != NULL)
		{
			len = strlen(temp_buf);
		}
		else
		{
			len = 0;
		}




		current_state = RCDATA_STATE;
		character_consumption = RECONSUME;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
		for(i = 0; i < len; i++)
		{
			tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
		}

		free(temp_buf);
		temp_buf = NULL;
		
		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *rawtext_less_than_sign_state_s14(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == SOLIDUS)
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');

		current_state = RAWTEXT_END_TAG_OPEN_STATE;
	}
	else
	{
		current_state = RAWTEXT_STATE;
		character_consumption = RECONSUME;

		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
	}
	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *rawtext_end_tag_open_state_s15(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = RAWTEXT_END_TAG_NAME_STATE;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = RAWTEXT_END_TAG_NAME_STATE;
	}
	else
	{
		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
		current_state = RAWTEXT_STATE;
		character_consumption = RECONSUME;

		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *rawtext_end_tag_name_state_s16(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			;	//in the spec: "switch to the before attribute name state"
				//but end tags should not have attributes,
				//this has not been implemented
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			current_state = RAWTEXT_STATE;
			character_consumption = RECONSUME;

			return tk_list;
		}
	}
	else if(c == SOLIDUS)
	{
		//if(strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0)
		//{
		//	;	//in the spec: "switch to the self-closing start tag state"
				//but end tags should not be self-closing,
				//this has not been implemented
		//}
		//else
		//{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			current_state = RAWTEXT_STATE;
			character_consumption = RECONSUME;

			return tk_list;
		//}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;
			return html_token_list_cons(curr_token, NULL);
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			current_state = RAWTEXT_STATE;
			character_consumption = RECONSUME;

			return tk_list;
		}
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else
	{
		int i, len;
			
		if(temp_buf != NULL)
		{
			len = strlen(temp_buf);
		}
		else
		{
			len = 0;
		}

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
		for(i = 0; i < len; i++)
		{
			tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
		}

		free(temp_buf);
		temp_buf = NULL;

		current_state = RAWTEXT_STATE;
		character_consumption = RECONSUME;
		
		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_less_than_sign_state_s17(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	if(c == SOLIDUS)
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		current_state = SCRIPT_DATA_END_TAG_OPEN_STATE;
	}
	else if(c == EXCLAMATION_MARK)
	{
		current_state = SCRIPT_DATA_ESCAPE_START_STATE;
		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(EXCLAMATION_MARK), tk_list);
		return tk_list;
	}
	else
	{
		character_consumption = RECONSUME;
		current_state = SCRIPT_DATA_STATE;
	
		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_end_tag_open_state_s18(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = SCRIPT_DATA_END_TAG_NAME_STATE;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = SCRIPT_DATA_END_TAG_NAME_STATE;
	}
	else
	{
		character_consumption = RECONSUME;
		current_state = SCRIPT_DATA_STATE;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_end_tag_name_state_s19(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			;	//in the spec: "switch to the before attribute name state"
				//but end tags should not have attributes,
				//this has not been implemented
		}
		else
		{			
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = SCRIPT_DATA_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		}
	}
	else if(c == SOLIDUS)
	{
		//if(strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0)
		//{
		//	;	//in the spec: "switch to the self-closing start tag state"
				//but end tags should not be self-closing,
				//this has not been implemented
		//}
		//else
		//{
			int i, len;

			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}


			current_state = SCRIPT_DATA_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		//}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;
			return html_token_list_cons(curr_token, NULL);
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = SCRIPT_DATA_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		}
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else
	{
		int i, len;
			
		if(temp_buf != NULL)
		{
			len = strlen(temp_buf);
		}
		else
		{
			len = 0;
		}

		current_state = SCRIPT_DATA_STATE;
		character_consumption = RECONSUME;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
		for(i = 0; i < len; i++)
		{
			tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
		}

		free(temp_buf);
		temp_buf = NULL;
		
		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escape_start_state_s20(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPE_START_DASH_STATE;

		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else
	{
		current_state = SCRIPT_DATA_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escape_start_dash_state_s21(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPED_DASH_DASH_STATE;
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else
	{
		current_state = SCRIPT_DATA_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escaped_state_s22(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPED_DASH_STATE;
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		return html_token_list_cons(create_character_token(c), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escaped_dash_state_s23(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPED_DASH_DASH_STATE;
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		return html_token_list_cons(create_character_token(c), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escaped_dash_dash_state_s24(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_STATE;
		return html_token_list_cons(create_character_token(GREATER_THAN_SIGN), NULL);
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		return html_token_list_cons(create_character_token(c), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escaped_less_than_sign_state_s25(unsigned char *ch)
{	
	unsigned char c = *ch;
	token_list *tk_list;

	if(c == SOLIDUS)
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		current_state = SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE;
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		temp_buf = string_append(temp_buf, c);

		current_state = SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(*ch), tk_list);
		return tk_list;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		temp_buf = string_append(temp_buf, c);

		current_state = SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(*ch), tk_list);
		return tk_list;
	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		character_consumption = RECONSUME;

		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escaped_end_tag_open_state_s26(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token = create_end_tag_token(c);
		temp_buf = string_append(temp_buf, *ch);
		current_state = SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE;
	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		character_consumption = RECONSUME;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_escaped_end_tag_name_state_s27(unsigned char *ch)
{
	unsigned char c = *ch;
	token_list *tk_list;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			;	//in the spec: "switch to the before attribute name state"
				//but end tags should not have attributes,
				//this has not been implemented
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = SCRIPT_DATA_ESCAPED_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		}
	}
	else if(c == SOLIDUS)
	{
		//if(strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0)
		//{
		//	;	//in the spec: "switch to the self-closing start tag state"
				//but end tags should not be self-closing,
				//this has not been implemented
		//}
		//else
		//{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = SCRIPT_DATA_ESCAPED_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		//{

	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;
			return html_token_list_cons(curr_token, NULL);
		}
		else
		{
			int i, len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			current_state = SCRIPT_DATA_ESCAPED_STATE;
			character_consumption = RECONSUME;

			tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
			tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
			for(i = 0; i < len; i++)
			{
				tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return tk_list;
		}
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		curr_token->ett.tag_name = string_append(curr_token->ett.tag_name, c);
		temp_buf = string_append(temp_buf, *ch);
	}
	else
	{
		int i, len;
			
		if(temp_buf != NULL)
		{
			len = strlen(temp_buf);
		}
		else
		{
			len = 0;
		}

		current_state = SCRIPT_DATA_ESCAPED_STATE;
		character_consumption = RECONSUME;

		tk_list = html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
		tk_list = html_token_list_cons(create_character_token(SOLIDUS), tk_list);
		
		for(i = 0; i < len; i++)
		{
			tk_list = html_token_list_cons(create_character_token(temp_buf[i]), tk_list);
		}

		free(temp_buf);
		temp_buf = NULL;
		
		return tk_list;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_double_escape_start_state_s28(unsigned char *ch)
{
	unsigned char c = *ch;

	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) || (c == SPACE)
			|| (c == SOLIDUS) || (c == GREATER_THAN_SIGN))
	{
		if((temp_buf != NULL) && (strcmp(temp_buf, "script") == 0))
		{
			current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		}
		else
		{
			current_state = SCRIPT_DATA_ESCAPED_STATE;
		}
		
		return html_token_list_cons(create_character_token(c), NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		temp_buf = string_append(temp_buf, c);
		return html_token_list_cons(create_character_token(*ch), NULL);
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		temp_buf = string_append(temp_buf, *ch);
		return html_token_list_cons(create_character_token(*ch), NULL);
	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_double_escaped_state_s29(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE;
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;
		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		return html_token_list_cons(create_character_token(*ch), NULL);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_double_escaped_dash_state_s30(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE;
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;
		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		return html_token_list_cons(create_character_token(*ch), NULL);
	}	

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_double_escaped_dash_dash_state_s31(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == HYPHEN_MINUS)
	{
		return html_token_list_cons(create_character_token(HYPHEN_MINUS), NULL);
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;
		return html_token_list_cons(create_character_token(LESS_THAN_SIGN), NULL);
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_STATE;
		return html_token_list_cons(create_character_token(GREATER_THAN_SIGN), NULL);
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		return replacement_for_null_char();
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		return html_token_list_cons(create_character_token(*ch), NULL);
	}	
	
	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_double_escaped_less_than_sign_state_s32(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == SOLIDUS)
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		current_state = SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE;
		return html_token_list_cons(create_character_token(SOLIDUS), NULL);
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *script_data_double_escape_end_state_s33(unsigned char *ch)
{
	unsigned char c = *ch;

	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) || (c == SPACE)
			|| (c == SOLIDUS) || (c == GREATER_THAN_SIGN))
	{
		if((temp_buf != NULL) && (strcmp(temp_buf, "script") == 0))
		{
			current_state = SCRIPT_DATA_ESCAPED_STATE;
		}
		else
		{
			current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		}
		
		return html_token_list_cons(create_character_token(*ch), NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		temp_buf = string_append(temp_buf, c);
		return html_token_list_cons(create_character_token(*ch), NULL);
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		temp_buf = string_append(temp_buf, *ch);
		return html_token_list_cons(create_character_token(*ch), NULL);
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *before_attribute_name_state_s34(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the  character
	}
	else if(c == SOLIDUS)
	{
		current_state = SELF_CLOSING_START_TAG_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate
			
			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);
		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		//add the attribute only if the current token is a start tag token
		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_attr_name = string_append(NULL, c);
			curr_attr_value = string_append(NULL, '\0');
			current_state = ATTRIBUTE_NAME_STATE;
			//if it ever gets to the ATTRIBUTE_NAME_STATE, the current token
			//must be a start tag token.
		}
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error

		//add the attribute only if the current token is a start tag token
		if(curr_token->type == TOKEN_START_TAG)
		{
			unsigned char byte_seq[5];
			int len, i;

			utf8_byte_sequence(0xFFFD, byte_seq);
			len = strlen(byte_seq);

			curr_attr_name = NULL;
			for(i = 0; i < len; i++)
			{
				curr_attr_name = string_append(curr_attr_name, byte_seq[i]);
			}

			curr_attr_value = string_append(NULL, '\0');
			current_state = ATTRIBUTE_NAME_STATE;
			//if it ever gets to the ATTRIBUTE_NAME_STATE, the current token
			//must be a start tag token.
		}
	}	
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN) || (c == EQUALS_SIGN))
	{
		//parse error

		//add the attribute only if the current token is a start tag token
		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_attr_name = string_append(NULL, c);
			curr_attr_value = string_append(NULL, '\0');
			current_state = ATTRIBUTE_NAME_STATE;
			//if it ever gets to the ATTRIBUTE_NAME_STATE, the current token
			//must be a start tag token.
		}

	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		//add the attribute only if the current token is a start tag token
		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_attr_name = string_append(NULL, c);
			curr_attr_value = string_append(NULL, '\0');
			current_state = ATTRIBUTE_NAME_STATE;
			//if it ever gets to the ATTRIBUTE_NAME_STATE, the current token
			//must be a start tag token.
		}
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *attribute_name_state_s35(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = AFTER_ATTRIBUTE_NAME_STATE;
	}
	else if(c == SOLIDUS)
	{
		current_state = SELF_CLOSING_START_TAG_STATE;
	}
	else if(c == EQUALS_SIGN)
	{
		current_state = BEFORE_ATTRIBUTE_VALUE_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate

			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);
		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		curr_attr_name = string_append(curr_attr_name, c);
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_attr_name = string_append(curr_attr_name, byte_seq[i]);
		}
	}
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN))
	{
		//parse error
		curr_attr_name = string_append(curr_attr_name, c);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		curr_attr_name = string_append(curr_attr_name, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *after_attribute_name_state_s36(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if(c == SOLIDUS)
	{
		current_state = SELF_CLOSING_START_TAG_STATE;
	}
	else if(c == EQUALS_SIGN)
	{
		current_state = BEFORE_ATTRIBUTE_VALUE_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate

			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);
		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		//at this point, the previous attribute(name, value pair) 
		//should be added to the attribute list of the start tag token:
	
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		//start a new attribute:
		curr_attr_name = string_append(NULL, c);
		curr_attr_value = string_append(NULL, '\0');

		current_state = ATTRIBUTE_NAME_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);


		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		//start a new attribute:
		for(i = 0; i < len; i++)
		{
			curr_attr_name = string_append(curr_attr_name, byte_seq[i]);
		}
		curr_attr_value = string_append(NULL, '\0');

		current_state = ATTRIBUTE_NAME_STATE;
	}
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN))
	{
		//parse error

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		//start a new attribute:
		curr_attr_name = string_append(NULL, c);
		curr_attr_value = string_append(NULL, '\0');

		current_state = ATTRIBUTE_NAME_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		//start a new attribute:
		curr_attr_name = string_append(NULL, c);
		curr_attr_value = string_append(NULL, '\0');

		current_state = ATTRIBUTE_NAME_STATE;
	}


	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *before_attribute_value_state_s37(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if(c == QUOTATION_MARK)
	{
		current_state = ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE;
	}
	else if(c == AMPERSAND)
	{
		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
		character_consumption = RECONSUME;
	}
	else if(c == APOSTROPHE)
	{
		current_state = ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_attr_value = string_append(curr_attr_value, byte_seq[i]);
		}

		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		current_state = DATA_STATE;

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate

			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);
		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if((c == LESS_THAN_SIGN) || (c == EQUALS_SIGN) || (c == GRAVE_ACCENT))
	{
		//parse error
		curr_attr_value = string_append(curr_attr_value, c);
		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		curr_attr_value = string_append(curr_attr_value, c);
		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *attribute_value_double_quoted_state_s38(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == QUOTATION_MARK)
	{
		current_state = AFTER_ATTRIBUTE_VALUE_QUOTED_STATE;
	}
	else if(c == AMPERSAND)
	{
		current_state = CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE_STATE;
		previous_attribute_value_state_bookmark = 1;
		//with the additional allowed character being U+0022 QUOTATION MARK (")
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_attr_value = string_append(curr_attr_value, byte_seq[i]);
		}
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		curr_attr_value = string_append(curr_attr_value, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *attribute_value_single_quoted_state_s39(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == APOSTROPHE)
	{
		current_state = AFTER_ATTRIBUTE_VALUE_QUOTED_STATE;
	}
	else if(c == AMPERSAND)
	{
		current_state = CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE_STATE;
		previous_attribute_value_state_bookmark = 2;
		//with the additional allowed characterp being U+0027 APOSTROPHE (').
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_attr_value = string_append(curr_attr_value, byte_seq[i]);
		}
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		curr_attr_value = string_append(curr_attr_value, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *attribute_value_unquoted_state_s40(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		current_state = BEFORE_ATTRIBUTE_NAME_STATE;
	}
	else if(c == AMPERSAND)
	{
		current_state = CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE_STATE;
		previous_attribute_value_state_bookmark = 3;
		//with the additional allowed characterp being U+003E GREATER-THAN SIGN (>).
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate
			
			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);
		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_attr_value = string_append(curr_attr_value, byte_seq[i]);
		}
	}
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN) 
				|| (c == EQUALS_SIGN) || (c == GRAVE_ACCENT))
	{	
		//parse error
		curr_attr_value = string_append(curr_attr_value, c);

	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		curr_attr_value = string_append(curr_attr_value, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *ch_reference_in_attribute_value_state_s41(unsigned char *ch)
{
	int chars_consumed;
	int is_attribute = 1;
	int has_additional_allowed_char = 1;	//with additional allowed char
	unsigned char additional_allowed_char;
	unsigned char *char_ref;

	if(previous_attribute_value_state_bookmark == 1)
	{
		additional_allowed_char = QUOTATION_MARK;
		current_state = ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE;
	}
	else if(previous_attribute_value_state_bookmark == 2)
	{
		additional_allowed_char = APOSTROPHE;
		current_state = ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE;
	}
	else // previous_attribute_value_state_bookmark == 3
	{
		additional_allowed_char = GREATER_THAN_SIGN;
		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}
	previous_attribute_value_state_bookmark = 0;	//reset attribute value state bookmark.

	char_ref = html_character_reference(ch,is_attribute, 
				has_additional_allowed_char, additional_allowed_char, &chars_consumed, curr_buffer_index, buffer_len);

	if(char_ref == NULL)
	{
		character_consumption = RECONSUME;

		curr_attr_value = string_append(curr_attr_value, AMPERSAND);
	}
	else
	{	
		int i, len = strlen(char_ref);
		
		for(i = 0; i < len; i++)
		{
			curr_attr_value = string_append(curr_attr_value, char_ref[i]);
		}

		free(char_ref);
		character_skip = chars_consumed - 1;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *after_attribute_value_quoted_state_s42(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		current_state = BEFORE_ATTRIBUTE_NAME_STATE;	
	}
	else if(c == SOLIDUS)
	{
		current_state = SELF_CLOSING_START_TAG_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		if(curr_token->type == TOKEN_START_TAG)
		{
			//make record of start tag name, for use later,
			//when determining whether an end tag is appropriate
			free(last_start_tag_name);
			last_start_tag_name = NULL;
			last_start_tag_name = strdup(curr_token->stt.tag_name);
		}

		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		//parse error
		current_state = BEFORE_ATTRIBUTE_NAME_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *self_closing_start_tag_state_s43(unsigned char *ch)
{
	unsigned char c = *ch; 

	if(c == GREATER_THAN_SIGN)
	{
		curr_token->stt.self_closing_flag = SET;
		current_state = DATA_STATE;
		
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		return html_token_list_cons(curr_token, NULL);

	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	}
	else
	{
		//parse error
		current_state = BEFORE_ATTRIBUTE_NAME_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *bogus_comment_state_s44(unsigned char *ch)
{
	unsigned char c = *ch;
	unsigned char byte_seq[5];
	long curr_buf_index = curr_buffer_index;
	int len, i, j;


	utf8_byte_sequence(0xFFFD, byte_seq);
	len = strlen(byte_seq);

	curr_token = create_comment_token();

	if(*(ch - 1) == NULL_CHARACTER)		//include the character before the current one in the comment
	{
		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, *(ch - 1));
	}


	//start adding characters in the comment
	j = 0;
	while((curr_buf_index + j) <= (buffer_len - 1))
	{
		if(*(ch + j) == GREATER_THAN_SIGN)
		{
			break;
		}

		if(*(ch + j) == NULL_CHARACTER)		//include the character before the current one in the comment
		{
			for(i = 0; i < len; i++)
			{
				curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
			}
		}
		else
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, *(ch + j));
		}

		j += 1;
	}

	character_skip = j;

	current_state = DATA_STATE;
	return html_token_list_cons(curr_token, NULL);
}


/*----------------------------------------------------------------------------*/
token_list *markup_declaration_open_state_s45(unsigned char *ch)
{
	if(((curr_buffer_index + 1) <= (buffer_len - 1)) && ((*ch == HYPHEN_MINUS) && (*(ch + 1) == HYPHEN_MINUS)))
	{
		character_skip = 1;
		curr_token = create_comment_token();
		current_state = COMMENT_START_STATE;
	}
	else if(((curr_buffer_index + 6) <= (buffer_len - 1)) && 
		  (((*ch == 'D') || (*ch == 'd')) && 
		   ((*(ch + 1) =='O') || (*(ch + 1) =='o')) && 
		   ((*(ch + 2) == 'C') || (*(ch + 2) == 'c')) && 
		   ((*(ch + 3) == 'T') || (*(ch + 3) == 't')) && 
		   ((*(ch + 4) == 'Y') || (*(ch + 4) == 'y')) && 
		   ((*(ch + 5) == 'P') || (*(ch + 5) == 'p')) && 
		   ((*(ch + 6) == 'E') || (*(ch + 6) == 'e'))))
	{
		character_skip = 6;
		current_state = DOCTYPE_STATE;
	}
	/*else if  (Otherwise, if the current node is not an element 
				in the HTML namespace and the next seven characters 
				are an case-sensitive match for the string "[CDATA[" 
				(the five uppercase letters "CDATA" with a U+005B LEFT SQUARE BRACKET character before and after),
				then consume those characters and switch to the CDATA section state.)

	*/
	else if(((curr_buffer_index + 6) <= (buffer_len - 1)) && 
			((*ch == '[') && (*(ch + 1) == 'C') && (*(ch + 2) == 'D') && (*(ch + 3) == 'A') && 
			 (*(ch + 4) == 'T') && (*(ch + 5) == 'A') && (*(ch + 6) == '[')))
	{
		character_skip = 6;
		current_state = CDATA_SECTION_STATE;
	}
	else
	{
		//parse error
		current_state = BOGUS_COMMENT_STATE;	
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *comment_start_state_s46(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_START_DASH_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
		current_state = COMMENT_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
		current_state = COMMENT_STATE;
	}


	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *comment_start_dash_state_s47(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_END_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
		
		current_state = COMMENT_STATE;
	}
	else if( c == GREATER_THAN_SIGN)
	{
		//parse error
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);

		current_state = COMMENT_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *comment_state_s48(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_END_DASH_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *comment_end_dash_state_s49(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_END_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
		
		current_state = COMMENT_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
		
		current_state = COMMENT_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *comment_end_state_s50(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
		
		current_state = COMMENT_STATE;
	}
	else if( c == EXCLAMATION_MARK)
	{
		//parse error
		current_state = COMMENT_END_BANG_STATE;
	}
	else if( c == HYPHEN_MINUS)
	{
		//parse error
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
		
		current_state = COMMENT_STATE;
	}
	
	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *comment_end_bang_state_s51(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, EXCLAMATION_MARK);

		current_state = COMMENT_END_DASH_STATE;
	}
	else if( c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, EXCLAMATION_MARK);
		
		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
		
		current_state = COMMENT_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, EXCLAMATION_MARK);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);

		current_state = COMMENT_STATE;
	}


	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *doctype_state_s52(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = BEFORE_DOCTYPE_NAME_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token = create_doctype_token('\0');
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		current_state = BEFORE_DOCTYPE_NAME_STATE;
		character_consumption = RECONSUME;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *before_doctype_name_state_s53(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;

		curr_token = create_doctype_token(c);
		
		current_state = DOCTYPE_NAME_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		curr_token = create_doctype_token('\0');

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->dt.doctype_name = string_append(curr_token->dt.doctype_name, byte_seq[i]);
		}

		current_state = DOCTYPE_NAME_STATE;
	}
	else if( c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token = create_doctype_token('\0');
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token = create_doctype_token('\0');
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token = create_doctype_token(c);
		current_state = DOCTYPE_NAME_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *doctype_name_state_s54(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = AFTER_DOCTYPE_NAME_STATE;
	}
	else if( c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		curr_token->dt.doctype_name = string_append(curr_token->dt.doctype_name, c);
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->dt.doctype_name = string_append(curr_token->dt.doctype_name, byte_seq[i]);
		}
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->dt.doctype_name = string_append(curr_token->dt.doctype_name, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *after_doctype_name_state_s55(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if( c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		if(((curr_buffer_index + 5) <= (buffer_len - 1)) && 
		  (((*ch == 'P') || (*ch == 'p')) && 
		   ((*(ch + 1) == 'U') || (*(ch + 1) == 'u')) && 
		   ((*(ch + 2) == 'B') || (*(ch + 2) == 'b')) && 
		   ((*(ch + 3) == 'L') || (*(ch + 3) == 'l')) && 
		   ((*(ch + 4) == 'I') || (*(ch + 4) == 'i')) && 
		   ((*(ch + 5) == 'C') || (*(ch + 5) == 'c'))))
		{
			character_skip = 5;
			current_state = AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE;
		}
		else if(((curr_buffer_index + 5) <= (buffer_len - 1)) && 
		  (((*ch == 'S') || (*ch == 's')) && 
		   ((*(ch + 1) == 'Y') || (*(ch + 1) == 'y')) && 
		   ((*(ch + 2) == 'S') || (*(ch + 2) == 's')) && 
		   ((*(ch + 3) == 'T') || (*(ch + 3) == 't')) && 
		   ((*(ch + 4) == 'E') || (*(ch + 4) == 'e')) && 
		   ((*(ch + 5) == 'M') || (*(ch + 5) == 'm'))))
		{
			character_skip = 5;
			current_state = AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE;
		}
		else
		{
			//parse error
			curr_token->dt.force_quirks_flag = ON;
			current_state = BOGUS_DOCTYPE_STATE;
		}
	}

	return NULL;
}

/*----------------------------------------------------------------------------*/
token_list *after_doctype_public_keyword_state_s56(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE;
	}
	else if(c == QUOTATION_MARK)
	{
		//parse error
		curr_token->dt.doctype_public_identifier = string_append(NULL, '\0');
		current_state = DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{
		//parse error
		curr_token->dt.doctype_public_identifier = string_append(NULL, '\0');
		current_state = DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{	
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *before_doctype_public_identifier_state_s57(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if(c == QUOTATION_MARK)
	{
		curr_token->dt.doctype_public_identifier = string_append(NULL, '\0');
		current_state = DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{
		curr_token->dt.doctype_public_identifier = string_append(NULL, '\0');
		current_state = DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE;		
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}
	
	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *doctype_public_identifier_double_quoted_state_s58(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == QUOTATION_MARK)
	{
		current_state = AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->dt.doctype_public_identifier = string_append(curr_token->dt.doctype_public_identifier, byte_seq[i]);
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{	
		curr_token->dt.doctype_public_identifier = string_append(curr_token->dt.doctype_public_identifier, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *doctype_public_identifier_single_quoted_state_s59(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == APOSTROPHE)
	{
		current_state = AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->dt.doctype_public_identifier = string_append(curr_token->dt.doctype_public_identifier, byte_seq[i]);
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{	
		curr_token->dt.doctype_public_identifier = string_append(curr_token->dt.doctype_public_identifier, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *after_doctype_public_identifier_state_s60(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(c == QUOTATION_MARK)
	{	
		//parse error
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{	
		//parse error
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *between_doctype_public_and_system_identifiers_state_s61(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(c == QUOTATION_MARK)
	{		
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{	
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *after_doctype_system_keyword_state_s62(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE;
	}
	else if(c == QUOTATION_MARK)
	{	
		//parse error
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{	
		//parse error
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *before_doctype_system_identifier_state_s63(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if(c == QUOTATION_MARK)
	{
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *doctype_system_identifier_double_quoted_state_s64(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == QUOTATION_MARK)
	{
		current_state = AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, byte_seq[i]);
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *doctype_system_identifier_single_quoted_state_s65(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == APOSTROPHE)
	{
		current_state = AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		unsigned char byte_seq[5];
		int len, i;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, byte_seq[i]);
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, c);
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *after_doctype_system_identifier_state_s66(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		;	//ignore the character
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		//parse error
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		//parse error
		current_state = BOGUS_DOCTYPE_STATE;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *bogus_doctype_state_s67(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(curr_token, NULL);
	}
	else if(curr_buffer_index == buffer_len)	//equivalent to (c == EOF)
	{
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
		
		return html_token_list_cons(curr_token, NULL);
	}
	else
	{
		;	//ignore the character
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
token_list *cdata_section_state_s68(unsigned char *ch)
{
	int i;
	long curr_buf_index = curr_buffer_index;
	token_list *tk_list;

	if((curr_buf_index + 2) > (buffer_len - 1))	//if there are less than 3 chars left (including the current char *ch)
	{
		current_state = DATA_STATE;
		return html_token_list_cons(create_character_token(*ch), NULL);
	}

	tk_list = NULL;
	character_skip = 0;
	i = 0;
	while(!((*(ch + i) == ']') && (*(ch + 1 + i) == ']') && (*(ch + 2 + i) == '>')))
	{
		tk_list =  html_token_list_cons(create_character_token(*(ch + i)), tk_list);
		i += 1;
		character_skip = (i - 1);

		curr_buf_index += 1;
		if((curr_buf_index + 2) > (buffer_len - 1))		//there are less than 3 chars left
		{
			current_state = DATA_STATE;
			return tk_list;
		}
	}

	character_skip += 3;
	current_state = DATA_STATE;
	
	return tk_list;
}

/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
void initial_mode(token *tk)
{
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					; //ignore the token
				}
				else
				{
					//implementation pending
					//set the document to quirks mode

					current_mode = BEFORE_HTML;
					token_process = REPROCESS;	
				}
			}
			break;
		case TOKEN_COMMENT:
			//append comment node to Document object
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//append a DocumentType node to the Document node
			//set the Document to an appropriate mode( quirks or limited-quirks)
			{
				doc_type = create_doctype_token('\0');

				doc_type->dt.doctype_name = strdup(tk->dt.doctype_name);
				if(tk->dt.doctype_public_identifier == NULL)
				{
					doc_type->dt.doctype_public_identifier = NULL;
				}
				else
				{
					doc_type->dt.doctype_public_identifier = strdup(tk->dt.doctype_public_identifier);
				}

				if(tk->dt.doctype_system_identifier == NULL)
				{
					doc_type->dt.doctype_system_identifier = NULL;
				}
				else
				{
					doc_type->dt.doctype_system_identifier = strdup(tk->dt.doctype_system_identifier);
				}

				doc_type->dt.force_quirks_flag = OFF;

				current_mode = BEFORE_HTML;
			}
			break;
		default:
			current_mode = BEFORE_HTML;
			token_process = REPROCESS;	
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void before_html_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					; //ignore the token
				}
				else
				{
					//create an html element, append it to the Document object, 
					element_node *e = create_element_node("html", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = BEFORE_HEAD;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_COMMENT:
			//append comment node to Document object
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
			{
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{	
					//create an html element, append it to the Document object, 
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

				}
				else
				{
					element_node *e = create_element_node("html", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					token_process = REPROCESS;
				}

				current_mode = BEFORE_HEAD;
			}
			break;
		case TOKEN_END_TAG:
			{
				if((strcmp(tk->ett.tag_name, "head") == 0) ||
				   (strcmp(tk->ett.tag_name, "body") == 0) ||
				   (strcmp(tk->ett.tag_name, "html") == 0) ||
				   (strcmp(tk->ett.tag_name, "br") == 0))
				{
					element_node *e = create_element_node("html", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = BEFORE_HEAD;
					token_process = REPROCESS;
				}
				else
				{
					//parse error, ignore the token
					;
				}
			}

			break;		
		default:
			{
				element_node *e = create_element_node("html", NULL, HTML);
				add_child_node(current_node, (node *)e);
				current_node = (node *)e;

				current_mode = BEFORE_HEAD;
				token_process = REPROCESS;
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void before_head_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					; //ignore the token
				}
				else
				{
					element_node *e = create_element_node("head", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_COMMENT:
			//append a comment node to current node
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
			{
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{	
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "head") == 0)
				{
					//Insert an HTML element for the token.
					//Set the head element pointer to the newly created head element.
					
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					head_element_ptr = e;

					current_mode = IN_HEAD;
				}
				else
				{
					element_node *e = create_element_node("head", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					head_element_ptr = e;

					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if((strcmp(tk->ett.tag_name, "head") == 0) ||
				   (strcmp(tk->ett.tag_name, "body") == 0) ||
				   (strcmp(tk->ett.tag_name, "html") == 0) ||
				   (strcmp(tk->ett.tag_name, "br") == 0))
				{
					element_node *e = create_element_node("head", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					head_element_ptr = e;

					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		default:
			{
				element_node *e = create_element_node("head", NULL, HTML);
				add_child_node(current_node, (node *)e);
				current_node = (node *)e;

				head_element_ptr = e;

				current_mode = IN_HEAD;
				token_process = REPROCESS;
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void in_head_mode(token *tk)
{ 

	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					; //insert the character into the current node
				}
				else
				{
					//Act as if an end tag token with the tag name "head" had been seen, 
					//and reprocess the current token.

					current_node = current_node->parent;

					current_mode = AFTER_HEAD;
					token_process = REPROCESS;	

				}
			}

			break;
		case TOKEN_COMMENT:
			//append comment node to current node
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
			{

				if(strcmp(tk->stt.tag_name, "html") == 0)
				{	
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else if((strcmp(tk->stt.tag_name, "base") == 0) ||
						(strcmp(tk->stt.tag_name, "basefont") == 0) ||
						(strcmp(tk->stt.tag_name, "bgsound") == 0) ||
						(strcmp(tk->stt.tag_name, "command") == 0) ||
						(strcmp(tk->stt.tag_name, "link") == 0))
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
		
					//implementation pending: Acknowledge the token's self-closing flag , if it is set
				}
				else if(strcmp(tk->stt.tag_name, "meta") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);

					//implementation pending: Acknowledge the token's self-closing flag , if it is set.
				}
				else if(strcmp(tk->stt.tag_name, "title") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = RCDATA_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;

				}
				else if((strcmp(tk->stt.tag_name, "noframes") == 0) ||
						(strcmp(tk->stt.tag_name, "style") == 0))
				{
					//follow the generic raw text element parsing algorithm on p676

					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "noscript") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_HEAD_NOSCRIPT;
				}
				else if(strcmp(tk->stt.tag_name, "script") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = SCRIPT_DATA_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "head") == 0)
				{
					//parse error
					//ignore the token
					;
				}
				else
				{
					//Act as if an end tag token with the tag name "head" had been seen, 
					//and reprocess the current token
					current_node = current_node->parent;

					current_mode = AFTER_HEAD;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if((strcmp(tk->ett.tag_name, "body") == 0) ||
				   (strcmp(tk->ett.tag_name, "html") == 0) ||
				   (strcmp(tk->ett.tag_name, "br") == 0))
				{
					//Act as if an end tag token with the tag name "head" had been seen, 
					//and reprocess the current token.	
					current_node = current_node->parent;

					current_mode = AFTER_HEAD;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->ett.tag_name, "head") == 0)
				{
					current_node = current_node->parent;
					current_mode = AFTER_HEAD;
				}
				else
				{
					//parse error
					//ignore the token
					;
				}
			}

			break;
		default:
			//Act as if an end tag token with the tag name "head" had been seen, 
			//and reprocess the current token.
			{
				current_node = current_node->parent;
				current_mode = AFTER_HEAD;
				token_process = REPROCESS;	
			}
			break;
	}

}
/*------------------------------------------------------------------------------------*/
void in_head_noscript_mode(token *tk)
{
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					//Process the token using the rules for the "in head " insertion mode
					in_head_mode(tk);
				}
				else
				{
					//parse error
					current_node = current_node->parent;
					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
		    {
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else if((strcmp(tk->stt.tag_name, "basefont") == 0) ||
					    (strcmp(tk->stt.tag_name, "bgsound") == 0) ||
					    (strcmp(tk->stt.tag_name, "link") == 0) ||
					    (strcmp(tk->stt.tag_name, "meta") == 0) ||
					    (strcmp(tk->stt.tag_name, "noframes") == 0) ||
					    (strcmp(tk->stt.tag_name, "style") == 0))
				{
					//Process the token using the rules for the "in head " insertion mode.
					in_head_mode(tk);
				}
				else if((strcmp(tk->stt.tag_name, "head") == 0) ||
						(strcmp(tk->stt.tag_name, "noscript") == 0))
				{
					;//parse error, ignore the token
				}
				else
				{
					//parse error
					current_node = current_node->parent;
					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "noscript") == 0)
				{
					current_node = current_node->parent;
					current_mode = IN_HEAD;
				}
				if(strcmp(tk->ett.tag_name, "br") == 0)
				{
					current_node = current_node->parent;
					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		default:
			{
				//parse error
				current_node = current_node->parent;
				current_mode = IN_HEAD;
				token_process = REPROCESS;
			}
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void after_head_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					; //insert the character into the current node
				}
				else
				{
					//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
					//frameset-ok flag back to "ok", and then reprocess the current token.
					element_node *e = create_element_node("body", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
			{
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{					
					//Process the token using the rules for the "in body " insertion mode .
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "body") == 0)
				{
					//Insert an HTML element for the token.
					//Set the frameset-ok flag to "not ok".
					//Switch the insertion mode to "in body ".
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_BODY;
				}
				else if(strcmp(tk->stt.tag_name, "frameset") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;
					
					current_mode = IN_FRAMESET;
				}
				else if((strcmp(tk->stt.tag_name, "base") == 0) ||
						(strcmp(tk->stt.tag_name, "basefont") == 0) ||
						(strcmp(tk->stt.tag_name, "bgsound") == 0) ||
						(strcmp(tk->stt.tag_name, "link") == 0) ||
						(strcmp(tk->stt.tag_name, "meta") == 0))
				{
					//parse error
					//Push the node pointed to by the head element pointer onto the stack of open elements .
					//Process the token using the rules for the "in head " insertion mode .
					//Remove the node pointed to by the head element pointer from the stack of open elements .
					
					if(head_element_ptr != NULL)	//The head element pointer cannot be null at this point. (checking is for double-insurance)
					{
						current_node = (node *)head_element_ptr;

						in_head_mode(tk);

						current_node = head_element_ptr->parent;
					}
				}
				else if((strcmp(tk->stt.tag_name, "noframes") == 0) ||
						(strcmp(tk->stt.tag_name, "script") == 0) ||
						(strcmp(tk->stt.tag_name, "style") == 0) ||
						(strcmp(tk->stt.tag_name, "title") == 0))
				{
					//parse error
					//Push the node pointed to by the head element pointer onto the stack of open elements .
					//Process the token using the rules for the "in head " insertion mode .
					if(head_element_ptr != NULL)	//The head element pointer cannot be null at this point. (checking is for double-insurance)
					{
						current_node = (node *)head_element_ptr;

						in_head_mode(tk);
					}
				}
				else if(strcmp(tk->stt.tag_name, "head") == 0)
				{
					//parse error
					//ignore the token
				}
				else
				{
					//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
					//frameset-ok flag back to "ok", and then reprocess the current token.

					element_node *e = create_element_node("body", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_BODY;
					token_process = REPROCESS;
				}

			}
			break;
		case TOKEN_END_TAG:
			{
				if((strcmp(tk->ett.tag_name, "body") == 0) ||
				   (strcmp(tk->ett.tag_name, "html") == 0) ||
				   (strcmp(tk->ett.tag_name, "br") == 0))
				{
					//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
					//frameset-ok flag back to "ok", and then reprocess the current token.

					element_node *e = create_element_node("body", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
				else
				{
					//parse error
					//ignore the token
					;
				}
			}

			break;
		default:
			//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
			//frameset-ok flag back to "ok", and then reprocess the current token.
			{
				element_node *e = create_element_node("body", NULL, HTML);
				add_child_node(current_node, (node *)e);
				current_node = (node *)e;
				current_mode = IN_BODY;
				token_process = REPROCESS;
			}
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void in_body_mode(token *tk)
{

	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if(tk->cht.ch == NULL_CHARACTER)
				{
					//parse error
					//ignore the token
					;
				}
				else if((tk->cht.ch == CHARACTER_TABULATION) ||
						(tk->cht.ch == LINE_FEED) ||
						(tk->cht.ch == FORM_FEED) ||
						(tk->cht.ch == CARRIAGE_RETURN) ||
						(tk->cht.ch == SPACE))
				{
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					//insert the character into the current node

					if(current_node->type == TEXT_N)
					{
						text_node *t = (text_node *)current_node;
						t->text_data = string_append(t->text_data, tk->cht.ch);
					}
					else
					{
						text_node *t = create_text_node();
						t->text_data = string_append(t->text_data, tk->cht.ch);
						add_child_node(current_node, (node *)t);
						current_node = (node *)t;
					}
				}
				else
				{
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);
					
					//Insert the token's character into the current node .
					//Set the frameset-ok flag to "not ok".

					if(current_node->type == TEXT_N)
					{
						text_node *t = (text_node *)current_node;
						t->text_data = string_append(t->text_data, tk->cht.ch);
					}
					else
					{
						text_node *t = create_text_node();
						t->text_data = string_append(t->text_data, tk->cht.ch);
						add_child_node(current_node, (node *)t);
						current_node = (node *)t;
					}

				}
			}

			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);

				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}
			
				add_child_node(current_node, (node *)c);

			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			if(current_node->type == TEXT_N)
			{
				current_node = current_node->parent;
			}
			break;
		case TOKEN_START_TAG:
			{	

				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}

				if(strcmp(tk->stt.tag_name, "html") == 0)
				{					
					//parse error
					//For each attribute on the token, check to see if the attribute 
					//is already present on the top element of the stack of open elements . 
					//If it is not, add the attribute and its corresponding value to that element
					element_node *html_node = get_node_by_name((node *)doc_root, current_node, "html");

					if(html_node != NULL)
					{
						attribute_list *temp_ptr = tk->stt.attributes;

						while(temp_ptr != NULL)
						{
							if((temp_ptr->name != NULL) && (temp_ptr->value != NULL))
							{
								//if attribute on token is not already present in html_node->attributes:
								if(attribute_name_in_list(temp_ptr->name, html_node->attributes) == 0)
								{
									html_node->attributes = 
										html_attribute_list_cons(temp_ptr->name, temp_ptr->value, html_node->attributes);
								}
							}

							temp_ptr = temp_ptr->tail;
						}
					}
					
				}
				else if((strcmp(tk->stt.tag_name, "base") == 0) ||
						(strcmp(tk->stt.tag_name, "basefont") == 0) ||
						(strcmp(tk->stt.tag_name, "bgsound") == 0) ||
						(strcmp(tk->stt.tag_name, "command") == 0) ||
						(strcmp(tk->stt.tag_name, "link") == 0) ||
						(strcmp(tk->stt.tag_name, "meta") == 0) ||
						(strcmp(tk->stt.tag_name, "noframes") == 0) ||
						(strcmp(tk->stt.tag_name, "script") == 0) ||
						(strcmp(tk->stt.tag_name, "style") == 0) ||
						(strcmp(tk->stt.tag_name, "title") == 0))
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "body") == 0)
				{
					//parse error
					//set the frameset-ok flag to "not ok"
					//for each attribute on the token, check to see if the attribute is already present
					//on the body element if it is not, add the attribute and its corresponding value to that element.
					element_node *body_node = get_node_by_name((node *)doc_root, current_node, "body");
					attribute_list *curr_token_attrs = tk->stt.attributes;

					if(body_node != NULL)
					{
						while(curr_token_attrs != NULL)
						{
							//if attribute on token is not already present in html_node->attributes:
							if((curr_token_attrs->name != NULL) && (attribute_name_in_list(curr_token_attrs->name, body_node->attributes) == 0))
							{
								body_node->attributes = html_attribute_list_cons(curr_token_attrs->name, 
																				 curr_token_attrs->value, 
																				 body_node->attributes);
							}

							curr_token_attrs = curr_token_attrs->tail;
						}
					}
				}
				else if(strcmp(tk->stt.tag_name, "frameset") == 0)
				{
					//parse error
					//implementation pending
					//Insert an HTML element for the token.
					//Switch the insertion mode to "in frameset"
					;
				}
				else if((strcmp(tk->stt.tag_name, "address") == 0) ||
						(strcmp(tk->stt.tag_name, "article") == 0) ||
						(strcmp(tk->stt.tag_name, "aside") == 0) ||
						(strcmp(tk->stt.tag_name, "blockquote") == 0) ||
						(strcmp(tk->stt.tag_name, "center") == 0) ||
						(strcmp(tk->stt.tag_name, "details") == 0) ||
						(strcmp(tk->stt.tag_name, "dir") == 0) ||
						(strcmp(tk->stt.tag_name, "div") == 0) ||
						(strcmp(tk->stt.tag_name, "dl") == 0) ||
						(strcmp(tk->stt.tag_name, "fieldset") == 0) ||
						(strcmp(tk->stt.tag_name, "figcaption") == 0) ||
						(strcmp(tk->stt.tag_name, "figure") == 0) ||
						(strcmp(tk->stt.tag_name, "footer") == 0) ||
						(strcmp(tk->stt.tag_name, "header") == 0) ||
						(strcmp(tk->stt.tag_name, "hgroup") == 0) ||
						(strcmp(tk->stt.tag_name, "menu") == 0) ||
						(strcmp(tk->stt.tag_name, "nav") == 0) ||
						(strcmp(tk->stt.tag_name, "ol") == 0) ||
						(strcmp(tk->stt.tag_name, "p") == 0) ||
						(strcmp(tk->stt.tag_name, "section") == 0) ||
						(strcmp(tk->stt.tag_name, "summary") == 0) ||
						(strcmp(tk->stt.tag_name, "ul") == 0))
				{

					//If the stack of open elements has a p element in button scope , 
					//then act as if an end tag with the tag name "p" had been seen.
					//Insert an HTML element for the token.
					node *match_node;
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

				}
				else if((strcmp(tk->stt.tag_name, "h1") == 0) ||
						(strcmp(tk->stt.tag_name, "h2") == 0) ||
						(strcmp(tk->stt.tag_name, "h3") == 0) ||
						(strcmp(tk->stt.tag_name, "h4") == 0) ||
						(strcmp(tk->stt.tag_name, "h5") == 0) ||
						(strcmp(tk->stt.tag_name, "h6") == 0))
				{
					//If the stack of open elements has a p element in button scope , 
					//then act as if an end tag with the tag name "p" had been seen.
					//If the current node is an element whose tag name is 
					//one of "h1", "h2", "h3", "h4", "h5", or "h6", 
					//then this is a parse error ; pop the current node off the stack of open elements .
					//Insert an HTML element for the token.
					node *match_node;
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}

					if(current_node->type == ELEMENT_N)
					{
						element_node *curr_e = (element_node *)current_node;
						
						if((strcmp(curr_e->name, "h1") == 0) ||
						   (strcmp(curr_e->name, "h2") == 0) ||
						   (strcmp(curr_e->name, "h3") == 0) ||
						   (strcmp(curr_e->name, "h4") == 0) ||
						   (strcmp(curr_e->name, "h5") == 0) ||
						   (strcmp(curr_e->name, "h6") == 0))
						{
							//parse error;
							current_node = current_node->parent;
						}

					}

					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

				}
				else if((strcmp(tk->stt.tag_name, "pre") == 0) ||
						(strcmp(tk->stt.tag_name, "listing") == 0))
				{
					//If the stack of open elements has a p element in button scope ,
					//then act as if an end tag with the tag name "p" had been seen.
					node *match_node;
					element_node *e;

					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}
										
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//If the next token is a U+000A LINE FEED (LF) character token, 
					//then ignore that token and move on to the next one. 
					//(Newlines at the start of prep168 blocks are ignored as an authoring convenience.)

					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "form") == 0)
				{
					if(form_element_ptr != NULL)
					{
						; //parse error, ignore the token
					}
					else
					{
						//If the stack of open elements has a p element in button scope , then act as if an end tag 
						//with the tag name "p" had been seen.
						
						node *match_node;
						element_node *e;

						if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
						{
							current_node = match_node->parent;
						}
					
						e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
						add_child_node(current_node, (node *)e);
						current_node = (node *)e;

						//set the form element pointer to point to the element created.
						form_element_ptr = e;
					}
				}
				else if(strcmp(tk->stt.tag_name, "li") == 0)
				{
					//Set the frameset-ok flag to "not ok".
					element_node *e;
					node *match_node, *temp_node = current_node;

					if(temp_node->type != ELEMENT_N)
					{
						temp_node = temp_node->parent;
					}

					while(temp_node != (node *)doc_root)
					{
						element_node *temp_element = (element_node *)temp_node;

						if(strcmp(temp_element->name, "li") == 0)
						{
							current_node = temp_node->parent;
							break;
						}
						
						if((is_in_special_category(temp_element) == 1) &&
						   (strcmp(temp_element->name, "address") != 0) &&
						   (strcmp(temp_element->name, "div") != 0) &&
						   (strcmp(temp_element->name, "p") != 0))
						{
							current_node = temp_node;
							break;
						}

						temp_node = temp_node->parent;
					}

					//if the stack of open elements has a p element in button scope , then act as if an end tag 
					//with the tag name "p" had been seen.	
					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;
					
				}
				else if((strcmp(tk->stt.tag_name, "dd") == 0) ||
						(strcmp(tk->stt.tag_name, "dt") == 0))
				{
					//Set the frameset-ok flag to "not ok".
					element_node *e;
					node *match_node, *temp_node = current_node;

					if(temp_node->type != ELEMENT_N)
					{
						temp_node = temp_node->parent;
					}

					while(temp_node != (node *)doc_root)
					{
						element_node *temp_element = (element_node *)temp_node;

						if((strcmp(temp_element->name, "dd") == 0) ||
						   (strcmp(temp_element->name, "dt") == 0))
						{
							current_node = temp_node->parent;
							break;
						}
						
						if((is_in_special_category(temp_element) == 1) &&
						   (strcmp(temp_element->name, "address") != 0) &&
						   (strcmp(temp_element->name, "div") != 0) &&
						   (strcmp(temp_element->name, "p") != 0))
						{
							current_node = temp_node;
							break;
						}

						temp_node = temp_node->parent;
					}

					//if the stack of open elements has a p element in button scope , then act as if an end tag 
					//with the tag name "p" had been seen.	
					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

				}
				else if(strcmp(tk->stt.tag_name, "plaintext") == 0)
				{
					//If the stack of open elements has a p element in button scope ,
					//then act as if an end tag with the tag name "p" had been seen.
					node *match_node;
					element_node *e;

					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = PLAINTEXT_STATE;
				}
				else if(strcmp(tk->stt.tag_name, "button") == 0)
				{
					element_node *e;
					node *match_node;
					if(has_element_in_scope((node *)doc_root, current_node, "button", &match_node) == 1)
					{
						//parse error
						current_node = match_node->parent;
						token_process = REPROCESS;
					}
					else
					{
						//Reconstruct the active formatting elements, if any.
						current_node = reconstruct_active_formatting_elements(active_formatting_elements, 
																			  (node *)doc_root, current_node);

						//Insert an HTML element for the token.
						e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
						add_child_node(current_node, (node *)e);
						current_node = (node *)e;

						//Set the frameset-ok flag to "not ok".
						
					}

				}
				else if(strcmp(tk->stt.tag_name, "a") == 0)
				{
					element_node *element_to_be_removed;
					element_node *e;
					node *temp_node;

					if(get_formatting_element(active_formatting_elements, "a") != NULL)
					{
						//parse error, 
						//act as if an end tag with the tag name "a" had been seen:

						/*----------------------Adoption Agency Algorithm-------------------------------------*/
						adoption_agency_algorithm((node *)doc_root, &active_formatting_elements, &current_node, "a");

						/*------------------------------------------------------------------------------------*/
						//then remove that element from the list of active formatting elements and the stack of open elements if the end tag didn't
						//already remove it (it might not have if the element is not in table scope ).
						element_to_be_removed = get_formatting_element(active_formatting_elements, "a");

						if(element_to_be_removed != NULL)
						{
							active_formatting_elements =
								remove_element_from_active_formatting_list(active_formatting_elements, element_to_be_removed);
						}

						temp_node = current_node;
						while(temp_node != (node *)doc_root)
						{
							element_to_be_removed = (element_node *)temp_node;

							if(strcmp(element_to_be_removed->name, "a") == 0)
							{
								current_node = element_to_be_removed->parent;
							}

							temp_node = temp_node->parent;
						}
	
					}
		
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  
																			(node *)doc_root, current_node);

					//Insert an HTML element for the token. 
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//Push onto the list of active formatting elements that element.
					active_formatting_elements = 
						active_formatting_list_add_element(e, active_formatting_elements);
				}
				else if((strcmp(tk->stt.tag_name, "b") == 0) ||
				        (strcmp(tk->stt.tag_name, "big") == 0) ||
				        (strcmp(tk->stt.tag_name, "code") == 0) ||
						(strcmp(tk->stt.tag_name, "em") == 0) ||
						(strcmp(tk->stt.tag_name, "font") == 0) ||
						(strcmp(tk->stt.tag_name, "i") == 0) ||
						(strcmp(tk->stt.tag_name, "s") == 0) ||
						(strcmp(tk->stt.tag_name, "small") == 0) ||
						(strcmp(tk->stt.tag_name, "strike") == 0) ||
						(strcmp(tk->stt.tag_name, "strong") == 0) ||
						(strcmp(tk->stt.tag_name, "tt") == 0) ||
						(strcmp(tk->stt.tag_name, "u") == 0))
				{
					element_node *e;
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, 
																			(node *)doc_root, current_node);

					//Insert an HTML element for the token. 
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//Push onto the list of active formatting elements that element.
					active_formatting_elements = active_formatting_list_add_element(e, active_formatting_elements);

				}
				else if(strcmp(tk->stt.tag_name, "nobr") == 0)
				{ 
					element_node *e;
					node *match_node;

					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  
																			(node *)doc_root, current_node);
					//If the stack of open elements has a nobr element in scope , 
					//then this is a parse error ; act as if an end tag with the tag name "nobr" had been seen, 
					//then once again reconstruct the active formatting elements , if any.
					if(has_element_in_scope((node *)doc_root, current_node, "nobr", &match_node) == 1)
					{	
						//parse error
						adoption_agency_algorithm((node *)doc_root, &active_formatting_elements, &current_node, "nobr");

						current_node = reconstruct_active_formatting_elements(active_formatting_elements,  
																			(node *)doc_root, current_node);
					}

					//Insert an HTML element for the token. 
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//Push onto the list of active formatting elements that element.
					active_formatting_elements = active_formatting_list_add_element(e, active_formatting_elements);

				}
				else if((strcmp(tk->stt.tag_name, "applet") == 0) ||
						(strcmp(tk->stt.tag_name, "marquee") == 0) ||
						(strcmp(tk->stt.tag_name, "object") == 0))
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  
																			(node *)doc_root, current_node);
					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//Insert a marker at the end of the list of active formatting elements
					active_formatting_elements = active_formatting_list_add_marker(active_formatting_elements);

					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "table") == 0)
				{
					element_node *e;
					node *match_node;
						
					//If the Document is not set to quirks mode , and the stack of open elements has a p element in button scope'
					//then act as if an end tag with the tag name "p" had been seen.
					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;
					
					current_mode = IN_TABLE;

					//Set the frameset-ok flag to "not ok".
				}
				else if((strcmp(tk->stt.tag_name, "area") == 0) ||
						(strcmp(tk->stt.tag_name, "br") == 0) ||
						(strcmp(tk->stt.tag_name, "embed") == 0) ||
						(strcmp(tk->stt.tag_name, "img") == 0) ||
						(strcmp(tk->stt.tag_name, "keygen") == 0) ||
						(strcmp(tk->stt.tag_name, "wbr") == 0))
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					
					//Acknowledge the token's self-closing flag, if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "input") == 0)
				{
					element_node *e;
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					
					//Acknowledge the token's self-closing flag, if it is set.
					//If the token does not have an attribute with the name "type", or if it does, but that attribute's value is not
					//an ASCII case-insensitive match for the string "hidden", then: set the frameset-ok flag to "not ok".
				}
				else if((strcmp(tk->stt.tag_name, "param") == 0) ||
						(strcmp(tk->stt.tag_name, "source") == 0) ||
						(strcmp(tk->stt.tag_name, "track") == 0))
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					//Acknowledge the token's self-closing flag , if it is set.
				}
				else if(strcmp(tk->stt.tag_name, "hr") == 0)
				{
					node *match_node;
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					//If the stack of open elements has a p element in button scope, 
					//then act as if an end tag with the tag name "p" had been seen.
					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					
					add_child_node(current_node, (node *)e);

					//Acknowledge the token's self-closing flag , if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "image") == 0)
				{
					//parse error
					free(tk->stt.tag_name);
					tk->stt.tag_name = strdup("img");

					token_process = REPROCESS;
				}
				else if(strcmp(tk->stt.tag_name, "isindex") == 0)
				{
					//parse error
					if(form_element_ptr != NULL)
					{
						;	//ignore the token
					}
					else
					{
						//Acknowledge the token's self-closing flag , if it is set.
						
						//Act as if a start tag token with the tag name "form" had been seen.
						/*------------------------------------------------------------------*/
						node *match_node;
						element_node *form_e, *hr_e_1, *hr_e_2, *label_e, *input_e;
						text_node *label_text;
						unsigned char *action_attr_val, *prompt_attr_val;

						if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
						{
							current_node = match_node->parent;
						}
					
						form_e = create_element_node("form", NULL, HTML);
						add_child_node(current_node, (node *)form_e);
						current_node = (node *)form_e;

						//set the form element pointer to point to the element created.
						form_element_ptr = form_e;
						/*------------------------------------------------------------------*/

						action_attr_val = get_attribute_value("action", tk->stt.attributes);
						if(action_attr_val != NULL)
						{
							form_e->attributes = html_attribute_list_cons("action", action_attr_val, form_e->attributes);
						}
						
						hr_e_1 = create_element_node("hr", NULL, HTML);
						add_child_node(current_node, (node *)hr_e_1);

						current_node = reconstruct_active_formatting_elements(active_formatting_elements,  
																					(node *)doc_root, current_node);

						label_e = create_element_node("label", NULL, HTML);
						add_child_node(current_node, (node *)label_e);
						current_node = (node *)label_e;

						label_text = create_text_node();
						prompt_attr_val = get_attribute_value("prompt", tk->stt.attributes);
						if(prompt_attr_val != NULL)
						{
							label_text->text_data = prompt_attr_val;
						}
						else
						{
							label_text->text_data = strdup("This is a searchable index. Enter search keywords:");
						}

						add_child_node(current_node, (node *)label_text);

						//back to form element:
						current_node = (node *)form_e;

						input_e = create_element_node("input", NULL, HTML);
						
						input_e->attributes = tk->stt.attributes;
						input_e->attributes = remove_attribute("name", input_e->attributes);
						input_e->attributes = remove_attribute("action", input_e->attributes);
						input_e->attributes = remove_attribute("prompt", input_e->attributes);

						//Set the name attribute of the resulting input element to the value "isindex".
						input_e->attributes = html_attribute_list_cons("name", "isindex", input_e->attributes);
						add_child_node(current_node, (node *)input_e);

						hr_e_2 = create_element_node("hr", NULL, HTML);
						add_child_node(current_node, (node *)hr_e_2);

						//back to the parent of "form"
						current_node = form_e->parent;

						//set the form_element_ptr to NULL.
						form_element_ptr = NULL;
					}
				}
				else if(strcmp(tk->stt.tag_name, "textarea") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//implementation pending: ignore newlines at the start of the textarea
					current_state = RCDATA_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "xmp") == 0)
				{
					node *match_node;
					element_node *e;

					//If the stack of open elementsp has a p element in button scope , 
					//then act as if an end tag with the tag name "p" had been seen.
					if(has_element_in_button_scope((node *)doc_root, current_node, "p", &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					//Set the frameset-ok flag to "not ok".
					//Follow the generic raw text element parsing algorithm .
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "iframe") == 0)
				{
					//Set the frameset-ok flag to "not ok".
					//Follow the generic raw text element parsing algorithm .
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
					
				}
				else if((strcmp(tk->stt.tag_name, "noembed") == 0) ||
						(strcmp(tk->stt.tag_name, "noscript") == 0))	//if the scripting flag is enabled
				{
					//Follow the generic raw text element parsing algorithm .
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "select") == 0)
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					//Set the frameset-ok flag to "not ok".

					if((current_mode == IN_TABLE) ||
					   (current_mode == IN_CAPTION) ||
					   (current_mode == IN_TABLE_BODY) ||
					   (current_mode == IN_ROW) ||
					   (current_mode == IN_CELL))
					{
						current_mode = IN_SELECT_IN_TABLE;
					}
					else
					{
						current_mode = IN_SELECT;
					}
				}
				else if((strcmp(tk->stt.tag_name, "optgroup") == 0) ||
						(strcmp(tk->stt.tag_name, "option") == 0))
				{
					element_node *e;

					//If the current node is an option element, then act as if an end tag with the tag name "option" had been seen.
					if(current_node->type == ELEMENT_N)
					{
						element_node *curr_element = (element_node *)current_node;

						if(strcmp(curr_element->name, "option") == 0)
						{
							current_node = current_node->parent;
						}
					}

					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

				}
				else if((strcmp(tk->stt.tag_name, "rp") == 0) ||
						(strcmp(tk->stt.tag_name, "rt") == 0))
				{
					element_node *e;
					node *match_node;
					if(has_element_in_scope((node *)doc_root, current_node, "ruby", &match_node) == 1)
					{
						current_node = generate_implied_end_tags(current_node);
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

				}
				else if(strcmp(tk->stt.tag_name, "math") == 0)
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);
					
					//Adjust MathML attributes for the token. (This fixes the case of MathML attributes that are not all lowercase.)
					//Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink.)
					//Insert a foreign element for the token, in the MathML namespace .
					//If the token has its self-closing flag set, pop the current node off the stack of open elements and acknowledge the token's self-closing flag .
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, MATHML);
					add_child_node(current_node, (node *)e);

					if(tk->stt.self_closing_flag == SET)
					{
						;
					}
					else
					{
						current_node = (node *)e;
					}
				}
				else if(strcmp(tk->stt.tag_name, "svg") == 0)
				{
					element_node *e;

					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					//Adjust SVG attributes for the token. (This fixes the case of SVG attributes that are not all lowercase.)
					//Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink in SVG.)
					
					//Insert a foreign element for the token, in the SVG namespace .
					//If the token has its self-closing flag set, pop the current node off the stack of open elements and acknowledge the token's self-closing flag.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, SVG);
					add_child_node(current_node, (node *)e);

					if(tk->stt.self_closing_flag == SET)
					{
						;
					}
					else
					{
						current_node = (node *)e;
					}
				}
				else if((strcmp(tk->stt.tag_name, "caption") == 0) ||
						(strcmp(tk->stt.tag_name, "col") == 0) ||
						(strcmp(tk->stt.tag_name, "colgroup") == 0) ||
						(strcmp(tk->stt.tag_name, "frame") == 0) ||
						(strcmp(tk->stt.tag_name, "head") == 0) ||
						(strcmp(tk->stt.tag_name, "tbody") == 0) ||
						(strcmp(tk->stt.tag_name, "td") == 0) ||
						(strcmp(tk->stt.tag_name, "tfoot") == 0) ||
						(strcmp(tk->stt.tag_name, "th") == 0) ||
						(strcmp(tk->stt.tag_name, "thead") == 0) ||
						(strcmp(tk->stt.tag_name, "tr") == 0))
				{
					;//parse error, ignore the token
				}
				else
				{
					element_node *e;
					//This element will be an ordinary element.

					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);
					

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);

					if(tk->stt.self_closing_flag == SET)
					{
						;
					}
					else
					{
						current_node = (node *)e;
					}
				}

			}
			break;
		case TOKEN_END_TAG:
			{
				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}

				if(strcmp(tk->ett.tag_name, "body") == 0)
				{
					node *match_node;

					if(has_element_in_scope((node *)doc_root, current_node, "body", &match_node) == 0)
					{
						//parser error, ignore the token
					}
					else
					{
						current_mode = AFTER_BODY;
					}
				}
				else if(strcmp(tk->ett.tag_name, "html") == 0)
				{
					node *match_node;

					if(has_element_in_scope((node *)doc_root, current_node, "body", &match_node) == 0)
					{
						//parser error, ignore the token
					}
					else
					{
						current_mode = AFTER_BODY;
						token_process = REPROCESS;
					}
				}
				else if(strcmp(tk->ett.tag_name, "p") == 0)
				{
					node *match_node;
					if(has_element_in_button_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					else
					{
							//parse error, act as if a start tag with the tag name "p" had been seen, 
							//then reprocess the current token.
						element_node *e = create_element_node("p", NULL, HTML);
						add_child_node(current_node, (node *)e);
						current_node = (node *)e;

						token_process = REPROCESS;
					}
					
				}
				else if((strcmp(tk->ett.tag_name, "address") == 0) ||
						(strcmp(tk->ett.tag_name, "article") == 0) ||
						(strcmp(tk->ett.tag_name, "aside") == 0) ||
						(strcmp(tk->ett.tag_name, "blockquote") == 0) ||
						(strcmp(tk->ett.tag_name, "button") == 0) ||
						(strcmp(tk->ett.tag_name, "center") == 0) ||
						(strcmp(tk->ett.tag_name, "details") == 0) ||
						(strcmp(tk->ett.tag_name, "dir") == 0) ||
						(strcmp(tk->ett.tag_name, "div") == 0) ||
						(strcmp(tk->ett.tag_name, "dl") == 0) ||
						(strcmp(tk->ett.tag_name, "fieldset") == 0) ||
						(strcmp(tk->ett.tag_name, "figcaption") == 0) ||
						(strcmp(tk->ett.tag_name, "figure") == 0) ||
						(strcmp(tk->ett.tag_name, "footer") == 0) ||
						(strcmp(tk->ett.tag_name, "header") == 0) ||
						(strcmp(tk->ett.tag_name, "hgroup") == 0) ||
						(strcmp(tk->ett.tag_name, "listing") == 0) ||
						(strcmp(tk->ett.tag_name, "menu") == 0) ||
						(strcmp(tk->ett.tag_name, "nav") == 0) ||
						(strcmp(tk->ett.tag_name, "ol") == 0) ||
						(strcmp(tk->ett.tag_name, "pre") == 0) ||
						(strcmp(tk->ett.tag_name, "section") == 0) ||
						(strcmp(tk->ett.tag_name, "summary") == 0) ||
						(strcmp(tk->ett.tag_name, "ul") == 0))
				{
					node *match_node;
					if(has_element_in_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					else
					{
						; 	//parse error, ignore the token
					}

				}
				else if(strcmp(tk->ett.tag_name, "form") == 0)
				{
					node *match_node;
					element_node *form_element_node;
					
					form_element_node = form_element_ptr;

					form_element_ptr = NULL;

					if((form_element_node == NULL) ||
						(has_element_in_scope((node *)doc_root, current_node, "form", &match_node) == 0))
					{
						; 	//parse error, ignore the token
					}
					else
					{
						current_node = match_node->parent;		//to have come here, has_element_in_scope() must have had return value of 1 with match_node non-NULL.  
					}

				}
				else if((strcmp(tk->ett.tag_name, "h1") == 0) ||
						(strcmp(tk->ett.tag_name, "h2") == 0) ||
						(strcmp(tk->ett.tag_name, "h3") == 0) ||
						(strcmp(tk->ett.tag_name, "h4") == 0) ||
						(strcmp(tk->ett.tag_name, "h5") == 0) ||
						(strcmp(tk->ett.tag_name, "h6") == 0))
				{
					node *match_node;

					if((has_element_in_scope((node *)doc_root, current_node, "h1", &match_node) == 0) &&
					   (has_element_in_scope((node *)doc_root, current_node, "h2", &match_node) == 0) &&
					   (has_element_in_scope((node *)doc_root, current_node, "h3", &match_node) == 0) &&
					   (has_element_in_scope((node *)doc_root, current_node, "h4", &match_node) == 0) &&
					   (has_element_in_scope((node *)doc_root, current_node, "h5", &match_node) == 0) &&
					   (has_element_in_scope((node *)doc_root, current_node, "h6", &match_node) == 0))
					{
						;	//parse error, ignore the token
					}
					else
					{
						node *temp_node = current_node;
						
						if(temp_node->type != ELEMENT_N)
						{
							temp_node = temp_node->parent;
						}

						while(temp_node != (node *)doc_root)
						{
							element_node *temp_element = (element_node *)temp_node;
							
							if((strcmp(temp_element->name, "h1") == 0) ||
							   (strcmp(temp_element->name, "h2") == 0) ||
							   (strcmp(temp_element->name, "h3") == 0) ||
							   (strcmp(temp_element->name, "h4") == 0) ||
							   (strcmp(temp_element->name, "h5") == 0) ||
							   (strcmp(temp_element->name, "h6") == 0))
							{
								current_node = temp_element->parent;
								break;
							}

							temp_node = temp_node->parent;
						}
					}
				}
				else if(strcmp(tk->ett.tag_name, "li") == 0)
				{
					node *match_node;
					if(has_element_in_list_item_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					else
					{
						; 	//parse error, ignore the token
					}
				}
				else if((strcmp(tk->ett.tag_name, "dd") == 0) ||
						(strcmp(tk->ett.tag_name, "dt") == 0))
				{
					node *match_node;
					if(has_element_in_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = match_node->parent;
					}
					else
					{
						; 	//parse error, ignore the token
					}

				}
				else if((strcmp(tk->ett.tag_name, "a") == 0) ||
						(strcmp(tk->ett.tag_name, "b") == 0) ||
				        (strcmp(tk->ett.tag_name, "big") == 0) ||
				        (strcmp(tk->ett.tag_name, "code") == 0) ||
						(strcmp(tk->ett.tag_name, "em") == 0) ||
						(strcmp(tk->ett.tag_name, "font") == 0) ||
						(strcmp(tk->ett.tag_name, "i") == 0) ||
						(strcmp(tk->ett.tag_name, "nobr") == 0) ||
						(strcmp(tk->ett.tag_name, "s") == 0) ||
						(strcmp(tk->ett.tag_name, "small") == 0) ||
						(strcmp(tk->ett.tag_name, "strike") == 0) ||
						(strcmp(tk->ett.tag_name, "strong") == 0) ||
						(strcmp(tk->ett.tag_name, "tt") == 0) ||
						(strcmp(tk->ett.tag_name, "u") == 0))

				{
					/*----------------------Adoption Agency Algorithm-------------------------------------*/
					adoption_agency_algorithm((node *)doc_root, &active_formatting_elements, &current_node, tk->ett.tag_name);

				}
				else if((strcmp(tk->ett.tag_name, "applet") == 0) ||
						(strcmp(tk->ett.tag_name, "marquee") == 0) ||
						(strcmp(tk->ett.tag_name, "object") == 0))
				{
					node *match_node;

					//If the stack of open elements does not have an element in scope with the same tag name as that
					//of the token, then this is a parse error ; ignore the token.
					if(has_element_in_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = match_node->parent;

						//Clear the list of active formatting elements up to the last marker .
						active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);

					}
					else
					{
						; 	//parse error, ignore the token
					}

				}
				else if(strcmp(tk->ett.tag_name, "br") == 0)
				{
					//parse error . Act as if a start tag token with the tag name "br" had been seen. 
					//ignore the end tag token.
					element_node *e;
					
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements,  (node *)doc_root, current_node);

					e = create_element_node("br", NULL, HTML);
					add_child_node(current_node, (node *)e);
					
					
					//Acknowledge the token's self-closing flag, if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->ett.tag_name, "sarcasm") == 0)
				{
					//act as described in the "any other end tag" entry below
					node *temp_node = current_node;
						
					if(temp_node->type != ELEMENT_N)
					{
						temp_node = temp_node->parent;
					}

					while(temp_node != (node *)doc_root)
					{
						element_node *temp_element = (element_node *)temp_node;
						
						if(strcmp(temp_element->name, tk->ett.tag_name) == 0)
						{
							current_node = temp_element->parent;
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

				}
				else
				{
					node *temp_node = current_node;
						
					if(temp_node->type != ELEMENT_N)
					{
						temp_node = temp_node->parent;
					}

					while(temp_node != (node *)doc_root)
					{
						element_node *temp_element = (element_node *)temp_node;
						
						if(strcmp(temp_element->name, tk->ett.tag_name) == 0)
						{
							current_node = temp_element->parent;
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
				}
			}

			break;
		case TOKEN_EOF:
			//implementation pending
			break;
		default:
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void text_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if(current_node->type == TEXT_N)
				{
					text_node *t = (text_node *)current_node;
					t->text_data = string_append(t->text_data, tk->cht.ch);
				}
				else
				{
					text_node *t = create_text_node();
					t->text_data = string_append(t->text_data, tk->cht.ch);
					/*if current node is 'textarea' ignore LINE_FEED characters at the beginning of text*/
					add_child_node(current_node, (node *)t);
					current_node = (node *)t;
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "script") == 0)
				{
					if(current_node->type == TEXT_N)
					{
						current_node = current_node->parent->parent;
					}
					else
					{
						current_node = current_node->parent;
					}

					//a special case for the "script" start tag in "AFTER_HEAD" mode
					if(original_insertion_mode == AFTER_HEAD)
					{
						current_node = head_element_ptr->parent;
					}

					current_mode = original_insertion_mode;

				}
				else if((strcmp(tk->ett.tag_name, "noframes") == 0) ||
						(strcmp(tk->ett.tag_name, "style") == 0) ||
						(strcmp(tk->ett.tag_name, "title") == 0))
				{
					if(current_node->type == TEXT_N)
					{
						current_node = current_node->parent->parent;
					}
					else
					{
						current_node = current_node->parent;
					}

					//a special case for the start tags "noframes", "style", "title"  in "AFTER_HEAD" mode
					if(original_insertion_mode == AFTER_HEAD)
					{
						current_node = head_element_ptr->parent;
					}

					current_mode = original_insertion_mode;

				}
				else		//any other end tag
				{
					if(current_node->type == TEXT_N)
					{
						current_node = current_node->parent->parent;
					}
					else
					{
						current_node = current_node->parent;
					}

					current_mode = original_insertion_mode;
				}
			}
			break;
		case TOKEN_EOF:
			{
				if(current_node->type == ELEMENT_N)
				{
					element_node *e = (element_node *)current_node;
					if(strcmp(e->name, "script") == 0)
					{
						;//implementation pending: mark the script element as "already started" 
					}
				}

				current_node = current_node->parent;

				current_mode = original_insertion_mode;
				token_process = REPROCESS;
			}
			break;
		default:
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void in_table_mode(token *tk)
{ 

	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				original_insertion_mode = current_mode;
				current_mode = IN_TABLE_TEXT;
				token_process = REPROCESS;
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
		    {
				if(strcmp(tk->stt.tag_name, "caption") == 0)
				{
					//Clear the stack back to a table context.
					//Insert a marker at the end of the list of active formatting elements .
					//Insert an HTML element for the token, then switch the insertion mode to "in caption "
					element_node *e;
					node *table_context_node;

					table_context_node = back_to_table_context((node *)doc_root, current_node);
					if(table_context_node != NULL)
					{
						current_node = table_context_node;
					}

					active_formatting_elements = active_formatting_list_add_marker(active_formatting_elements);

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_CAPTION;
				}
				else if(strcmp(tk->stt.tag_name, "colgroup") == 0)
				{
					//Clear the stack back to a table context.
					//Insert an HTML element for the token, then switch the insertion mode to "in column group "
					element_node *e;
					node *table_context_node;

					table_context_node = back_to_table_context((node *)doc_root, current_node);
					if(table_context_node != NULL)
					{
						current_node = table_context_node;
					}
					
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_COLUMN_GROUP;
				}
				else if(strcmp(tk->stt.tag_name, "col") == 0)
				{	
					//Act as if a start tag token with the tag name "colgroup" had been seen, 
					//then reprocess the current token.
					
					element_node *e;
					node *table_context_node;

					table_context_node = back_to_table_context((node *)doc_root, current_node);
					if(table_context_node != NULL)
					{
						current_node = table_context_node;
					}
					
					e = create_element_node("colgroup", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_COLUMN_GROUP;
					token_process = REPROCESS;
				}
				else if((strcmp(tk->stt.tag_name, "tbody") == 0) ||
						(strcmp(tk->stt.tag_name, "tfoot") == 0) ||
						(strcmp(tk->stt.tag_name, "thead") == 0))
				{
					element_node *e;
					node *table_context_node;

					table_context_node = back_to_table_context((node *)doc_root, current_node);
					if(table_context_node != NULL)
					{
						current_node = table_context_node;
					}
					
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_TABLE_BODY;

				}
				else if((strcmp(tk->stt.tag_name, "td") == 0) ||
						(strcmp(tk->stt.tag_name, "th") == 0) ||
						(strcmp(tk->stt.tag_name, "tr") == 0))
				{
					element_node *e;
					node *table_context_node;

					table_context_node = back_to_table_context((node *)doc_root, current_node);
					if(table_context_node != NULL)
					{
						current_node = table_context_node;
					}


					e = create_element_node("tbody", NULL, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;

					current_mode = IN_TABLE_BODY;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->stt.tag_name, "table") == 0)
				{
					//parse error
					//Act as if an end tag token with the tag name "table" had been seen, then, if that token
					//wasn't ignored, reprocess the current token.
					node *match_node;
					if(has_element_in_table_scope((node *)doc_root, current_node, "table", &match_node) == 1)
					{
						current_node = match_node->parent;

						//Reset the insertion mode appropriately
						current_mode = reset_insertion_mode((node *)doc_root, current_node);
						token_process = REPROCESS;
					}
					else
					{
						//parse error, ignore the token
					}	
				}
				else if((strcmp(tk->stt.tag_name, "style") == 0) ||
						(strcmp(tk->stt.tag_name, "script") == 0))
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "input") == 0)
				{
					//If the token does not have an attribute with the name "type", or if it does, but that attribute's value is not
					//an ASCII case-insensitive match for the string "hidden", then: act as described in the "anything else" entry below.
					//Otherwise:
					//Parse error .
					//Insert an HTML element for the token.
					//Pop that input element off the stack of open elements .
					; //implementation pending
				}
				else if(strcmp(tk->stt.tag_name, "form") == 0)
				{
					//Parse error .
					//If the form element pointer is not null, ignore the token.
					//Otherwise:
					//Insert an HTML element for the token, and set the form element pointer to point to the element created.
					//Pop that form element off the stack of open elements .
					if(form_element_ptr != NULL)
					{
						; //ignore the token
					}
					else
					{
						element_node *e = create_element_node(tk->stt.tag_name, 
															  tk->stt.attributes, HTML);
						add_child_node(current_node, (node *)e);
						
						form_element_ptr = e;
					}
				}
				else
				{
					; //implementation pending
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "table") == 0)
				{	
					node *match_node;
					if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = match_node->parent;

						//Reset the insertion mode appropriately
						current_mode = reset_insertion_mode((node *)doc_root, current_node);
					}
					else
					{
						//parse error, ignore the token
					}

				}
				else if((strcmp(tk->ett.tag_name, "body") == 0) ||
						(strcmp(tk->ett.tag_name, "caption") == 0) ||
						(strcmp(tk->ett.tag_name, "col") == 0) ||
						(strcmp(tk->ett.tag_name, "colgroup") == 0) ||
						(strcmp(tk->ett.tag_name, "html") == 0) ||
						(strcmp(tk->ett.tag_name, "tbody") == 0) ||
						(strcmp(tk->ett.tag_name, "td") == 0) ||
						(strcmp(tk->ett.tag_name, "tfoot") == 0) ||
						(strcmp(tk->ett.tag_name, "th") == 0) ||
						(strcmp(tk->ett.tag_name, "thead") == 0) ||
						(strcmp(tk->ett.tag_name, "tr") == 0))
				{
					//parse error, ignore the token
				}
				else
				{
					; //implementation pending
				}

			}
			break;
		default:
			{
				//Parse error . Process the token using the rules for the "in body " insertion mode , except that
				//if the current node is a table , tbody , tfoot , thead , or tr element, then, whenever a
				//node would be inserted into the current node , it must instead be foster parented .
				   
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void in_table_text_mode(token *tk)
{

	if(tk->type == TOKEN_CHARACTER)
	{
		if(tk->cht.ch == NULL_CHARACTER)
		{
			;//parse error, ignore the token.
		}
		else
		{
			pending_table_characters = string_append(pending_table_characters, tk->cht.ch);

			if((tk->cht.ch != CHARACTER_TABULATION) &&
			   (tk->cht.ch != LINE_FEED) &&
			   (tk->cht.ch != FORM_FEED) &&
			   (tk->cht.ch != CARRIAGE_RETURN) &&
			   (tk->cht.ch != SPACE))
			{
				apply_foster_parenting = 1;
			}
		}

	}
	else
	{	
		text_node *t = create_text_node();

		t->text_data = strdup(pending_table_characters);

		if(apply_foster_parenting == 1)
		{
			add_child_to_foster_parent((node *)doc_root, current_node, (node *)t);
		}
		else
		{
			add_child_node(current_node, (node *)t);
		}

		free(pending_table_characters);
		pending_table_characters = NULL;
		apply_foster_parenting = 0;
		

		current_mode = original_insertion_mode;
		token_process = REPROCESS;
	}

}


/*------------------------------------------------------------------------------------*/
void in_caption_mode(token *tk)
{ 

	if((tk->type == TOKEN_START_TAG) &&
	   ((strcmp(tk->stt.tag_name, "caption") == 0) ||
		(strcmp(tk->stt.tag_name, "col") == 0) ||
		(strcmp(tk->stt.tag_name, "colgroup") == 0) ||
		(strcmp(tk->stt.tag_name, "tbody") == 0) ||
		(strcmp(tk->stt.tag_name, "td") == 0) ||
		(strcmp(tk->stt.tag_name, "tfoot") == 0) ||
		(strcmp(tk->stt.tag_name, "th") == 0) ||
		(strcmp(tk->stt.tag_name, "thead") == 0) ||
		(strcmp(tk->stt.tag_name, "tr") == 0)))
	{
		//parse error
		//Act as if an end tag with the tag name "caption" had been seen, then, 
		//if that token wasn't ignored, reprocess the current token.
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, "caption", &match_node) == 1)
		{
			current_node = match_node->parent;
			//Clear the list of active formatting elements up to the last marker.
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			current_mode = IN_TABLE;
			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "caption") == 0))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1)
		{
			current_node = match_node->parent;
			//Clear the list of active formatting elements up to the last marker.
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			current_mode = IN_TABLE;
		}
		else
		{
			;//parse error, ignore the token
		}

	}
	else if((tk->type == TOKEN_END_TAG) &&
		(strcmp(tk->ett.tag_name, "table") == 0))
	{
		//parse error
		//Act as if an end tag with the tag name "caption" had been seen, then, 
		//if that token wasn't ignored, reprocess the current token.
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, "caption", &match_node) == 1)
		{
			current_node = match_node->parent;
			//Clear the list of active formatting elements up to the last marker.
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			current_mode = IN_TABLE;
			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "body") == 0) ||
				(strcmp(tk->ett.tag_name, "col") == 0) ||
				(strcmp(tk->ett.tag_name, "colgroup") == 0) ||
				(strcmp(tk->ett.tag_name, "html") == 0) ||
				(strcmp(tk->ett.tag_name, "tbody") == 0) ||
				(strcmp(tk->ett.tag_name, "td") == 0) ||
				(strcmp(tk->ett.tag_name, "tfoot") == 0) ||
				(strcmp(tk->ett.tag_name, "th") == 0) ||
				(strcmp(tk->ett.tag_name, "thead") == 0) ||
				(strcmp(tk->ett.tag_name, "tr") == 0)))
	{
		;//parse error, ignore the token
	}
	else
	{
		//Process the token using the rules for the "in body" insertion mode.
		in_body_mode(tk);
	}
}

/*------------------------------------------------------------------------------------*/
void in_column_group_mode(token *tk)
{

	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					//insert the character into the current node

					if(current_node->type == TEXT_N)
					{
						text_node *t = (text_node *)current_node;
						t->text_data = string_append(t->text_data, tk->cht.ch);
					}
					else
					{
						text_node *t = create_text_node();
						t->text_data = string_append(t->text_data, tk->cht.ch);
						add_child_node(current_node, (node *)t);
						current_node = (node *)t;
					}
				}
				else
				{
					//Act as if an end tag with the tag name "colgroup" had been seen, 
					//and then, if that token wasn't ignored, reprocess the current token.

					element_node *temp_element_node;

					if(current_node->type != ELEMENT_N)
					{
						current_node = current_node->parent;
					}
					temp_element_node = (element_node *)current_node;

					
					if(strcmp(temp_element_node->name, "html") == 0)
					{
						;//parse error, ignore the token
					}
					else
					{
						current_node = current_node->parent;
						current_mode = IN_TABLE;
						token_process = REPROCESS;
					}
				}
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);

				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}
				
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			if(current_node->type == TEXT_N)
			{
				current_node = current_node->parent;
			}
			break;
		case TOKEN_START_TAG:
		    {
				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}

				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body " insertion mode.

					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "col") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					add_child_node(current_node, (node *)e);

					//Acknowledge the token's self-closing flag , if it is set.
				}
				else
				{
					//Act as if an end tag with the tag name "colgroup" had been seen, 
					//and then, if that token wasn't ignored, reprocess the current token.

					element_node *temp_element_node = (element_node *)current_node;
					//it's safe to cast current_node to (element_node *) here
					//as current_node cannot be TEXT_N

					if(strcmp(temp_element_node->name, "html") == 0)
					{
						;//parse error, ignore the token
					}
					else
					{
						current_node = current_node->parent;
						current_mode = IN_TABLE;
						token_process = REPROCESS;
					}
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}

				if(strcmp(tk->ett.tag_name, "colgroup") == 0)
				{
					//it's safe to cast current_node to (element_node *) here
					//as current_node cannot be TEXT_N
					element_node *temp_element_node = (element_node *)current_node;
					
					if(strcmp(temp_element_node->name, "html") == 0)
					{
						;//parse error, ignore the token
					}
					else
					{
						current_node = current_node->parent;

						current_mode = IN_TABLE;
					}
				}
				else if(strcmp(tk->ett.tag_name, "col") == 0)
				{
					;//parse error, ignore the token
				}
				else
				{
					//Act as if an end tag with the tag name "colgroup" had been seen, 
					//and then, if that token wasn't ignored, reprocess the current token.

					element_node *temp_element_node = (element_node *)current_node;
					//it's safe to cast current_node to (element_node *) here
					//as current_node cannot be TEXT_N
					
					if(strcmp(temp_element_node->name, "html") == 0)
					{
						;//parse error, ignore the token
					}
					else
					{
						current_node = current_node->parent;
						current_mode = IN_TABLE;
						token_process = REPROCESS;
					}
				}
			}
			break;
		default:
			{
				//Act as if an end tag with the tag name "colgroup" had been seen, 
				//and then, if that token wasn't ignored, reprocess the current token.

				element_node *temp_element_node;

				if(current_node->type != ELEMENT_N)
				{
					current_node = current_node->parent;
				}
				temp_element_node = (element_node *)current_node;

					
				if(strcmp(temp_element_node->name, "html") == 0)
				{
					;//parse error, ignore the token
				}
				else
				{
					current_node = current_node->parent;
					current_mode = IN_TABLE;
					token_process = REPROCESS;
				}

			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void in_table_body_mode(token *tk)
{ 
	if((tk->type == TOKEN_START_TAG) &&
	   (strcmp(tk->stt.tag_name, "tr") == 0))
	{
		element_node *e;
		node *table_body_context_node = back_to_table_body_context((node *)doc_root, current_node);

		if(table_body_context_node != NULL)
		{
			current_node = table_body_context_node;
		}

		e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
		add_child_node(current_node, (node *)e);
		current_node = (node *)e;

		current_mode = IN_ROW;
	}
	else if((tk->type == TOKEN_START_TAG) &&
			((strcmp(tk->stt.tag_name, "th") == 0) ||
			 (strcmp(tk->stt.tag_name, "td") == 0)))
	{
		//parse error
		//Act as if a start tag with the tag name "tr" had been seen, then reprocess the current token.
		element_node *e;
		node *table_body_context_node = back_to_table_body_context((node *)doc_root, current_node);

		if(table_body_context_node != NULL)
		{
			current_node = table_body_context_node;
		}

		e = create_element_node("tr", NULL, HTML);
		add_child_node(current_node, (node *)e);
		current_node = (node *)e;

		current_mode = IN_ROW;
		token_process = REPROCESS;
	}
	else if((tk->type == TOKEN_START_TAG) &&
			((strcmp(tk->stt.tag_name, "caption") == 0) ||
			 (strcmp(tk->stt.tag_name, "col") == 0) ||
			 (strcmp(tk->stt.tag_name, "colgroup") == 0) ||
			 (strcmp(tk->stt.tag_name, "tbody") == 0) ||
			 (strcmp(tk->stt.tag_name, "tfoot") == 0) ||
			 (strcmp(tk->stt.tag_name, "thead") == 0)))
	{
		node *match_node;
		if((has_element_in_table_scope((node *)doc_root, current_node, "tbody", &match_node) == 0) &&
		   (has_element_in_table_scope((node *)doc_root, current_node, "thead", &match_node) == 0) &&
		   (has_element_in_table_scope((node *)doc_root, current_node, "tfoot", &match_node) == 0))
		{
			;//parse error, ignore the token
		}
		else
		{
			node *table_body_context_node = back_to_table_body_context((node *)doc_root, current_node);

			if(table_body_context_node != NULL)
			{
				current_node = table_body_context_node;
				//current_node should be "tbody", "thead", or"tfoot".
			}
			//Act as if an end tag with the same tag name as the current node("tbody", "tfoot", or "thead") had
			//been seen, then reprocess the current token.
			current_node = current_node->parent;	//back to "table".

			current_mode = IN_TABLE;
			token_process = REPROCESS;

		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "table") == 0))
	{
		node *match_node;
		if((has_element_in_table_scope((node *)doc_root, current_node, "tbody", &match_node) == 0) &&
		   (has_element_in_table_scope((node *)doc_root, current_node, "thead", &match_node) == 0) &&
		   (has_element_in_table_scope((node *)doc_root, current_node, "tfoot", &match_node) == 0))
		{
			;//parse error, ignore the token
		}
		else
		{
			node *table_body_context_node = back_to_table_body_context((node *)doc_root, current_node);

			if(table_body_context_node != NULL)
			{
				current_node = table_body_context_node;
				//current_node should be "tbody", "thead", or"tfoot".
			}
			//Act as if an end tag with the same tag name as the current node("tbody", "tfoot", or "thead") had
			//been seen, then reprocess the current token.
			current_node = current_node->parent;	//back to "table".

			current_mode = IN_TABLE;
			token_process = REPROCESS;

		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			 ((strcmp(tk->ett.tag_name, "tbody") == 0) ||
			  (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			  (strcmp(tk->ett.tag_name, "thead") == 0)))

	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
		{
			node *table_body_context_node = back_to_table_body_context((node *)doc_root, current_node);

			if(table_body_context_node != NULL)
			{
				current_node = table_body_context_node;
				//current_node should be "tbody", "thead", or"tfoot".
			}

			current_node = current_node->parent;

			current_mode = IN_TABLE;

		}
		else
		{
			//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			 ((strcmp(tk->ett.tag_name, "body") == 0) ||
			  (strcmp(tk->ett.tag_name, "caption") == 0) ||
			  (strcmp(tk->ett.tag_name, "col") == 0) ||
			  (strcmp(tk->ett.tag_name, "colgroup") == 0) ||
			  (strcmp(tk->ett.tag_name, "html") == 0) ||
			  (strcmp(tk->ett.tag_name, "td") == 0) ||
			  (strcmp(tk->ett.tag_name, "th") == 0) ||
			  (strcmp(tk->ett.tag_name, "tr") == 0)))

	{
		;//parse error, ignore the token
	}
	else
	{
		//Process the token using the rules for the "in table " insertion mode 
		in_table_mode(tk);
	}
}

/*------------------------------------------------------------------------------------*/
void in_row_mode(token *tk)
{
	if((tk->type == TOKEN_START_TAG) &&
		((strcmp(tk->stt.tag_name, "th") == 0) ||
		 (strcmp(tk->stt.tag_name, "td") == 0)))
	{
		element_node *e;
		node *table_row_context_node = back_to_table_row_context((node *)doc_root, current_node);

		if(table_row_context_node != NULL)
		{
			current_node = table_row_context_node;
		}

		e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
		add_child_node(current_node, (node *)e);
		current_node = (node *)e;

		current_mode = IN_CELL;
		
		//Insert a marker at the end of the list of active formatting elements.
		active_formatting_elements = active_formatting_list_add_marker(active_formatting_elements);
	}
	else if((tk->type == TOKEN_START_TAG) &&
		((strcmp(tk->stt.tag_name, "caption") == 0) ||
		 (strcmp(tk->stt.tag_name, "col") == 0) ||
		 (strcmp(tk->stt.tag_name, "colgroup") == 0) ||
		 (strcmp(tk->stt.tag_name, "tbody") == 0) ||
		 (strcmp(tk->stt.tag_name, "tfoot") == 0) ||
		 (strcmp(tk->stt.tag_name, "thead") == 0) ||
		 (strcmp(tk->stt.tag_name, "tr") == 0)))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, "tr", &match_node) == 1) 
		{
			node *table_row_context_node = back_to_table_row_context((node *)doc_root, current_node);

			if(table_row_context_node != NULL)
			{
				current_node = table_row_context_node;
				//current_node should be "tr"
			}
			
			current_node = current_node->parent;	// back to parent of "tr" node

			current_mode = IN_TABLE_BODY;

			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}
		
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "tr") == 0))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
		{
			node *table_row_context_node = back_to_table_row_context((node *)doc_root, current_node);

			if(table_row_context_node != NULL)
			{
				current_node = table_row_context_node;
				//current_node should be "tr"
			}
			
			current_node = current_node->parent;	// back to parent of "tr" node

			current_mode = IN_TABLE_BODY;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "table") == 0))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, "tr", &match_node) == 1) 
		{
			node *table_row_context_node = back_to_table_row_context((node *)doc_root, current_node);

			if(table_row_context_node != NULL)
			{
				current_node = table_row_context_node;
				//current_node should be "tr"
			}
			
			current_node = current_node->parent;	// back to parent of "tr" node

			current_mode = IN_TABLE_BODY;

			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "tbody") == 0) ||
			 (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			 (strcmp(tk->ett.tag_name, "thead") == 0)))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
		{
			current_node = match_node;

			current_mode = IN_TABLE_BODY;

			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "body") == 0) ||
			 (strcmp(tk->ett.tag_name, "caption") == 0) ||
			 (strcmp(tk->ett.tag_name, "col") == 0) ||
			 (strcmp(tk->ett.tag_name, "colgroup") == 0) ||
			 (strcmp(tk->ett.tag_name, "html") == 0) ||
			 (strcmp(tk->ett.tag_name, "td") == 0) ||
			 (strcmp(tk->ett.tag_name, "th") == 0)))
	{
		;//parse error, ignore the token
	}
	else
	{
		//Process the token using the rules for the "in table " insertion mode.
		in_table_mode(tk);
	}
 }

/*------------------------------------------------------------------------------------*/
void in_cell_mode(token *tk)
{ 
	if((tk->type == TOKEN_START_TAG) &&
		((strcmp(tk->stt.tag_name, "caption") == 0) ||
		 (strcmp(tk->stt.tag_name, "col") == 0) ||
		 (strcmp(tk->stt.tag_name, "colgroup") == 0) ||
		 (strcmp(tk->stt.tag_name, "tbody") == 0) ||
		 (strcmp(tk->stt.tag_name, "tfoot") == 0) ||
		 (strcmp(tk->stt.tag_name, "thead") == 0) ||
		 (strcmp(tk->stt.tag_name, "tr") == 0) ||
		 (strcmp(tk->stt.tag_name, "td") == 0) ||
		 (strcmp(tk->stt.tag_name, "th") == 0)))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, "td", &match_node) == 1) 
		{
			//close the cell and reprocess the current token.
			current_node = match_node->parent;

			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row ".
			current_mode = IN_ROW;
			token_process = REPROCESS;
			
		}
		else if(has_element_in_table_scope((node *)doc_root, current_node, "th", &match_node) == 1) 
		{
			//close the cell and reprocess the current token.
			current_node = match_node->parent;
			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row ".
			current_mode = IN_ROW;
			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token.
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "td") == 0) ||
			 (strcmp(tk->ett.tag_name, "th") == 0)))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
		{
			current_node = match_node->parent;
			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row "
			current_mode = IN_ROW;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "body") == 0) ||
			 (strcmp(tk->ett.tag_name, "caption") == 0) ||
			 (strcmp(tk->ett.tag_name, "col") == 0) ||
			 (strcmp(tk->ett.tag_name, "colgroup") == 0) ||
			 (strcmp(tk->ett.tag_name, "html") == 0)))
	{
		;//parse error, ignore the token
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "table") == 0) ||
			 (strcmp(tk->ett.tag_name, "tbody") == 0) ||
			 (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			 (strcmp(tk->ett.tag_name, "thead") == 0) ||
			 (strcmp(tk->ett.tag_name, "tr") == 0)))
	{
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
		{
			//close the cell and reprocess the current token.
			if(has_element_in_table_scope((node *)doc_root, current_node, "td", &match_node) == 1) 
			{
				current_node = match_node->parent;
			}
			if(has_element_in_table_scope((node *)doc_root, current_node, "th", &match_node) == 1) 
			{
				current_node = match_node->parent;
			}
		
			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row ".
			current_mode = IN_ROW;
			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}

	}
	else
	{
		//Process the token using the rules for the "in body" insertion mode
		in_body_mode(tk);
	}
}

/*------------------------------------------------------------------------------------*/
void in_select_mode(token *tk)
{
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if(tk->cht.ch == NULL_CHARACTER)
				{
					//parse error
					//ignore the token
					;
				}
				else
				{
					//Insert the token's character into the current node .

					if(current_node->type == TEXT_N)
					{
						text_node *t = (text_node *)current_node;
						t->text_data = string_append(t->text_data, tk->cht.ch);
					}
					else
					{
						text_node *t = create_text_node();
						t->text_data = string_append(t->text_data, tk->cht.ch);
						add_child_node(current_node, (node *)t);
						current_node = (node *)t;
					}
				}

			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);

				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}
			
				add_child_node(current_node, (node *)c);	
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			{
				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}
			}
			break;
		case TOKEN_START_TAG:
			{
				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}

				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body " insertion mode .
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "option") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					element_node *curr_element_node = (element_node *)current_node;
					//safe to cast type to (element_node *) here.
					//as the current_node cannot be TEXT_N

					if(strcmp(curr_element_node->name, "option") == 0)
					{
						current_node = current_node->parent;
					}
					
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;
				}
				else if(strcmp(tk->stt.tag_name, "optgroup") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					element_node *curr_element_node;

					curr_element_node = (element_node *)current_node;
					if(strcmp(curr_element_node->name, "option") == 0)
					{
						current_node = current_node->parent;
					}

					curr_element_node = (element_node *)current_node;
					if(strcmp(curr_element_node->name, "optgroup") == 0)
					{
						current_node = current_node->parent;
					}

					add_child_node(current_node, (node *)e);
					current_node = (node *)e;
				}
				else if(strcmp(tk->stt.tag_name, "select") == 0)
				{
					//parse error . Act as if the token had been an end tag with the tag name "select" instead.
					node *match_node;
					if(has_element_in_select_scope((node *)doc_root, current_node, "select", &match_node) == 1) 
					{
						current_node = match_node->parent;

						//Reset the insertion mode appropriately.
						current_mode = reset_insertion_mode((node *)doc_root, current_node);
					}
					else
					{
						;//parse error, ignore the token
					}
				}
				else if((strcmp(tk->stt.tag_name, "input") == 0) ||
					    (strcmp(tk->stt.tag_name, "keygen") == 0) ||
						(strcmp(tk->stt.tag_name, "textarea") == 0))
				{
					//parse error.
					//If the stack of open elementsdoes not have a select element in select scope, ignore the token.
					//Otherwise, act as if an end tag with the tag name "select" had been seen, and reprocess the token.
					node *match_node;
					if(has_element_in_select_scope((node *)doc_root, current_node, "select", &match_node) == 1) 
					{
						current_node = match_node->parent;

						//Reset the insertion mode appropriately.
						current_mode = reset_insertion_mode((node *)doc_root, current_node);
						token_process = REPROCESS;
					}
					else
					{
						;//parse error, ignore the token
					}
				}
				else if(strcmp(tk->stt.tag_name, "script") == 0)
				{
					//Process the token using the rules for the "in head " insertion mode
					in_head_mode(tk);
				}
				else
				{
					//parse error, ignore the token
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(current_node->type == TEXT_N)
				{
					current_node = current_node->parent;
				}

				if(strcmp(tk->ett.tag_name, "optgroup") == 0)
				{
					element_node *curr_element_node = (element_node *)current_node;
					//safe to cast type

					if(strcmp(curr_element_node->name, "option") == 0)
					{
						current_node = current_node->parent;
					}

					curr_element_node = (element_node *)current_node;
					if(strcmp(curr_element_node->name, "optgroup") == 0)
					{
						current_node = current_node->parent;
					}
					else
					{
						;//parse error, ignore the token
					}
				}
				else if(strcmp(tk->ett.tag_name, "option") == 0)
				{
					element_node *curr_element_node = (element_node *)current_node;
						
					if(strcmp(curr_element_node->name, "option") == 0)
					{
						current_node = current_node->parent;
					}
					else
					{
						;//parse error, ignore the token
					}

				}
				else if(strcmp(tk->ett.tag_name, "select") == 0)
				{
					node *match_node;
					if(has_element_in_select_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
					{
						current_node = match_node->parent;

						//Reset the insertion mode appropriately.
						current_mode = reset_insertion_mode((node *)doc_root, current_node);
					}
					else
					{
						;//parse error, ignore the token
					}
				}
				else
				{
					//parse error, ignore the token
				}
			}
			break;
		default:
			{
				//parse error, ignore the token
			}
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void in_select_in_table_mode(token *tk)
{ 
	if((tk->type == TOKEN_START_TAG) &&
	   ((strcmp(tk->stt.tag_name, "caption") == 0) ||
	    (strcmp(tk->stt.tag_name, "table") == 0) ||
		(strcmp(tk->stt.tag_name, "tbody") == 0) ||
		(strcmp(tk->stt.tag_name, "tfoot") == 0) ||
		(strcmp(tk->stt.tag_name, "thead") == 0) ||
		(strcmp(tk->stt.tag_name, "tr") == 0) ||
		(strcmp(tk->stt.tag_name, "td") == 0) ||
		(strcmp(tk->stt.tag_name, "th") == 0)))
	{
		//parse error . Act as if an end tag with the tag name "select" had been seen, and reprocess the token.
		node *match_node;
		if(has_element_in_select_scope((node *)doc_root, current_node, "select", &match_node) == 1) 
		{
			current_node = match_node->parent;

			//Reset the insertion mode appropriately.
			current_mode = reset_insertion_mode((node *)doc_root, current_node);
			token_process = REPROCESS;
		}
		else
		{
			;//parse error, ignore the token
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "caption") == 0) ||
			 (strcmp(tk->ett.tag_name, "table") == 0) ||
			 (strcmp(tk->ett.tag_name, "tbody") == 0) ||
			 (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			 (strcmp(tk->ett.tag_name, "thead") == 0) ||
			 (strcmp(tk->ett.tag_name, "tr") == 0) ||
			 (strcmp(tk->ett.tag_name, "td") == 0) ||
			 (strcmp(tk->ett.tag_name, "th") == 0)))
	{
		//parse error
		node *match_node;
		if(has_element_in_table_scope((node *)doc_root, current_node, tk->ett.tag_name, &match_node) == 1) 
		{
			if(has_element_in_select_scope((node *)doc_root, current_node, "select", &match_node) == 1) 
			{
				current_node = match_node->parent;

				//Reset the insertion mode appropriately.
				current_mode = reset_insertion_mode((node *)doc_root, current_node);
				token_process = REPROCESS;
			}
			else
			{
				;//ignore the token
			}
		}
		else
		{
			;//ignore the token
		}
	}
	else
	{
		//Process the token using the rules for the "in select" insertion mode .
		in_select_mode(tk);
	}
			
}

/*------------------------------------------------------------------------------------*/
void after_body_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
			
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					//Process the token using the rules for the "in body " insertion mode.
					in_body_mode(tk);
				}
				else
				{
					//parse error
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_COMMENT:
			//append comment node to the first element in the stack of open elements (the html element),
			//with the data attribute set to the data given in the comment token.
			{
				element_node *html_node = get_node_by_name((node *)doc_root, current_node, "html");
				comment_node *c = create_comment_node(tk->cmt.comment);

				if(html_node != NULL)
				{
					add_child_node((node *)html_node, (node *)c);
				}
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
			{
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{					
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else
				{
					//parse error
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}

			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "html") == 0)
				{
					//If the parser was originally created as part of the HTML fragment parsing algorithm, 
					//this is a parse error ; ignore the token. (fragment case)
					//Otherwise, switch the insertion mode to "after after body".
					current_mode = AFTER_AFTER_BODY;
				}
				else
				{
					//parse error
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_EOF:
			//implementation pending
			//stop parsing;
			break;
		default:
			break;
	}

}



/*------------------------------------------------------------------------------------*/
void in_frameset_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				//parse error, ignore the token
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
		    {
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body " insertion mode .
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "frameset") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					current_node = (node *)e;
				}
				else if(strcmp(tk->stt.tag_name, "frame") == 0)
				{
					//Insert an HTML element for the token. Immediately pop the current node off the stack of open elements .
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					//Acknowledge the token's self-closing flag , if it is set.
				}
				else if(strcmp(tk->stt.tag_name, "noframes") == 0)
				{
					//Process the token using the rules for the "in head" insertion mode .
					in_head_mode(tk);
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "frameset") == 0)
				{
					element_node *curr_element_node = (element_node *)current_node;
					if(strcmp(curr_element_node->name, "html") == 0)
					{
						;//parse error, ignore the token
					}
					else
					{
						current_node = current_node->parent;
						//If the parser was not originally created as part of the HTML fragment parsing algorithm (fragment case ), 
						//and the current node is no longer a frameset element, then switch the insertion mode to "after frameset".
						curr_element_node = (element_node *)current_node;
						if(strcmp(curr_element_node->name, "frameset") != 0)
						{
							current_mode = AFTER_FRAMESET;
						}
					}
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		default:
			{
				//parse error, ignore the token
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void after_frameset_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				;//parse error, ignore the token
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
		    {
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body " insertion mode .
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "noframes") == 0)
				{
					//Process the token using the rules for the "in head" insertion mode .
					in_head_mode(tk);
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "html") == 0)
				{
					current_mode = AFTER_AFTER_FRAMESET;
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		default:
			{
				;//parse error, ignore the token
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void after_after_body_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
			
				if((tk->cht.ch == CHARACTER_TABULATION) ||
				   (tk->cht.ch == LINE_FEED) ||
				   (tk->cht.ch == FORM_FEED) ||
				   (tk->cht.ch == CARRIAGE_RETURN) ||
				   (tk->cht.ch == SPACE))
				{
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else
				{
					//parse error
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_COMMENT:
			//append a Comment node to the Document object with the data attribute set to 
			//the data given in the comment token.
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node((node *)doc_root, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			in_body_mode(tk);
			break;
		case TOKEN_START_TAG:
			{
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{					
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else
				{
					//parse error
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}

			}
			break;
		case TOKEN_END_TAG:
			//parse error
			//Switch the insertion mode to "in body" and reprocess the token.
			{
				current_mode = IN_BODY;
				token_process = REPROCESS;
			}
			break;
		case TOKEN_EOF:
			//stop parsing;
			break;
		default:
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void after_after_frameset_mode(token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				;//parse error, ignore the token
			}
			break;
		case TOKEN_COMMENT:
			{
				//Append a Comment node to the Document object with the data attribute set to the data given in the comment token.
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node((node *)doc_root, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			break;
		case TOKEN_START_TAG:
		    {
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body" insertion mode.
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "noframes") == 0)
				{
					//Process the token using the rules for the "in head " insertion mode .
					in_head_mode(tk);
				}
				else
				{
					;//parse error, ignore the token
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				;//parse error, ignore the token
			}
			break;
		default:
			{
				;//parse error, ignore the token
			}
			break;
	}
}


/*------------------------------------------------------------------------------------*/	
insertion_mode reset_insertion_mode(node *root, node *current_node)
{
	element_node *temp_element_node;
	
	if(current_node->type != ELEMENT_N)
	{
		current_node = current_node->parent;
	}

	temp_element_node = (element_node *)current_node;

	while((node *)temp_element_node != root)
	{
		if(strcmp(temp_element_node->name, "select") == 0)
		{
			return IN_SELECT;
		}
		else if((strcmp(temp_element_node->name, "td") == 0) ||
				(strcmp(temp_element_node->name, "th") == 0))
		{
			return IN_CELL;
		}
		else if(strcmp(temp_element_node->name, "tr") == 0)
		{
			return IN_ROW;
		}
		else if((strcmp(temp_element_node->name, "tbody") == 0) ||
				(strcmp(temp_element_node->name, "thead") == 0) ||
				(strcmp(temp_element_node->name, "tfoot") == 0))
		{
			return IN_TABLE_BODY;
		}
		else if(strcmp(temp_element_node->name, "caption") == 0)
		{
			return IN_CAPTION;
		}
		else if(strcmp(temp_element_node->name, "colgroup") == 0)
		{
			return IN_COLUMN_GROUP;
		}
		else if(strcmp(temp_element_node->name, "table") == 0)
		{
			return IN_TABLE;
		}
		else if(strcmp(temp_element_node->name, "head") == 0)
		{
			return IN_BODY;
		}
		else if(strcmp(temp_element_node->name, "body") == 0)
		{
			return IN_BODY;
		}
		else if(strcmp(temp_element_node->name, "frameset") == 0)
		{
			return IN_FRAMESET;
		}
		else if(strcmp(temp_element_node->name, "html") == 0)
		{
			return BEFORE_HEAD;
		}

		temp_element_node = (element_node *)temp_element_node->parent;
	}

	return IN_BODY;
}

/*------------------------------------------------------------------------------------*/
insertion_mode reset_insertion_mode_fragment(unsigned char *context_element_name)
{

	if(strcmp(context_element_name, "select") == 0)
	{
		return IN_SELECT;
	}
	else if(strcmp(context_element_name, "tr") == 0)
	{
		return IN_ROW;
	}
	else if((strcmp(context_element_name, "tbody") == 0) ||
			(strcmp(context_element_name, "thead") == 0) ||
			(strcmp(context_element_name, "tfoot") == 0))
	{
		return IN_TABLE_BODY;
	}
	else if(strcmp(context_element_name, "caption") == 0)
	{
		return IN_CAPTION;
	}
	else if(strcmp(context_element_name, "colgroup") == 0)
	{
		return IN_COLUMN_GROUP;
	}
	else if(strcmp(context_element_name, "table") == 0)
	{
		return IN_TABLE;
	}
	else if(strcmp(context_element_name, "head") == 0)
	{
		return IN_BODY;
	}
	else if(strcmp(context_element_name, "body") == 0)
	{
		return IN_BODY;
	}
	else if(strcmp(context_element_name, "frameset") == 0)
	{
		return IN_FRAMESET;
	}
	else if(strcmp(context_element_name, "html") == 0)
	{
		return BEFORE_HEAD;
	}
	else
	{
		return IN_BODY;
	}
}