// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "parser.h"
#include "util.h"
#include "format.h"
#include "foreign.h"
#include "template.h"



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
	NULL_CHARACTER_IN_TEXT,
	UNEXPECTED_END_OF_FILE,
	BOGUS_COMMENT,
	UNESCAPED_LESS_THAN,
	EMPTY_END_TAG,
	BAD_CHARACTER_FOR_ATTRIBUTE_NAME,
	MISSING_ATTRIBUTE_VALUE,
	BAD_CHARACTER_FOR_ATTRIBUTE_VALUE,
	MISSING_SPACE_AFTER_ATTRIBUTE_VALUE,
	UNEXPECTED_SOLIDUS,
	INVALID_COMMENT,
	INVALID_DOCTYPE,
	BOGUS_DOCTYPE,
	MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE,
	MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE,
	MISSING_SPACE_AFTER_PUBLIC_IDENTIFIER,
	MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE,
	UNEXPECTED_TEXT,
	UNEXPECTED_DOCTYPE,
	UNEXPECTED_START_TAG,
	UNEXPECTED_END_TAG,
	DEPRECATED_TAG,
	INVALID_END_TAG
} error_message_type;



/*----------------TOKENISER STATE TRANSITION FUNCTIONS--------------------*/
/*------------------------------------------------------------------------*/
void data_state_s1(unsigned char *ch);
void character_reference_in_data_state_s2(unsigned char *ch);
void rcdata_state_s3(unsigned char *ch);
void character_reference_in_rcdata_state_s4(unsigned char *ch);
void rawtext_state_s5(unsigned char *ch);
void script_data_state_s6(unsigned char *ch);
void plaintext_state_s7(unsigned char *ch);
void tag_open_state_s8(unsigned char *ch);
void end_tag_open_state_s9(unsigned char *ch);
void tag_name_state_s10(unsigned char *ch);
void rcdata_less_than_sign_state_s11(unsigned char *ch);
void rcdata_end_tag_open_state_s12(unsigned char *ch);
void rcdata_end_tag_name_state_s13(unsigned char *ch);
void rawtext_less_than_sign_state_s14(unsigned char *ch);
void rawtext_end_tag_open_state_s15(unsigned char *ch);
void rawtext_end_tag_name_state_s16(unsigned char *ch);
void script_data_less_than_sign_state_s17(unsigned char *ch);
void script_data_end_tag_open_state_s18(unsigned char *ch);
void script_data_end_tag_name_state_s19(unsigned char *ch);
void script_data_escape_start_state_s20(unsigned char *ch);
void script_data_escape_start_dash_state_s21(unsigned char *ch);
void script_data_escaped_state_s22(unsigned char *ch);
void script_data_escaped_dash_state_s23(unsigned char *ch);
void script_data_escaped_dash_dash_state_s24(unsigned char *ch);
void script_data_escaped_less_than_sign_state_s25(unsigned char *ch);
void script_data_escaped_end_tag_open_state_s26(unsigned char *ch);
void script_data_escaped_end_tag_name_state_s27(unsigned char *ch);
void script_data_double_escape_start_state_s28(unsigned char *ch);
void script_data_double_escaped_state_s29(unsigned char *ch);
void script_data_double_escaped_dash_state_s30(unsigned char *ch);
void script_data_double_escaped_dash_dash_state_s31(unsigned char *ch);
void script_data_double_escaped_less_than_sign_state_s32(unsigned char *ch);
void script_data_double_escape_end_state_s33(unsigned char *ch);
void before_attribute_name_state_s34(unsigned char *ch);
void attribute_name_state_s35(unsigned char *ch);
void after_attribute_name_state_s36(unsigned char *ch);
void before_attribute_value_state_s37(unsigned char *ch);
void attribute_value_double_quoted_state_s38(unsigned char *ch);
void attribute_value_single_quoted_state_s39(unsigned char *ch);
void attribute_value_unquoted_state_s40(unsigned char *ch);
void ch_reference_in_attribute_value_state_s41(unsigned char *ch);
void after_attribute_value_quoted_state_s42(unsigned char *ch);
void self_closing_start_tag_state_s43(unsigned char *ch);
void bogus_comment_state_s44(unsigned char *ch);
void markup_declaration_open_state_s45(unsigned char *ch);
void comment_start_state_s46(unsigned char *ch);
void comment_start_dash_state_s47(unsigned char *ch);
void comment_state_s48(unsigned char *ch);
void comment_end_dash_state_s49(unsigned char *ch);
void comment_end_state_s50(unsigned char *ch);
void comment_end_bang_state_s51(unsigned char *ch);
void doctype_state_s52(unsigned char *ch);
void before_doctype_name_state_s53(unsigned char *ch);
void doctype_name_state_s54(unsigned char *ch);
void after_doctype_name_state_s55(unsigned char *ch);
void after_doctype_public_keyword_state_s56(unsigned char *ch);
void before_doctype_public_identifier_state_s57(unsigned char *ch);
void doctype_public_identifier_double_quoted_state_s58(unsigned char *ch);
void doctype_public_identifier_single_quoted_state_s59(unsigned char *ch);
void after_doctype_public_identifier_state_s60(unsigned char *ch);
void between_doctype_public_and_system_identifiers_state_s61(unsigned char *ch);
void after_doctype_system_keyword_state_s62(unsigned char *ch);
void before_doctype_system_identifier_state_s63(unsigned char *ch);
void doctype_system_identifier_double_quoted_state_s64(unsigned char *ch);
void doctype_system_identifier_single_quoted_state_s65(unsigned char *ch);
void after_doctype_system_identifier_state_s66(unsigned char *ch);
void bogus_doctype_state_s67(unsigned char *ch);
void cdata_section_state_s68(unsigned char *ch);
/*------------------------------------------------------------------------*/


/*---------------------TREE CONSTRUCTION FUNCTIONS------------------------*/
/*------------------------------------------------------------------------*/
void initial_mode(const token *tk);
void before_html_mode(const token *tk);
void before_head_mode(const token *tk);
void in_head_mode(const token *tk);
void in_head_noscript_mode(const token *tk);
void after_head_mode(const token *tk);
void in_body_mode(const token *tk);
void text_mode(const token *tk);
void in_table_mode(const token *tk);
void in_table_text_mode(const token *tk);
void in_caption_mode(const token *tk);
void in_column_group_mode(const token *tk);
void in_table_body_mode(const token *tk);
void in_row_mode(const token *tk);
void in_cell_mode(const token *tk);
void in_select_mode(const token *tk);
void in_select_in_table_mode(const token *tk);
void in_template_mode(const token *tk);
void after_body_mode(const token *tk);
void in_frameset_mode(const token *tk);
void after_frameset_mode(const token *tk);
void after_after_body_mode(const token *tk);
void after_after_frameset_mode(const token *tk);

/*------------------------------------------------------------------------*/

/*the caller of this function must instantiate the global variable: doc_root
  in the full document case: "starting_node" should be doc_root
							 "starting_state" should be "DATA_STATE".
							 "starting_mode" should be "INITIAL"
  in the fragment case: "starting_node" should be "html" node.
						"staring_state" and "starting_mode" should be determined by the "context" element.
*/
void html_parse_memory_1(unsigned char *file_buffer, long buffer_length, element_node *starting_node,
						 tokenization_state starting_state, insertion_mode starting_mode);

insertion_mode reset_insertion_mode(element_stack *st);
insertion_mode reset_insertion_mode_fragment(unsigned char *context_element_name);

void parse_token_in_foreign_content(const token *tk);
void process_trailing_text(void);
void process_trailing_comment(void);
void process_token(token *tk);
void process_eof(void);
void copy_text_chunk(void);
void append_null_replacement(void);
void append_null_replacement_for_attribute_value(void);

int is_in_a_script_data_parsing_state(void);

void parse_error(error_message_type err_msg, unsigned long line_num);

void print_stack(element_stack *st);


/*--------------------TOKENISER STATE MACHINE-----------------------------*/
/*------------------------------------------------------------------------*/
void (*const tokeniser_state_functions [NUM_TOKENIZATION_STATES]) (unsigned char *ch) = {
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
void (*const tree_cons_state_functions [NUM_INSERTION_MODES]) (const token *tk) = {
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
	in_template_mode,
	after_body_mode,
	in_frameset_mode,
	after_frameset_mode,
	after_after_body_mode,
	after_after_frameset_mode
};


/*----------------------------global variables--------------------------------*/

tokenization_state current_state;
insertion_mode original_insertion_mode;
insertion_mode current_template_insertion_mode;
insertion_mode current_mode;
element_node *current_node;
element_node *context_node = NULL;
element_node *form_element_ptr = NULL, *head_element_ptr = NULL;
active_formatting_list *active_formatting_elements = NULL;
element_stack *o_e_stack = NULL;
mode_stack *m_stack = NULL;

character_consumption_type character_consumption = NOT_RECONSUME;
token_process_type token_process = NOT_REPROCESS;

token *curr_token = NULL;		//instantiate a token object and free that object after it is processed;
attribute_list *curr_token_attr_list = NULL;
unsigned char *curr_attr_name = NULL;
unsigned char *curr_attr_value = NULL;
unsigned char *temp_buf;
unsigned char *last_start_tag_name = NULL;
	
int apply_foster_parenting = 0;	//(1 - apply foster parenting, 0 - not apply foster parenting)
int previous_attribute_value_state_bookmark = 0;
//(1 - ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE, 2 - ATTIBUTE_VALUE_SINGLE_QUOTED_STATE, 3 - ATTRIBUTE_VALUE_UNQUOTED_STATE)

unsigned char *text_chunk = NULL;
unsigned char *text_buffer = NULL;

unsigned char *attr_value_chunk = NULL;

long attr_value_char_count = 0;
long text_char_count = 0;

long curr_buffer_index;
long buffer_len;
unsigned char *file_buf = NULL;

unsigned long line_number = 1;

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
		//printf("Error in read_file()\n");
		return 0;
	}


	html_parse_memory(file_buffer, buffer_length, root_ptr, doctype_ptr);
	//html_parse_memory_fragment(file_buffer, buffer_length, "script", root_ptr);
	//html_parse_memory_fragment(file_buffer, buffer_length, "math", MATHML, root_ptr);


	return 1;
}


/*----------------------------------------------------------------------------*/
void html_parse_memory_fragment(unsigned char *string_buffer, long string_length, unsigned char *context, namespace_type context_namespace, node **root_ptr)
{	
	element_node *html_node;
	tokenization_state start_state;
	insertion_mode start_mode;

	assert(string_buffer != NULL);
	assert(context != NULL);

	o_e_stack = NULL;
	m_stack = NULL;

	doc_root = create_element_node("DOC", NULL, HTML);
	html_node = create_element_node("html", NULL, HTML);
	add_child_node(doc_root, (node *)html_node);
	open_element_stack_push(&o_e_stack, html_node);

	//make context_node to point to context element
	context_node = create_element_node(context, NULL, context_namespace);

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

	//preprocessing input buffer, make LF the only character representing newlines.
	preprocess_input(string_buffer, string_length, &string_length);

	html_parse_memory_1(string_buffer, string_length, html_node, start_state, start_mode);

	//the context_node needs to be freed here.
	free_node((node *)context_node);
	context_node = NULL;


	*root_ptr = (node *)doc_root;
	
}



/*----------------------------------------------------------------------------*/
void html_parse_memory(unsigned char *file_buffer, long buffer_length, node **root_ptr, token **doctype_ptr)
{
	o_e_stack = NULL;
	m_stack = NULL;
	context_node = NULL;

	//create a document root element
	doc_root = create_element_node("DOC", NULL, HTML);


	//preprocessing input buffer, make LF the only character representing newlines.
	preprocess_input(file_buffer, buffer_length, &buffer_length);

	html_parse_memory_1(file_buffer, buffer_length, doc_root, DATA_STATE, INITIAL);
	
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
void html_parse_memory_1(unsigned char *file_buffer, long buffer_length, element_node *starting_node,
						tokenization_state starting_state, insertion_mode starting_mode)
{
	unsigned char *curr_char;
	long curr_index;

	assert(file_buffer != NULL);

	/*----------------- initialize global variable ------------------------------*/

	current_node = starting_node;
	current_state = starting_state;
	current_mode = starting_mode;

	buffer_len = buffer_length;
	file_buf = file_buffer;

	form_element_ptr = NULL;
	head_element_ptr = NULL;
	active_formatting_elements = NULL;

	character_consumption = NOT_RECONSUME;
	token_process = NOT_REPROCESS;

	curr_token = NULL;
	curr_token_attr_list = NULL;
	curr_attr_name = NULL;
	curr_attr_value = NULL;
	temp_buf = NULL;
	last_start_tag_name = NULL;
	
	apply_foster_parenting = 0;
	previous_attribute_value_state_bookmark = 0;

	text_chunk = NULL;
	text_buffer = NULL;

	attr_value_chunk = NULL;

	attr_value_char_count = 0;
	text_char_count = 0;

	line_number = 1;

	character_skip = 0;

/*----------------- end of initialization ------------------------------*/


	//create a DOCTYPE token object
	doc_type = malloc(sizeof(token));


	//if there is a UTF-8 Byte Order Mark, skip it.
	if((buffer_length >= 3) && 
			((file_buffer[0] == 0xEF) && (file_buffer[1] == 0xBB) && (file_buffer[2] == 0xBF)))
	{
		curr_index = 3;
	}
	else
	{
		curr_index = 0;
	}
	curr_buffer_index = curr_index;


	while(curr_index < buffer_length)
	{
		curr_char = &file_buffer[curr_index];


		if(*curr_char == LINE_FEED)
		{
			line_number += 1;
		}
		
		assert((current_state >= 0) && (current_state < NUM_TOKENIZATION_STATES));
		assert((current_mode >= 0) && (current_mode < NUM_INSERTION_MODES));
		
		tokeniser_state_functions[current_state](curr_char);

	
		if(character_consumption == RECONSUME)
		{
			assert((current_state >= 0) && (current_state < NUM_TOKENIZATION_STATES));
			assert((current_mode >= 0) && (current_mode < NUM_INSERTION_MODES));
			
			tokeniser_state_functions[current_state](curr_char);
				
			character_consumption = NOT_RECONSUME; 
		}
		

		curr_index += (1 + character_skip);
		character_skip = 0;

		curr_buffer_index = curr_index;
	}

	process_eof();

}




/*----------------------------------------------------------------------------*/
void data_state_s1(unsigned char *ch)
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
		//parse error, emit the current input character as a character token.
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);
		if((text_chunk != NULL) && (text_char_count > 0))	//there is text before the null character
		{
				//copy the text from file buffer to text_buffer:
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}

		text_chunk = NULL;
		text_char_count = 0;

		//The NULL character is not emitted here, as it would be ignored in in_body_mode(). 
	}
	else
	{
		if(text_char_count == 0)	//only send the first character in a chunk of text
		{
			text_chunk = ch;
			text_char_count = 1;
			process_token(create_character_token(c));
			return;
		}
		else	//increment the text_char_count for the subsequent characters in the chunk of text
		{
			text_char_count += 1;
		}
	}
	
}


/*----------------------------------------------------------------------------*/
void character_reference_in_data_state_s2(unsigned char *ch)
{
	int chars_consumed;
	int is_attribute = 0;
	int has_additional_allowed_char = 0;	//no additional allowed char
	unsigned char additional_allowed_char = '\0';	//this will be ignored
	unsigned char *char_ref;

	current_state = DATA_STATE;

	char_ref = html_character_reference(ch,is_attribute, 
				has_additional_allowed_char, additional_allowed_char, &chars_consumed, curr_buffer_index, buffer_len);

	if(char_ref == NULL)
	{
		character_consumption = RECONSUME;

		//not a character reference, so treat it as normal text.
		if(text_char_count == 0)	//no text before '&'
		{
			text_chunk = (ch - 1);
			text_char_count = 1;

			process_token(create_character_token(AMPERSAND));
			return;	
		}
		else	//there is text before '&'
		{
			text_char_count += 1;
			return;
		}
	}
	else	//copy text_chunk to text_buffer, copy char_ref to text_buffer, and start another text chunk.
	{	
		if((text_chunk != NULL) && (text_char_count > 0))	//there is text before the character reference
		{
				//copy the text from file buffer to text_buffer:
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}

		text_chunk = NULL;
		text_char_count = 0;


		text_buffer = string_n_append(text_buffer, char_ref, strlen(char_ref));

		character_skip = chars_consumed - 1;

		//only necessary if the character reference is at the beginning of the text node.
		//send a char token to the tree construction to get a text node created and added to the tree.
		process_token(create_character_token(char_ref[0]));
		free(char_ref);
		return;
	}
}

/*----------------------------------------------------------------------------*/
void rcdata_state_s3(unsigned char *ch)
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
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);
		append_null_replacement();
		return;
	}
	else
	{
		if(text_char_count == 0)	//only send the first character in a chunk of text
		{
			text_chunk = ch;
			text_char_count = 1;

			process_token(create_character_token(c));
			return;
		}
		else	//increment the text_char_count for the subsequent characters in the chunk of text
		{
			text_char_count += 1;
		}
	}

}


/*----------------------------------------------------------------------------*/
void character_reference_in_rcdata_state_s4(unsigned char *ch)
{
	int chars_consumed;
	int is_attribute = 0;
	int has_additional_allowed_char = 0;	//no additional allowed char
	unsigned char additional_allowed_char = '\0';	//this will be ignored
	unsigned char *char_ref;

	current_state = RCDATA_STATE;
	char_ref = html_character_reference(ch,is_attribute, 
				has_additional_allowed_char, additional_allowed_char, &chars_consumed, curr_buffer_index, buffer_len);

	if(char_ref == NULL)
	{
		character_consumption = RECONSUME;

		//not a character reference, so treat it as normal text.
		if(text_char_count == 0)	//no text before '&'
		{
			text_chunk = (ch - 1);
			text_char_count = 1;
			//return html_token_list_cons(create_character_token(AMPERSAND), NULL);
			process_token(create_character_token(AMPERSAND));
			return;
		}
		else	//there is text before '&'
		{
			text_char_count += 1;
			return;
		}

	}
	else	//copy text_chunk to text_buffer, copy char_ref to text_buffer, and start another text chunk.
	{	
		if((text_chunk != NULL) && (text_char_count > 0))	//there is text before the character reference
		{
			//copy the text from file buffer to text_buffer:
			text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}

		text_chunk = NULL;
		text_char_count = 0;

		text_buffer = string_n_append(text_buffer, char_ref, strlen(char_ref));

		character_skip = chars_consumed - 1;

		//only necessary if the character reference is at the beginning of the text node.
		//send a char token to the tree construction to get a text node created and added to the tree.
		process_token(create_character_token(char_ref[0]));
		free(char_ref);
		return;
	}
}


/*----------------------------------------------------------------------------*/
void rawtext_state_s5(unsigned char *ch)
{
	unsigned char c = *ch;


	if(c == LESS_THAN_SIGN)
	{
		current_state = RAWTEXT_LESS_THAN_SIGN_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);
		append_null_replacement();
		return;
	}
	else
	{
		if(text_char_count == 0)	//only send the first character in a chunk of text
		{
			text_chunk = ch;
			text_char_count = 1;
	
			process_token(create_character_token(c));
			return;
		}
		else	//increment the text_char_count for the subsequent characters in the chunk of text
		{
			text_char_count += 1;
		}
	}

}


/*----------------------------------------------------------------------------*/
void script_data_state_s6(unsigned char *ch)
{
	unsigned char c = *ch;


	if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_LESS_THAN_SIGN_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;
	}
	else
	{
		unsigned char *curr_ch = ch;
		long curr_ch_index = curr_buffer_index;
		long ch_count = 0;

		while((*curr_ch != LESS_THAN_SIGN) && (*curr_ch != NULL_CHARACTER) && (curr_ch_index < buffer_len))
		{
			curr_ch_index += 1;
			curr_ch += 1;
			ch_count += 1;

			if(*curr_ch == LINE_FEED)
			{
				line_number += 1;
			}
		}

		//the value of ch_count must be at least 1, having come out of the while loop.
		character_skip = ch_count - 1;
		process_token(create_multi_char_token(ch, ch_count));
		return;
	}
}


/*----------------------------------------------------------------------------*/
void plaintext_state_s7(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == NULL_CHARACTER)
	{
		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);
		append_null_replacement();
		return;

	}
	else
	{
		if(text_char_count == 0)	//only send the first character in a chunk of text
		{
			text_chunk = ch;
			text_char_count = 1;

			process_token(create_character_token(c));
			return;
		}
		else	//increment the text_char_count for the subsequent characters in the chunk of text
		{
			text_char_count += 1;
		}
	}

}


/*----------------------------------------------------------------------------*/
void tag_open_state_s8(unsigned char *ch)
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
		parse_error(BOGUS_COMMENT, line_number);
		current_state = BOGUS_COMMENT_STATE;
	}
	else
	{
		//parse error
		parse_error(UNESCAPED_LESS_THAN, line_number);

		current_state = DATA_STATE;
		character_consumption = RECONSUME;


		if(text_char_count == 0)	//no text before '<'
		{
			text_chunk = (ch - 1);		//mark the beginning of text at '<'
			text_char_count = 1;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else		//there is text before '<'
		{
			text_char_count += 1;		//extend the existing text before '<'
		}
	}

}

/*----------------------------------------------------------------------------*/
void end_tag_open_state_s9(unsigned char *ch)
{
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
		parse_error(EMPTY_END_TAG, line_number);
		//skip the following three characters: "</>", totally ignore them.

		//------------------------------
		if((text_chunk != NULL) && (text_char_count > 0))
		{
			//copy the text from file buffer to text_buffer:
			text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}

		//reset global variables: (to start another chunk of text)
		text_chunk = NULL;
		text_char_count = 0;
		//------------------------------

		current_state = DATA_STATE;
	}
	else
	{
		//parse error
		parse_error(BOGUS_COMMENT, line_number);

		current_state = BOGUS_COMMENT_STATE;
	}

}

/*----------------------------------------------------------------------------*/
void tag_name_state_s10(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;

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
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//Append a U+FFFD REPLACEMENT CHARACTER 
		//character to the current tag token's tag name.
		utf8_byte_sequence(0xFFFD, byte_seq);

		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_token->stt.tag_name = string_n_append(curr_token->stt.tag_name, byte_seq, strlen(byte_seq));
		}
		else if(curr_token->type == TOKEN_END_TAG)
		{
			curr_token->ett.tag_name = string_n_append(curr_token->ett.tag_name, byte_seq, strlen(byte_seq));
		}
		else
		{
			;
		}
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

}


/*----------------------------------------------------------------------------*/
void rcdata_less_than_sign_state_s11(unsigned char *ch)
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

		//not an end tag, so treat it as text.
		if(text_char_count == 0)
		{
			text_chunk = (ch - 1);		//make the beginning of text at '<'.
			text_char_count = 1;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else
		{
			text_char_count += 1;		//extend the existing text before '<'.
		}
	}

}


/*----------------------------------------------------------------------------*/
void rcdata_end_tag_open_state_s12(unsigned char *ch)
{
	unsigned char c = *ch;

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


		//invalid tag name,  so treat it as text.
		if(text_char_count == 0)	//no text before '<'
		{
			text_chunk = (ch - 2);	 //mark the beginning of the text at '<', so jump back two chars.
			text_char_count = 2;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else	//there is text before '<'
		{
			text_char_count += 2;	//extend the text by two chars.
		}
	}

}


/*----------------------------------------------------------------------------*/
void rcdata_end_tag_name_state_s13(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		//if it is an appropriate end tag, switch to the before attribute name state.
		//it is safe to do so, as there are checks in "before attribute name state"
		//to make sure attributes are not added to an end tag.
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = BEFORE_ATTRIBUTE_NAME_STATE;
		}
		else
		{
			int len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			//free memory
			free(temp_buf);
			temp_buf = NULL;

			current_state = RCDATA_STATE;
			character_consumption = RECONSUME;

			//not an appropriate RCDATA end tag, so treat it as text.
			if(text_char_count == 0)	//no text before '<'
			{
				text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
				text_char_count = len + 2;

				process_token(create_character_token(LESS_THAN_SIGN));
				return;
			}
			else	//there is text before '<'
			{
				text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
			}
		}
	}
	else if(c == SOLIDUS)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//it is safe to switch to self_closing_start_tag_state.
			//there are checks to ensure attributes are NOT added and self_closing_flag is NOT set for end tags.
			free(temp_buf);
			temp_buf = NULL;
			current_state = SELF_CLOSING_START_TAG_STATE;
		}
		else
		{
			int len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			//free memory
			free(temp_buf);
			temp_buf = NULL;

			current_state = RCDATA_STATE;
			character_consumption = RECONSUME;

			//invalid RCDATA end tag, so treat it as text.
			if(text_char_count == 0)	//no text before '<'
			{
				text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
				text_char_count = len + 2;

				process_token(create_character_token(LESS_THAN_SIGN));
				return;
			}
			else	//there is text before '<'
			{
				text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
			}
		
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;

			process_token(curr_token);
			curr_token = NULL;
			return;
		}
		else
		{
			int len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			//free memory
			free(temp_buf);
			temp_buf = NULL;

			current_state = RCDATA_STATE;
			character_consumption = RECONSUME;

			//not an appropriate RCDATA end tag, so treat it as text.
			if(text_char_count == 0)	//no text before '<'
			{
				text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
				text_char_count = len + 2;

				process_token(create_character_token(LESS_THAN_SIGN));
				return;
			}
			else	//there is text before '<'
			{
				text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
			}

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
		int len;
			
		if(temp_buf != NULL)
		{
			len = strlen(temp_buf);
		}
		else
		{
			len = 0;
		}

		//free memory
		free(temp_buf);
		temp_buf = NULL;


		current_state = RCDATA_STATE;
		character_consumption = RECONSUME;

		//invalid RCDATA end tag, so treat it as text.
		if(text_char_count == 0)	//no text before '<'
		{
			text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
			text_char_count = len + 2;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else	//there is text before '<'
		{
			text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
		}


	}

}


/*----------------------------------------------------------------------------*/
void rawtext_less_than_sign_state_s14(unsigned char *ch)
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


		//not an end tag, so treat it as text.
		if(text_char_count == 0)
		{
			text_chunk = (ch - 1);		//make the beginning of text at '<'.
			text_char_count = 1;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else
		{
			text_char_count += 1;		//extend the existing text before '<'.
		}

	}

}


/*----------------------------------------------------------------------------*/
void rawtext_end_tag_open_state_s15(unsigned char *ch)
{
	unsigned char c = *ch;


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
	
		current_state = RAWTEXT_STATE;
		character_consumption = RECONSUME;


		//invalid tag name,  so treat it as text.
		if(text_char_count == 0)	//no text before '<'
		{
			text_chunk = (ch - 2);	 //mark the beginning of the text at '<', so jump back two chars.
			text_char_count = 2;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else	//there is text before '<'
		{
			text_char_count += 2;	//extend the text by two chars.
		}
	}

}


/*----------------------------------------------------------------------------*/
void rawtext_end_tag_name_state_s16(unsigned char *ch)
{
	unsigned char c = *ch;


	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//if it is an appropriate end tag, switch to the before attribute name state.
			//it is safe to do so, as there are checks in "before attribute name state"
			//to make sure attributes are not added to an end tag.

			free(temp_buf);
			temp_buf = NULL;
			current_state = BEFORE_ATTRIBUTE_NAME_STATE;
		}
		else
		{
			/*----------------------------*/
			int len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			//free memory
			free(temp_buf);
			temp_buf = NULL;

			current_state = RAWTEXT_STATE;
			character_consumption = RECONSUME;


			if(text_char_count == 0)	//no text before '<'
			{
				text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
				text_char_count = len + 2;

				process_token(create_character_token(LESS_THAN_SIGN));
				return;
			}
			else	//there is text before '<'
			{
				text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
			}
			/*----------------------------*/

		}
	}
	else if(c == SOLIDUS)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//it is safe to switch to self_closing_start_tag_state.
			//there are checks to ensure attributes are NOT added to and self_closing_flag is NOT set for end tags.
			free(temp_buf);
			temp_buf = NULL;
			current_state = SELF_CLOSING_START_TAG_STATE;
		}
		else
		{
			/*----------------------------*/
			int len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			//free memory
			free(temp_buf);
			temp_buf = NULL;

			current_state = RAWTEXT_STATE;
			character_consumption = RECONSUME;


			if(text_char_count == 0)	//no text before '<'
			{
				text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
				text_char_count = len + 2;

				process_token(create_character_token(LESS_THAN_SIGN));	
				return;
			}
			else	//there is text before '<'
			{
				text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
			}
			/*----------------------------*/
		
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;

			process_token(curr_token);
			curr_token = NULL;
			return;
		}
		else
		{
			//not an appropriate RAWTEXT end tag, so treat it as text.
			/*----------------------------*/
			int len;
			
			if(temp_buf != NULL)
			{
				len = strlen(temp_buf);
			}
			else
			{
				len = 0;
			}

			//free memory
			free(temp_buf);
			temp_buf = NULL;

			current_state = RAWTEXT_STATE;
			character_consumption = RECONSUME;


			if(text_char_count == 0)	//no text before '<'
			{
				text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
				text_char_count = len + 2;

				process_token(create_character_token(LESS_THAN_SIGN));
				return;
			}
			else	//there is text before '<'
			{
				text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
			}
			/*----------------------------*/
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
		/*----------------------------*/
		int len;
			
		if(temp_buf != NULL)
		{
			len = strlen(temp_buf);
		}
		else
		{
			len = 0;
		}

		//free memory
		free(temp_buf);
		temp_buf = NULL;

		current_state = RAWTEXT_STATE;
		character_consumption = RECONSUME;

		//invalid RAWTEXT end tag, so treat it as text.
		if(text_char_count == 0)	//no text before '<'
		{
			text_chunk = (ch - 2 - len);		//mark the beginning of text at '<'.
			text_char_count = len + 2;

			process_token(create_character_token(LESS_THAN_SIGN));
			return;
		}
		else	//there is text before '<'
		{
			text_char_count += (len + 2);		//extend the existing text before '<' by (len + 2) chars.
		}
		/*----------------------------*/
	}

}


/*----------------------------------------------------------------------------*/
void script_data_less_than_sign_state_s17(unsigned char *ch)
{
	unsigned char c = *ch;

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

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(EXCLAMATION_MARK));
		return;

	}
	else
	{
		character_consumption = RECONSUME;
		current_state = SCRIPT_DATA_STATE;
	
		process_token(create_character_token(LESS_THAN_SIGN));
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_end_tag_open_state_s18(unsigned char *ch)
{
	unsigned char c = *ch;

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

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(SOLIDUS));
		return;

	}

}


/*----------------------------------------------------------------------------*/
void script_data_end_tag_name_state_s19(unsigned char *ch)
{
	unsigned char c = *ch;


	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//if it is an appropriate end tag, switch to the before attribute name state.
			//it is safe to do so, as there are checks in "before attribute name state"
			//to make sure attributes are not added to an end tag.

			free(temp_buf);
			temp_buf = NULL;
			current_state = BEFORE_ATTRIBUTE_NAME_STATE;
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

			process_token(create_character_token(LESS_THAN_SIGN));
			process_token(create_character_token(SOLIDUS));

		
			for(i = 0; i < len; i++)
			{
				process_token(create_character_token(temp_buf[i]));
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return;
		}
	}
	else if(c == SOLIDUS)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//it is safe to switch to self_closing_start_tag_state.
			//there are checks to ensure attributes are NOT added to and self_closing_flag is NOT set for end tags.
			free(temp_buf);
			temp_buf = NULL;
			current_state = SELF_CLOSING_START_TAG_STATE;
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

			process_token(create_character_token(LESS_THAN_SIGN));
			process_token(create_character_token(SOLIDUS));

		
			for(i = 0; i < len; i++)
			{
				process_token(create_character_token(temp_buf[i]));
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return;
		}
	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;
			
			process_token(curr_token);
			curr_token = NULL;
			return;
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

			process_token(create_character_token(LESS_THAN_SIGN));
			process_token(create_character_token(SOLIDUS));

		
			for(i = 0; i < len; i++)
			{
				process_token(create_character_token(temp_buf[i]));
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return;
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

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(SOLIDUS));

		
		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(temp_buf[i]));
		}

		free(temp_buf);
		temp_buf = NULL;
		
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escape_start_state_s20(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPE_START_DASH_STATE;

		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else
	{
		current_state = SCRIPT_DATA_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escape_start_dash_state_s21(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPED_DASH_DASH_STATE;

		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else
	{
		current_state = SCRIPT_DATA_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escaped_state_s22(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPED_DASH_STATE;

		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		//return replacement_for_null_char();
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;

	}
	else
	{
		process_token(create_character_token(c));
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escaped_dash_state_s23(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_ESCAPED_DASH_DASH_STATE;

		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		current_state = SCRIPT_DATA_ESCAPED_STATE;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;

	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;

		process_token(create_character_token(c));
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escaped_dash_dash_state_s24(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == HYPHEN_MINUS)
	{
		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_STATE;

		process_token(create_character_token(GREATER_THAN_SIGN));
		return;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		current_state = SCRIPT_DATA_ESCAPED_STATE;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;

	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;

		process_token(create_character_token(c));
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escaped_less_than_sign_state_s25(unsigned char *ch)
{	
	unsigned char c = *ch;


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

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(*ch));
		return;

	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		temp_buf = string_append(temp_buf, c);

		current_state = SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE;

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(*ch));
		return;

	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		character_consumption = RECONSUME;
		
		process_token(create_character_token(LESS_THAN_SIGN));
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_escaped_end_tag_open_state_s26(unsigned char *ch)
{
	unsigned char c = *ch;


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

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(SOLIDUS));
		return;

	}

}


/*----------------------------------------------------------------------------*/
void script_data_escaped_end_tag_name_state_s27(unsigned char *ch)
{
	unsigned char c = *ch;


	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{

		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//if it is an appropriate end tag, switch to the before attribute name state.
			//it is safe to do so, as there are checks in "before attribute name state"
			//to make sure attributes are not added to an end tag.

			free(temp_buf);
			temp_buf = NULL;
			current_state = BEFORE_ATTRIBUTE_NAME_STATE;
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

			process_token(create_character_token(LESS_THAN_SIGN));
			process_token(create_character_token(SOLIDUS));

		
			for(i = 0; i < len; i++)
			{
				process_token(create_character_token(temp_buf[i]));
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return;
		}
	}
	else if(c == SOLIDUS)
	{

		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			//it is safe to switch to self_closing_start_tag_state.
			//there are checks to ensure attributes are NOT added to and self_closing_flag is NOT set for end tags.
			free(temp_buf);
			temp_buf = NULL;
			current_state = SELF_CLOSING_START_TAG_STATE;
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

			process_token(create_character_token(LESS_THAN_SIGN));
			process_token(create_character_token(SOLIDUS));


			for(i = 0; i < len; i++)
			{
				process_token(create_character_token(temp_buf[i]));
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return;
		}

	}
	else if(c == GREATER_THAN_SIGN)
	{
		if((last_start_tag_name != NULL) && (strcmp(curr_token->ett.tag_name, last_start_tag_name) == 0))
		{
			free(temp_buf);
			temp_buf = NULL;
			current_state = DATA_STATE;

			process_token(curr_token);
			curr_token = NULL;
			return;
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

			process_token(create_character_token(LESS_THAN_SIGN));
			process_token(create_character_token(SOLIDUS));

		
			for(i = 0; i < len; i++)
			{
				process_token(create_character_token(temp_buf[i]));
			}

			free(temp_buf);
			temp_buf = NULL;
		
			return;
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

		process_token(create_character_token(LESS_THAN_SIGN));
		process_token(create_character_token(SOLIDUS));

		
		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(temp_buf[i]));
		}

		free(temp_buf);
		temp_buf = NULL;
		
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_double_escape_start_state_s28(unsigned char *ch)
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
		
		process_token(create_character_token(c));
		return;

	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		temp_buf = string_append(temp_buf, c);

		process_token(create_character_token(*ch));
		return;

	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		temp_buf = string_append(temp_buf, *ch);

		process_token(create_character_token(*ch));
		return;

	}
	else
	{
		current_state = SCRIPT_DATA_ESCAPED_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_double_escaped_state_s29(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE;
		
		process_token(create_character_token(HYPHEN_MINUS));
		return;

	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;

		process_token(create_character_token(LESS_THAN_SIGN));
		return;

	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;

	}
	else
	{
		process_token(create_character_token(*ch));
		return;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_double_escaped_dash_state_s30(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == HYPHEN_MINUS)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE;

		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;

		process_token(create_character_token(LESS_THAN_SIGN));
		return;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;

		process_token(create_character_token(*ch));
		return;
	}	

}


/*----------------------------------------------------------------------------*/
void script_data_double_escaped_dash_dash_state_s31(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == HYPHEN_MINUS)
	{
		process_token(create_character_token(HYPHEN_MINUS));
		return;
	}
	else if(c == LESS_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;

		process_token(create_character_token(LESS_THAN_SIGN));
		return;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		current_state = SCRIPT_DATA_STATE;

		process_token(create_character_token(GREATER_THAN_SIGN));
		return;
	}
	else if(c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];
		int i, len;

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;

		utf8_byte_sequence(0xFFFD, byte_seq);
		len = strlen(byte_seq);

		for(i = 0; i < len; i++)
		{
			process_token(create_character_token(byte_seq[i]));
		}
		return;
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;

		process_token(create_character_token(*ch));
		return;
	}	
	
}


/*----------------------------------------------------------------------------*/
void script_data_double_escaped_less_than_sign_state_s32(unsigned char *ch)
{
	unsigned char c = *ch;
	
	if(c == SOLIDUS)
	{
		free(temp_buf);
		temp_buf = NULL;
		temp_buf = string_append(NULL, '\0');
		current_state = SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE;

		process_token(create_character_token(SOLIDUS));
		return;
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void script_data_double_escape_end_state_s33(unsigned char *ch)
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
		
		process_token(create_character_token(*ch));
		return;
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		temp_buf = string_append(temp_buf, c);

		process_token(create_character_token(*ch));
		return;
	}
	else if((c >= SMALL_A) && (c <= SMALL_Z))
	{
		temp_buf = string_append(temp_buf, *ch);

		process_token(create_character_token(*ch));
		return;
	}
	else
	{
		current_state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void before_attribute_name_state_s34(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;
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
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//add the attribute only if the current token is a start tag token
		if(curr_token->type == TOKEN_START_TAG)
		{
			unsigned char byte_seq[5];

			utf8_byte_sequence(0xFFFD, byte_seq);

			curr_attr_name = string_n_append(NULL, byte_seq, strlen(byte_seq));
			curr_attr_value = string_append(NULL, '\0');

			current_state = ATTRIBUTE_NAME_STATE;
			//if it ever gets to the ATTRIBUTE_NAME_STATE, the current token
			//must be a start tag token.
		}
	}	
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN) || (c == EQUALS_SIGN))
	{
		//parse error
		parse_error(BAD_CHARACTER_FOR_ATTRIBUTE_NAME, line_number);

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

}


/*----------------------------------------------------------------------------*/
void attribute_name_state_s35(unsigned char *ch)
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
																  DEFAULT,
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

		process_token(curr_token);
		curr_token = NULL;
		return;
	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		curr_attr_name = string_append(curr_attr_name, c);
	}
	else if(c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		utf8_byte_sequence(0xFFFD, byte_seq);

		curr_attr_name = string_n_append(curr_attr_name, byte_seq, strlen(byte_seq));
		
	}
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN))
	{
		//parse error
		parse_error(BAD_CHARACTER_FOR_ATTRIBUTE_NAME, line_number);

		curr_attr_name = string_append(curr_attr_name, c);
	}
	else
	{
		curr_attr_name = string_append(curr_attr_name, c);
	}

}


/*----------------------------------------------------------------------------*/
void after_attribute_name_state_s36(unsigned char *ch)
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
																  DEFAULT,
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

		process_token(curr_token);
		curr_token = NULL;
		return;

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
																  DEFAULT,
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
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;

		//start a new attribute:
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_attr_name = string_n_append(NULL, byte_seq, strlen(byte_seq));
		curr_attr_value = string_append(NULL, '\0');

		current_state = ATTRIBUTE_NAME_STATE;
	}
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN))
	{
		//parse error
		parse_error(BAD_CHARACTER_FOR_ATTRIBUTE_NAME, line_number);

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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
	else
	{
		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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

}


/*----------------------------------------------------------------------------*/
void before_attribute_value_state_s37(unsigned char *ch)
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
		unsigned char byte_seq[5];

		//parse_error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//Append a U+FFFD REPLACEMENT CHARACTER to the current attribute's value.
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_attr_value = string_n_append(curr_attr_value, byte_seq, strlen(byte_seq));
		
		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_ATTRIBUTE_VALUE, line_number);

		current_state = DATA_STATE;

		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;


		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else if((c == LESS_THAN_SIGN) || (c == EQUALS_SIGN) || (c == GRAVE_ACCENT))
	{
		//parse error
		parse_error(BAD_CHARACTER_FOR_ATTRIBUTE_VALUE, line_number);
		//curr_attr_value = string_append(curr_attr_value, c);

		//instead of appending current char to the curr_attr_value,
		//mark the beginnig of the attribute value in the file buffer,
		//and start counting the number of characters.
		attr_value_chunk = ch;
		attr_value_char_count = 1;
	
		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}
	else
	{
		//curr_attr_value = string_append(curr_attr_value, c);
		
		//instead of appending current char to the curr_attr_value,
		//mark the beginnig of the attribute value in the file buffer,
		//and start counting the number of characters.
		attr_value_chunk = ch;
		attr_value_char_count = 1;

		current_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void attribute_value_double_quoted_state_s38(unsigned char *ch)
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
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		append_null_replacement_for_attribute_value();
	}
	else
	{
		//curr_attr_value = string_append(curr_attr_value, c);

		//instead of appending current char to the curr_attr_value,
		//mark the beginnig of the attribute value in the file buffer, AND/OR
		//start counting the number of characters.
		if(attr_value_char_count == 0)
		{
			attr_value_chunk = ch;
			attr_value_char_count = 1;
		}
		else
		{
			attr_value_char_count += 1;
		}
	}

}


/*----------------------------------------------------------------------------*/
void attribute_value_single_quoted_state_s39(unsigned char *ch)
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
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		append_null_replacement_for_attribute_value();
	}
	else
	{
		//curr_attr_value = string_append(curr_attr_value, c);

		//instead of appending current char to the curr_attr_value,
		//mark the beginnig of the attribute value in the file buffer, AND/OR
		//start counting the number of characters.
		if(attr_value_char_count == 0)
		{
			attr_value_chunk = ch;
			attr_value_char_count = 1;
		}
		else
		{
			attr_value_char_count += 1;
		}
	}

}


/*----------------------------------------------------------------------------*/
void attribute_value_unquoted_state_s40(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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
		
		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;

		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else if(c == NULL_CHARACTER)
	{
		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		append_null_replacement_for_attribute_value();

	}
	else if((c == QUOTATION_MARK) || (c == APOSTROPHE) || (c == LESS_THAN_SIGN) 
				|| (c == EQUALS_SIGN) || (c == GRAVE_ACCENT))
	{	
		//parse error
		parse_error(BAD_CHARACTER_FOR_ATTRIBUTE_VALUE, line_number);

		//curr_attr_value = string_append(curr_attr_value, c);

		//instead of appending current char to the curr_attr_value,
		//mark the beginnig of the attribute value in the file buffer, AND/OR
		//start counting the number of characters.
		if(attr_value_char_count == 0)
		{
			attr_value_chunk = ch;
			attr_value_char_count = 1;
		}
		else
		{
			attr_value_char_count += 1;
		}

	}
	else
	{
		//curr_attr_value = string_append(curr_attr_value, c);

		//instead of appending current char to the curr_attr_value,
		//mark the beginnig of the attribute value in the file buffer, AND/OR
		//start counting the number of characters.
		if(attr_value_char_count == 0)
		{
			attr_value_chunk = ch;
			attr_value_char_count = 1;
		}
		else
		{
			attr_value_char_count += 1;
		}
	}

}


/*----------------------------------------------------------------------------*/
void ch_reference_in_attribute_value_state_s41(unsigned char *ch)
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

		//curr_attr_value = string_append(curr_attr_value, AMPERSAND);

		//not a character reference, so treat it as normal text.
		if(attr_value_char_count == 0)	//no text before '&'
		{
			attr_value_chunk = (ch - 1);	//mark the beginning of attribute value at '&'.
			attr_value_char_count = 1;				//the current char ch will be reconsumed, so do not count it yet.
		}
		else
		{
			attr_value_char_count += 1;		//there must be text before '&', just increment it to include '&'.
		}
	}
	else
	{	
		
		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;


		//append char_ref to curr_attr_value:
		curr_attr_value = string_n_append(curr_attr_value, char_ref, strlen(char_ref));


		free(char_ref);
		character_skip = chars_consumed - 1;
	}

}


/*----------------------------------------------------------------------------*/
void after_attribute_value_quoted_state_s42(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;


		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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

		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;


		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
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

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		//parse error
		parse_error(MISSING_SPACE_AFTER_ATTRIBUTE_VALUE, line_number);

		//copy attr_value_chunk from file buffer and append it to curr_attr_value:
		if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
		{
			curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
		}
		attr_value_chunk = NULL;
		attr_value_char_count = 0;


		//if the attribute name is not already in the attribute list, 
		//then add the attribute(name, value pair) to the attribute list.
		if((curr_attr_name != NULL) && (attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
		{
			curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																  curr_attr_value, 
																  DEFAULT,
																  curr_token->stt.attributes);
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;


		current_state = BEFORE_ATTRIBUTE_NAME_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void self_closing_start_tag_state_s43(unsigned char *ch)
{
	unsigned char c = *ch; 

	if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		if(curr_token->type == TOKEN_START_TAG)
		{
			curr_token->stt.self_closing_flag = SET;

			//copy attr_value_chunk from file buffer and append it to curr_attr_value:
			if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
			{
				curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
			}
			attr_value_chunk = NULL;
			attr_value_char_count = 0;
		
			//if the attribute name is not already in the attribute list, 
			//then add the attribute(name, value pair) to the attribute list.
			if((curr_attr_name != NULL) && 
					(attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
			{
				curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																	  curr_attr_value, 
																	  DEFAULT,
																	  curr_token->stt.attributes);
			}
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;
	
		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		//parse error
		parse_error(UNEXPECTED_SOLIDUS, line_number);

		if(curr_token->type == TOKEN_START_TAG)
		{

			//copy attr_value_chunk from file buffer and append it to curr_attr_value:
			if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
			{
				curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
			}
			attr_value_chunk = NULL;
			attr_value_char_count = 0;
		
			//if the attribute name is not already in the attribute list, 
			//then add the attribute(name, value pair) to the attribute list.
			if((curr_attr_name != NULL) && 
					(attribute_name_in_list(curr_attr_name, curr_token->stt.attributes) == 0))
			{
				curr_token->stt.attributes = html_attribute_list_cons(curr_attr_name, 
																	  curr_attr_value, 
																	  DEFAULT,
																	  curr_token->stt.attributes);
			}
		}
		free(curr_attr_name);
		free(curr_attr_value);
		curr_attr_name = NULL;
		curr_attr_value = NULL;


		current_state = BEFORE_ATTRIBUTE_NAME_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void bogus_comment_state_s44(unsigned char *ch)
{
	unsigned char c = *ch;
	unsigned char byte_seq[5];
	long curr_buf_index = curr_buffer_index;
	int len, i, j;


	utf8_byte_sequence(0xFFFD, byte_seq);
	len = strlen(byte_seq);

	curr_token = create_comment_token();

	//include the character before the current one in the comment
	if(*(ch - 1) == NULL_CHARACTER)		
	{
		for(i = 0; i < len; i++)
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
		}
	}
	else if(*(ch - 1) == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;
		character_consumption = RECONSUME;
	
		process_token(curr_token);
		curr_token = NULL;
		return;
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

		if(*(ch + j) == NULL_CHARACTER)
		{
			for(i = 0; i < len; i++)
			{
				curr_token->cmt.comment = string_append(curr_token->cmt.comment, byte_seq[i]);
			}
		}
		else
		{
			curr_token->cmt.comment = string_append(curr_token->cmt.comment, *(ch + j));

			if(*(ch + j) == LINE_FEED)
			{
				line_number += 1;
			}
		}

		j += 1;
	}

	character_skip = j;

	current_state = DATA_STATE;

	
	process_token(curr_token);
	curr_token = NULL;
	return;
}


/*----------------------------------------------------------------------------*/
void markup_declaration_open_state_s45(unsigned char *ch)
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
	else if((current_node->name_space != HTML) &&
			(((curr_buffer_index + 6) <= (buffer_len - 1)) && 
			((*ch == '[') && (*(ch + 1) == 'C') && (*(ch + 2) == 'D') && (*(ch + 3) == 'A') && 
			 (*(ch + 4) == 'T') && (*(ch + 5) == 'A') && (*(ch + 6) == '['))))
	{
		character_skip = 6;
		current_state = CDATA_SECTION_STATE;
	}
	else
	{
		//parse error
		parse_error(BOGUS_COMMENT, line_number);

		current_state = BOGUS_COMMENT_STATE;	
	}

}


/*----------------------------------------------------------------------------*/
void comment_start_state_s46(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_START_DASH_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//append replacement character U+FFFD to the comment data
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->cmt.comment = string_n_append(curr_token->cmt.comment, byte_seq, strlen(byte_seq));

		current_state = COMMENT_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(INVALID_COMMENT, line_number);

		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
		current_state = COMMENT_STATE;
	}


}


/*----------------------------------------------------------------------------*/
void comment_start_dash_state_s47(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_END_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
	
		//append replacement character U+FFFD to the comment data
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->cmt.comment = string_n_append(curr_token->cmt.comment, byte_seq, strlen(byte_seq));
		
		current_state = COMMENT_STATE;
	}
	else if( c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(INVALID_COMMENT, line_number);

		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);

		current_state = COMMENT_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void comment_state_s48(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_END_DASH_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//append replacement character U+FFFD to the comment data
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->cmt.comment = string_n_append(curr_token->cmt.comment, byte_seq, strlen(byte_seq));

	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
	}

}


/*----------------------------------------------------------------------------*/
void comment_end_dash_state_s49(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == HYPHEN_MINUS)
	{
		current_state = COMMENT_END_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
		//append replacement character U+FFFD to the comment data
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->cmt.comment = string_n_append(curr_token->cmt.comment, byte_seq, strlen(byte_seq));
		
		current_state = COMMENT_STATE;
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
		
		current_state = COMMENT_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void comment_end_state_s50(unsigned char *ch)
{
	unsigned char c = *ch;

	if( c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
		//append replacement character U+FFFD to the comment data
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->cmt.comment = string_n_append(curr_token->cmt.comment, byte_seq, strlen(byte_seq));
		
		current_state = COMMENT_STATE;
	}
	else if( c == EXCLAMATION_MARK)
	{
		//parse error
		parse_error(INVALID_COMMENT, line_number);

		current_state = COMMENT_END_BANG_STATE;
	}
	else if( c == HYPHEN_MINUS)
	{
		//parse error
		parse_error(INVALID_COMMENT, line_number);

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		
	}
	else
	{
		//parse error
		parse_error(INVALID_COMMENT, line_number);

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);
		
		current_state = COMMENT_STATE;
	}
	
}


/*----------------------------------------------------------------------------*/
void comment_end_bang_state_s51(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, EXCLAMATION_MARK);
		
		//append replacement character U+FFFD to the comment data
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->cmt.comment = string_n_append(curr_token->cmt.comment, byte_seq, strlen(byte_seq));
		
		current_state = COMMENT_STATE;
	}
	else
	{
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, HYPHEN_MINUS);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, EXCLAMATION_MARK);
		curr_token->cmt.comment = string_append(curr_token->cmt.comment, c);

		current_state = COMMENT_STATE;
	}


}


/*----------------------------------------------------------------------------*/
void doctype_state_s52(unsigned char *ch)
{
	unsigned char c = *ch;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) ||(c == SPACE))
	{
		current_state = BEFORE_DOCTYPE_NAME_STATE;
	}
	else
	{
		//parse error
		parse_error(INVALID_DOCTYPE, line_number);

		current_state = BEFORE_DOCTYPE_NAME_STATE;
		character_consumption = RECONSUME;
	}

}


/*----------------------------------------------------------------------------*/
void before_doctype_name_state_s53(unsigned char *ch)
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
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		curr_token = create_doctype_token('\0');

		//append replacement character U+FFFD to DOCTYPE name
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->dt.doctype_name = string_n_append(curr_token->dt.doctype_name, byte_seq, strlen(byte_seq));

		current_state = DOCTYPE_NAME_STATE;
	}
	else if( c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(INVALID_DOCTYPE, line_number);

		curr_token = create_doctype_token('\0');
		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		curr_token = create_doctype_token(c);
		current_state = DOCTYPE_NAME_STATE;
	}


}


/*----------------------------------------------------------------------------*/
void doctype_name_state_s54(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else if((c >= CAPITAL_A) && (c <= CAPITAL_Z))
	{
		c = c + CAPITAL_TO_SMALL;
		
		curr_token->dt.doctype_name = string_append(curr_token->dt.doctype_name, c);
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//append replacement character U+FFFD to DOCTYPE name
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->dt.doctype_name = string_n_append(curr_token->dt.doctype_name, byte_seq, strlen(byte_seq));

	}
	else
	{
		curr_token->dt.doctype_name = string_append(curr_token->dt.doctype_name, c);
	}

}


/*----------------------------------------------------------------------------*/
void after_doctype_name_state_s55(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;

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
			parse_error(BOGUS_DOCTYPE, line_number);

			curr_token->dt.force_quirks_flag = ON;
			current_state = BOGUS_DOCTYPE_STATE;
		}
	}

}

/*----------------------------------------------------------------------------*/
void after_doctype_public_keyword_state_s56(unsigned char *ch)
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
		parse_error(MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE, line_number);

		curr_token->dt.doctype_public_identifier = string_append(NULL, '\0');
		current_state = DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{
		//parse error
		parse_error(MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE, line_number);

		curr_token->dt.doctype_public_identifier = string_append(NULL, '\0');
		current_state = DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{	
		//parse error
		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void before_doctype_public_identifier_state_s57(unsigned char *ch)
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
		parse_error(MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		
		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		//parse error
		parse_error(BOGUS_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}
	
}


/*----------------------------------------------------------------------------*/
void doctype_public_identifier_double_quoted_state_s58(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == QUOTATION_MARK)
	{
		current_state = AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->dt.doctype_public_identifier = string_n_append(curr_token->dt.doctype_public_identifier, byte_seq, strlen(byte_seq));

	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		
		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{	
		curr_token->dt.doctype_public_identifier = string_append(curr_token->dt.doctype_public_identifier, c);
	}

}


/*----------------------------------------------------------------------------*/
void doctype_public_identifier_single_quoted_state_s59(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == APOSTROPHE)
	{
		current_state = AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE;
	}
	else if( c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//append the replacement character U+FFFD to the public identifier.
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->dt.doctype_public_identifier = string_n_append(curr_token->dt.doctype_public_identifier, byte_seq, strlen(byte_seq));

	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;

		current_state = DATA_STATE;
		
		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{	
		curr_token->dt.doctype_public_identifier = string_append(curr_token->dt.doctype_public_identifier, c);
	}

}


/*----------------------------------------------------------------------------*/
void after_doctype_public_identifier_state_s60(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else if(c == QUOTATION_MARK)
	{	
		//parse error
		parse_error(MISSING_SPACE_AFTER_PUBLIC_IDENTIFIER, line_number);

		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{	
		//parse error
		parse_error(MISSING_SPACE_AFTER_PUBLIC_IDENTIFIER, line_number);

		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else
	{
		//parse error
		parse_error(BOGUS_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void between_doctype_public_and_system_identifiers_state_s61(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;

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
	else
	{
		//parse error
		parse_error(BOGUS_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void after_doctype_system_keyword_state_s62(unsigned char *ch)
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
		parse_error(MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE, line_number);

		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE;
	}
	else if(c == APOSTROPHE)
	{	
		//parse error
		parse_error(MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE, line_number);

		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, '\0');
		current_state = DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE;
	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		//parse error
		parse_error(BOGUS_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void before_doctype_system_identifier_state_s63(unsigned char *ch)
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
		parse_error(MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		//parse error
		parse_error(BOGUS_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = BOGUS_DOCTYPE_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void doctype_system_identifier_double_quoted_state_s64(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == QUOTATION_MARK)
	{
		current_state = AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//append the replacement character U+FFFD to the system identifier
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->dt.doctype_system_identifier = string_n_append(curr_token->dt.doctype_system_identifier, byte_seq, strlen(byte_seq));

	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, c);
	}

}


/*----------------------------------------------------------------------------*/
void doctype_system_identifier_single_quoted_state_s65(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == APOSTROPHE)
	{
		current_state = AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE;
	}
	else if(c == NULL_CHARACTER)
	{
		unsigned char byte_seq[5];

		//parse error
		parse_error(NULL_CHARACTER_IN_TEXT, line_number);

		//append the replacement character U+FFFD to the system identifier
		utf8_byte_sequence(0xFFFD, byte_seq);
		curr_token->dt.doctype_system_identifier = string_n_append(curr_token->dt.doctype_system_identifier, byte_seq, strlen(byte_seq));

	}
	else if(c == GREATER_THAN_SIGN)
	{
		//parse error
		parse_error(MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE, line_number);

		curr_token->dt.force_quirks_flag = ON;
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		curr_token->dt.doctype_system_identifier = string_append(curr_token->dt.doctype_system_identifier, c);
	}

}


/*----------------------------------------------------------------------------*/
void after_doctype_system_identifier_state_s66(unsigned char *ch)
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

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		//parse error
		parse_error(BOGUS_DOCTYPE, line_number);

		current_state = BOGUS_DOCTYPE_STATE;
	}

}


/*----------------------------------------------------------------------------*/
void bogus_doctype_state_s67(unsigned char *ch)
{
	unsigned char c = *ch;

	if(c == GREATER_THAN_SIGN)
	{
		current_state = DATA_STATE;

		process_token(curr_token);
		curr_token = NULL;
		return;

	}
	else
	{
		;	//ignore the character
	}

}


/*----------------------------------------------------------------------------*/
void cdata_section_state_s68(unsigned char *ch)
{
	long i;
	long temp_index = curr_buffer_index;
	unsigned char *chunk_start;
	unsigned char byte_seq[5];

	//UTF-8 sequence for replacement char(0xFFFD)
	utf8_byte_sequence(0xFFFD, byte_seq);

	current_state = DATA_STATE;


	//if there is text before "<![CDATA[", copy it to the text_buffer:
	if((text_chunk != NULL) && (text_char_count > 0))
	{
		text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
	}
	text_chunk = NULL;
	text_char_count = 0;


	chunk_start = ch;
	i = 0;
	while((temp_index + 2) <= (buffer_len - 1))		//loop to a point where there are less than three chars left.
	{
		//increment line_number, a global variable.
		if(*(chunk_start + i) == LINE_FEED)
		{
			line_number += 1;
		}


		//there is a CDATA ending sequence "]]>"
		if((*(chunk_start + i) == ']') && (*(chunk_start + i + 1) == ']') && (*(chunk_start + i + 2) == '>'))
		{
			if(i > 0)
			{
				//copy i characters to text_buffer
				text_buffer = string_n_append(text_buffer, chunk_start, i);
				
				//get a text node created and added to the tree.
				process_token(create_character_token(*chunk_start));
			}

			//skip total number of characters consumed, less one.
			character_skip = temp_index - curr_buffer_index + 2;

			return;
		}

		if(*(chunk_start + i) == NULL_CHARACTER)
		{
			//parse error
			parse_error(NULL_CHARACTER_IN_TEXT, line_number);

			//copy the chunk up to the NULL char
			text_buffer = string_n_append(text_buffer, chunk_start, i);

			//append the replacement character
			text_buffer = string_n_append(text_buffer, byte_seq, strlen(byte_seq));

			//get a text node created and added to the tree.
			process_token(create_character_token(*chunk_start));
			
			chunk_start += i + 1;

			i = 0;
		}
		else
		{
			i += 1;
		}

		temp_index += 1;
	}

	//we have not found the ending sequence "]]>", and reached the EOF.
	//temp_index will be the index of either the last character or the second last character.

	//copy the chunk up to (but not including) the character temp_index is pointing at.
	text_buffer = string_n_append(text_buffer, chunk_start, i);

	//copy the remaining characters up to the EOF
	while(temp_index <= (buffer_len - 1))
	{
		if(*(chunk_start + i) == LINE_FEED)
		{
			line_number += 1;
		}

		if(*(chunk_start + i) == NULL_CHARACTER)
		{
			//parse error
			parse_error(NULL_CHARACTER_IN_TEXT, line_number);

			//append the replacement character
			text_buffer = string_n_append(text_buffer, byte_seq, strlen(byte_seq));
		}
		else
		{
			//append the character
			text_buffer = string_n_append(text_buffer, (chunk_start + i), 1);
		}
		
		i += 1;
		temp_index += 1;
	}


	//get a text node created and added to the tree.
	process_token(create_character_token(*chunk_start));

	//skip total number of characters consumed, less one.
	character_skip = buffer_len - curr_buffer_index - 1;

}


/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
void initial_mode(const token *tk)
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
					//ignore the token
					//when chars are ignored, reset the text chunk variables
					text_chunk = NULL;
					text_char_count = 0;
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
				add_child_node(doc_root, (node *)c);
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
void before_html_mode(const token *tk)
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
					//ignore the token
					//when chars are ignored, reset the text chunk variables
					text_chunk = NULL;
					text_char_count = 0;
				}
				else
				{
					//create an html element, append it to the Document object, 
					element_node *e = create_element_node("html", NULL, HTML);
					add_child_node(doc_root, (node *)e);	

					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

					current_mode = BEFORE_HEAD;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_COMMENT:
			//append comment node to Document object
			{
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(doc_root, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			parse_error(UNEXPECTED_DOCTYPE, line_number);
			//ignore the token
			break;
		case TOKEN_START_TAG:
			{
				if(strcmp(tk->stt.tag_name, "html") == 0)
				{	
					//create an html element, append it to the Document object, 
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(doc_root, (node *)e);

					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

				}
				else
				{
					element_node *e = create_element_node("html", NULL, HTML);
					add_child_node(doc_root, (node *)e);

					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

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
					add_child_node(doc_root, (node *)e);

					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

					current_mode = BEFORE_HEAD;
					token_process = REPROCESS;
				}
				else
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}

			break;		
		default:
			{
				element_node *e = create_element_node("html", NULL, HTML);
				add_child_node(doc_root, (node *)e);

				open_element_stack_push(&o_e_stack, e);
				current_node = open_element_stack_top(o_e_stack);

				current_mode = BEFORE_HEAD;
				token_process = REPROCESS;
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void before_head_mode(const token *tk)
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
					//ignore the token
					//when chars are ignored, reset the text chunk variables
					text_chunk = NULL;
					text_char_count = 0;
				}
				else
				{
					element_node *e = create_element_node("head", NULL, HTML);
					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
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

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					head_element_ptr = e;

					current_mode = IN_HEAD;
				}
				else
				{
					element_node *e = create_element_node("head", NULL, HTML);
					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

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

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					head_element_ptr = e;

					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
				else
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}
			break;
		default:
			{
				element_node *e = create_element_node("head", NULL, HTML);
				add_child_node(current_node, (node *)e);

				open_element_stack_push(&o_e_stack, e); 
				current_node = open_element_stack_top(o_e_stack);

				head_element_ptr = e;

				current_mode = IN_HEAD;
				token_process = REPROCESS;
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void in_head_mode(const token *tk)
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

					//we ignore them for now.
					//when chars are ignored, reset the text chunk variables
					text_chunk = NULL;
					text_char_count = 0;
				}
				else
				{
					//Act as if an end tag token with the tag name "head" had been seen, 
					//and reprocess the current token.

					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
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
						(strcmp(tk->stt.tag_name, "link") == 0))
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(apply_foster_parenting)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
		
					//implementation pending: Acknowledge the token's self-closing flag , if it is set
				}
				else if(strcmp(tk->stt.tag_name, "meta") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(apply_foster_parenting)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}

					//implementation pending: Acknowledge the token's self-closing flag , if it is set.
				}
				else if(strcmp(tk->stt.tag_name, "title") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(apply_foster_parenting)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = RCDATA_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;

				}
				else if((strcmp(tk->stt.tag_name, "noframes") == 0) ||
						(strcmp(tk->stt.tag_name, "style") == 0))
				{
					//follow the generic raw text element parsing algorithm on p676

					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(apply_foster_parenting)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;

				}
				else if(strcmp(tk->stt.tag_name, "noscript") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_HEAD_NOSCRIPT;
				}
				else if(strcmp(tk->stt.tag_name, "script") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = SCRIPT_DATA_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "template") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					active_formatting_elements = active_formatting_list_add_marker(active_formatting_elements);

					current_mode = IN_TEMPLATE;
			
					mode_stack_push(&m_stack, IN_TEMPLATE);
					current_template_insertion_mode = mode_stack_top(m_stack);
					
				}
				else if(strcmp(tk->stt.tag_name, "head") == 0)
				{
					//parse error
					//ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);
				}
				else
				{
					//Act as if an end tag token with the tag name "head" had been seen, 
					//and reprocess the current token

					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

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
					
					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = AFTER_HEAD;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					//if there is no template element on the stack of open elements, 
					//then this is a parse error; ignore the token.
					if(get_node_by_name(o_e_stack, "template") == NULL)
					{
						//parse error, ignore the token.
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						current_node = generate_implied_end_tags(&o_e_stack, NULL);

						if(strcmp(current_node->name, "template") != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}
						
						pop_elements_up_to(&o_e_stack, "template");
						current_node = open_element_stack_top(o_e_stack);

						//clear the list of active formatting elements up to the last marker:
						active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);

						mode_stack_pop(&m_stack);

						current_mode = reset_insertion_mode(o_e_stack);

					}
				}
				else if(strcmp(tk->ett.tag_name, "head") == 0)
				{
					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);
					current_mode = AFTER_HEAD;
				}
				else
				{
					//parse error
					//ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}

			break;
		default:
			//Act as if an end tag token with the tag name "head" had been seen, 
			//and reprocess the current token.
			{
				open_element_stack_pop(&o_e_stack); 
				current_node = open_element_stack_top(o_e_stack);

				current_mode = AFTER_HEAD;
				token_process = REPROCESS;	
			}
			break;
	}

}
/*------------------------------------------------------------------------------------*/
void in_head_noscript_mode(const token *tk)
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
					parse_error(UNEXPECTED_TEXT, line_number);

					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
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
					//parse error, ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);
				}
				else
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);

					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "noscript") == 0)
				{
					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_HEAD;
				}
				else if(strcmp(tk->ett.tag_name, "br") == 0)
				{
					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_HEAD;
					token_process = REPROCESS;
				}
				else
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}
			break;
		default:
			{
				//parse error
				//current_node = current_node->parent; 
				open_element_stack_pop(&o_e_stack); 
				current_node = open_element_stack_top(o_e_stack);

				current_mode = IN_HEAD;
				token_process = REPROCESS;
			}
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void after_head_mode(const token *tk)
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
					//insert the character into the current node (it should be the html element)

					//we ignore them for now
					//when chars are ignored, reset the text chunk variables
					text_chunk = NULL;
					text_char_count = 0;
				}
				else
				{
					//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
					//frameset-ok flag back to "ok", and then reprocess the current token.
					element_node *e = create_element_node("body", NULL, HTML);
					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
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

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_BODY;
				}
				else if(strcmp(tk->stt.tag_name, "frameset") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
 
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);
					
					current_mode = IN_FRAMESET;
				}
				else if((strcmp(tk->stt.tag_name, "base") == 0) ||
						(strcmp(tk->stt.tag_name, "basefont") == 0) ||
						(strcmp(tk->stt.tag_name, "bgsound") == 0) ||
						(strcmp(tk->stt.tag_name, "link") == 0) ||
						(strcmp(tk->stt.tag_name, "meta") == 0))
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);
					//Push the node pointed to by the head element pointer onto the stack of open elements .
					//Process the token using the rules for the "in head " insertion mode .
					//Remove the node pointed to by the head element pointer from the stack of open elements .
					
					if(head_element_ptr != NULL)	//The head element pointer cannot be null at this point. (checking is for double-insurance)
					{
						open_element_stack_push(&o_e_stack, head_element_ptr); 
						current_node = open_element_stack_top(o_e_stack);

						in_head_mode(tk);

						remove_element_from_stack(&o_e_stack, head_element_ptr);
						current_node = open_element_stack_top(o_e_stack);
					}
				}
				else if((strcmp(tk->stt.tag_name, "noframes") == 0) ||
						(strcmp(tk->stt.tag_name, "script") == 0) ||
						(strcmp(tk->stt.tag_name, "style") == 0) ||
						(strcmp(tk->stt.tag_name, "template") == 0) ||
						(strcmp(tk->stt.tag_name, "title") == 0))
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);
					//Push the node pointed to by the head element pointer onto the stack of open elements .
					//Process the token using the rules for the "in head " insertion mode .
					//Remove the node pointed to by the head element pointer from the stack of open elements 
					if(head_element_ptr != NULL)	//The head element pointer cannot be null at this point. (checking is for double-insurance)
					{
						open_element_stack_push(&o_e_stack, head_element_ptr); 
						current_node = open_element_stack_top(o_e_stack);

						in_head_mode(tk);

						remove_element_from_stack(&o_e_stack, head_element_ptr);
						current_node = open_element_stack_top(o_e_stack);
					}
				}
				else if(strcmp(tk->stt.tag_name, "head") == 0)
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);
					//ignore the token
				}
				else
				{
					//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
					//frameset-ok flag back to "ok", and then reprocess the current token.

					element_node *e = create_element_node("body", NULL, HTML);
					add_child_node(current_node, (node *)e);

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

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

					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					in_head_mode(tk);
				}
				else
				{
					//parse error
					//ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}

			break;
		default:
			//Act as if a start tag token with the tag name "body" and no attributes had been seen, then set the
			//frameset-ok flag back to "ok", and then reprocess the current token.
			{
				element_node *e = create_element_node("body", NULL, HTML);
				add_child_node(current_node, (node *)e);

				open_element_stack_push(&o_e_stack, e); 
				current_node = open_element_stack_top(o_e_stack);

				current_mode = IN_BODY;
				token_process = REPROCESS;
			}
			break;
	}
}

/*------------------------------------------------------------------------------------*/
/*=======================BEGINNING OF IN BODY MODE==================================*/
void in_body_mode(const token *tk)
{

	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if(tk->cht.ch == NULL_CHARACTER)
				{
					//parse error, ignore the token
					parse_error(NULL_CHARACTER_IN_TEXT, line_number);

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
				}
				else if((tk->cht.ch == CHARACTER_TABULATION) ||
						(tk->cht.ch == LINE_FEED) ||
						(tk->cht.ch == FORM_FEED) ||
						(tk->cht.ch == CARRIAGE_RETURN) ||
						(tk->cht.ch == SPACE))
				{
					if(is_in_a_script_data_parsing_state())	//process script data in fragment case
					{
						if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
						{
							text_node *t = (text_node *)current_node->last_child;
							t->text_data = string_append(t->text_data, tk->cht.ch);
						}
						else
						{
							text_node *t = create_text_node();
							t->text_data = string_append(t->text_data, tk->cht.ch);
							add_child_node(current_node, (node *)t);
						}
					}
					else											//process normal text in body
					{
						//Reconstruct the active formatting elements , if any.
						current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

						//insert the character into the current node
						if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
						{
							;
						}
						else
						{
							text_node *t = create_text_node();		//create text node with empty string as data.
							add_child_node(current_node, (node *)t);
						}
					}
	
				}
				else
				{
					if(is_in_a_script_data_parsing_state())	//process script data in fragment case
					{
						if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
						{
							text_node *t = (text_node *)current_node->last_child;
							t->text_data = string_append(t->text_data, tk->cht.ch);
						}
						else
						{
							text_node *t = create_text_node();
							t->text_data = string_append(t->text_data, tk->cht.ch);
							add_child_node(current_node, (node *)t);
						}
					}
					else											//process normal text in body
					{
						//Reconstruct the active formatting elements , if any.
						current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);
					
						//Insert the token's character into the current node .
						//Set the frameset-ok flag to "not ok".
						if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
						{
							;
						}
						else
						{
							text_node *t = create_text_node();		//create text node with empty string as data.
							add_child_node(current_node, (node *)t);
						}
					}
				}
			}
			break;

		case TOKEN_MULTI_CHAR:						//only the SCRIPT_DATA_STATE will produce this type of token
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;
					t->text_data = string_n_append(t->text_data, tk->mcht.mch, tk->mcht.char_count);
				}
				else
				{
					text_node *t = create_text_node();
					t->text_data = string_n_append(t->text_data, tk->mcht.mch, tk->mcht.char_count);
					add_child_node(current_node, (node *)t);
				}
			}
			break;

		case TOKEN_COMMENT:
			{
				comment_node *c;

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			parse_error(UNEXPECTED_DOCTYPE, line_number);

			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;

					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

			}
			break;
		case TOKEN_START_TAG:
			{	
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;


					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				if(strcmp(tk->stt.tag_name, "html") == 0)
				{	
					element_node *html_node;

					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);

					//If there is a template element in the stack of open elements, then ignore the token.
					if(get_node_by_name(o_e_stack, "template") != NULL)
					{
						; 
					}
					else
					{
						//For each attribute on the token, check to see if the attribute 
						//is already present on the top element of the stack of open elements . 
						//If it is not, add the attribute and its corresponding value to that element
						html_node = get_node_by_name(o_e_stack, "html");

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
											html_attribute_list_cons(temp_ptr->name, temp_ptr->value, temp_ptr->attr_ns, html_node->attributes);
									}
								}

								temp_ptr = temp_ptr->tail;
							}
						}
					}
					
				}
				else if((strcmp(tk->stt.tag_name, "base") == 0) ||
						(strcmp(tk->stt.tag_name, "basefont") == 0) ||
						(strcmp(tk->stt.tag_name, "bgsound") == 0) ||
						(strcmp(tk->stt.tag_name, "link") == 0) ||
						(strcmp(tk->stt.tag_name, "meta") == 0) ||
						(strcmp(tk->stt.tag_name, "noframes") == 0) ||
						(strcmp(tk->stt.tag_name, "script") == 0) ||
						(strcmp(tk->stt.tag_name, "style") == 0) ||
						(strcmp(tk->stt.tag_name, "template") == 0) ||
						(strcmp(tk->stt.tag_name, "title") == 0))
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "body") == 0)
				{
					//If the second element on the stack of open elements is not a body element, 
					//if the stack of open elements has only one node on it, 
					//or if there is a template element on the stack of open elements, 
					//then ignore the token. (fragment case)

					if(((o_e_stack != NULL) && (o_e_stack->tail == NULL)) 
					   ||
					   (get_node_by_name(o_e_stack, "template") != NULL))
					{
						;	//ignore the token
					}
					else
					{
						//set the frameset-ok flag to "not ok"
						//for each attribute on the token, check to see if the attribute is already present
						//on the body element if it is not, add the attribute and its corresponding value to that element.
						element_node *body_node = get_node_by_name(o_e_stack, "body");
						attribute_list *curr_token_attrs = tk->stt.attributes;

						//parse error
						parse_error(UNEXPECTED_START_TAG, line_number);

						if(body_node != NULL)
						{
							while(curr_token_attrs != NULL)
							{
								//if attribute on token is not already present in html_node->attributes:
								if((curr_token_attrs->name != NULL) && (attribute_name_in_list(curr_token_attrs->name, body_node->attributes) == 0))
								{
									body_node->attributes = html_attribute_list_cons(curr_token_attrs->name, 
																				 curr_token_attrs->value, 
																				 curr_token_attrs->attr_ns,
																				 body_node->attributes);
								}

								curr_token_attrs = curr_token_attrs->tail;
							}
						}
					}

				}
				else if(strcmp(tk->stt.tag_name, "frameset") == 0)
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);
					//implementation pending
					//Insert an HTML element for the token.
					//Switch the insertion mode to "in frameset"
				}
				else if((strcmp(tk->stt.tag_name, "address") == 0) ||
						(strcmp(tk->stt.tag_name, "article") == 0) ||
						(strcmp(tk->stt.tag_name, "aside") == 0) ||
						(strcmp(tk->stt.tag_name, "blockquote") == 0) ||
						(strcmp(tk->stt.tag_name, "center") == 0) ||
						(strcmp(tk->stt.tag_name, "details") == 0) ||
						(strcmp(tk->stt.tag_name, "dialog") == 0) ||
						(strcmp(tk->stt.tag_name, "dir") == 0) ||
						(strcmp(tk->stt.tag_name, "div") == 0) ||
						(strcmp(tk->stt.tag_name, "dl") == 0) ||
						(strcmp(tk->stt.tag_name, "fieldset") == 0) ||
						(strcmp(tk->stt.tag_name, "figcaption") == 0) ||
						(strcmp(tk->stt.tag_name, "figure") == 0) ||
						(strcmp(tk->stt.tag_name, "footer") == 0) ||
						(strcmp(tk->stt.tag_name, "header") == 0) ||
						(strcmp(tk->stt.tag_name, "hgroup") == 0) ||
						(strcmp(tk->stt.tag_name, "main") == 0) ||
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
					element_node *match_node;
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}
					
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

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
					element_node *match_node;
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}

					if((strcmp(current_node->name, "h1") == 0) ||
					   (strcmp(current_node->name, "h2") == 0) ||
					   (strcmp(current_node->name, "h3") == 0) ||
					   (strcmp(current_node->name, "h4") == 0) ||
					   (strcmp(current_node->name, "h5") == 0) ||
					   (strcmp(current_node->name, "h6") == 0))
					{
						//parse error;
						parse_error(UNEXPECTED_START_TAG, line_number);

						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}

					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

				}
				else if((strcmp(tk->stt.tag_name, "pre") == 0) ||
						(strcmp(tk->stt.tag_name, "listing") == 0))
				{
					//If the stack of open elements has a p element in button scope ,
					//then act as if an end tag with the tag name "p" had been seen.
					element_node *match_node;
					element_node *e;

					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}
										
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					//If the next token is a U+000A LINE FEED (LF) character token, 
					//then ignore that token and move on to the next one. 
					//(Newlines at the start of prep168 blocks are ignored as an authoring convenience.)
					if(((curr_buffer_index + 1) < buffer_len) && (file_buf[curr_buffer_index + 1] == LINE_FEED))
					{	
						character_skip += 1;
						line_number += 1;
					}

					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "form") == 0)
				{
					if(form_element_ptr != NULL)
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_START_TAG, line_number);

					}
					else
					{
						//If the stack of open elements has a p element in button scope , then act as if an end tag 
						//with the tag name "p" had been seen.
						
						element_node *match_node;
						element_node *e;

						if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
						{
							pop_elements_up_to(&o_e_stack, "p");
							current_node = open_element_stack_top(o_e_stack);
						}
					
						e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
						if(apply_foster_parenting == 1)
						{
							add_child_to_foster_parent(o_e_stack, (node *)e);
						}
						else
						{
							add_child_node(current_node, (node *)e);
						}
						open_element_stack_push(&o_e_stack, e); 
						current_node = open_element_stack_top(o_e_stack);

						//set the form element pointer to point to the element created.
						form_element_ptr = e;
					}
				}
				else if(strcmp(tk->stt.tag_name, "li") == 0)
				{
					//Set the frameset-ok flag to "not ok".
					element_node *e;
					element_node *match_node, *temp_element;
					element_stack *temp_stack = o_e_stack;

					while(temp_stack != NULL)
					{
						temp_element = open_element_stack_top(temp_stack);

						if(strcmp(temp_element->name, "li") == 0)
						{
							pop_elements_up_to(&o_e_stack, "li");
							current_node = open_element_stack_top(o_e_stack);
							break;
						}
						
						if((is_in_special_category(temp_element) == 1) &&
						   (strcmp(temp_element->name, "address") != 0) &&
						   (strcmp(temp_element->name, "div") != 0) &&
						   (strcmp(temp_element->name, "p") != 0))
						{
							break;
						}

						temp_stack = previous_stack_node(temp_stack);
					}

					//if the stack of open elements has a p element in button scope , then act as if an end tag 
					//with the tag name "p" had been seen.	
					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);
					
				}
				else if((strcmp(tk->stt.tag_name, "dd") == 0) ||
						(strcmp(tk->stt.tag_name, "dt") == 0))
			
				{
					//Set the frameset-ok flag to "not ok".
					element_node *e;
					element_node *match_node, *temp_element;
					element_stack *temp_stack = o_e_stack;

					while(temp_stack != NULL)
					{
						temp_element = open_element_stack_top(temp_stack);

						if((strcmp(temp_element->name, "dd") == 0) ||
						   (strcmp(temp_element->name, "dt") == 0))
						{
							pop_elements_up_to(&o_e_stack, temp_element->name);
							current_node = open_element_stack_top(o_e_stack);
							break;
						}
						
						if((is_in_special_category(temp_element) == 1) &&
						   (strcmp(temp_element->name, "address") != 0) &&
						   (strcmp(temp_element->name, "div") != 0) &&
						   (strcmp(temp_element->name, "p") != 0))
						{
							break;
						}

						temp_stack = previous_stack_node(temp_stack);
					}

					//if the stack of open elements has a p element in button scope , then act as if an end tag 
					//with the tag name "p" had been seen.	
					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

				}
				else if(strcmp(tk->stt.tag_name, "plaintext") == 0)
				{
					//If the stack of open elements has a p element in button scope ,
					//then act as if an end tag with the tag name "p" had been seen.
					element_node *match_node;
					element_node *e;

					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = PLAINTEXT_STATE;
				}
				else if(strcmp(tk->stt.tag_name, "button") == 0)
				{
					element_node *e;
					element_node *match_node;
					if(has_element_in_scope(o_e_stack, "button", &match_node) == 1)
					{
						//parse error
						parse_error(UNEXPECTED_START_TAG, line_number);

						pop_elements_up_to(&o_e_stack, "button");
						current_node = open_element_stack_top(o_e_stack);

						token_process = REPROCESS;
					}
					else
					{
						//Reconstruct the active formatting elements, if any.
						current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

						//Insert an HTML element for the token.
						e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
						if(apply_foster_parenting == 1)
						{
							add_child_to_foster_parent(o_e_stack, (node *)e);
						}
						else
						{
							add_child_node(current_node, (node *)e);
						}
						open_element_stack_push(&o_e_stack, e);
						current_node = open_element_stack_top(o_e_stack);

						//Set the frameset-ok flag to "not ok".
					}

				}
				else if(strcmp(tk->stt.tag_name, "a") == 0)
				{
					element_node *element_to_be_removed;
					element_node *e;

					if(get_formatting_element(active_formatting_elements, "a") != NULL)
					{
						//parse error, 
						//act as if an end tag with the tag name "a" had been seen:

						/*----------------------Adoption Agency Algorithm-------------------------------------*/

						adoption_agency_algorithm(&active_formatting_elements, &o_e_stack, &current_node, "a");

						/*------------------------------------------------------------------------------------*/
						//then remove that element from the list of active formatting elements and the stack of open elements if the end tag didn't
						//already remove it (it might not have if the element is not in table scope ).
						element_to_be_removed = get_formatting_element(active_formatting_elements, "a");

						if(element_to_be_removed != NULL)
						{
							active_formatting_elements =
								remove_element_from_active_formatting_list(active_formatting_elements, element_to_be_removed);

							remove_element_from_stack(&o_e_stack, element_to_be_removed);
							current_node = open_element_stack_top(o_e_stack);
						}
					}
		
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Insert an HTML element for the token. 
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

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
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Insert an HTML element for the token. 
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

					//Push onto the list of active formatting elements that element.
					active_formatting_elements = active_formatting_list_add_element(e, active_formatting_elements);

				}
				else if(strcmp(tk->stt.tag_name, "nobr") == 0)
				{ 
					element_node *e;
					element_node *match_node;

					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//If the stack of open elements has a nobr element in scope , 
					//then this is a parse error ; act as if an end tag with the tag name "nobr" had been seen, 
					//then once again reconstruct the active formatting elements , if any.
					if(has_element_in_scope(o_e_stack, "nobr", &match_node) == 1)
					{	
						//parse error
						adoption_agency_algorithm(&active_formatting_elements, &o_e_stack, &current_node, "nobr");

						current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);
					}

					//Insert an HTML element for the token. 
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					//Push onto the list of active formatting elements that element.
					active_formatting_elements = active_formatting_list_add_element(e, active_formatting_elements);

				}
				else if((strcmp(tk->stt.tag_name, "applet") == 0) ||
						(strcmp(tk->stt.tag_name, "marquee") == 0) ||
						(strcmp(tk->stt.tag_name, "object") == 0))
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					//Insert a marker at the end of the list of active formatting elements
					active_formatting_elements = active_formatting_list_add_marker(active_formatting_elements);

					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "table") == 0)
				{
					element_node *e;
					element_node *match_node;
						
					//If the Document is not set to quirks mode , and the stack of open elements has a p element in button scope'
					//then act as if an end tag with the tag name "p" had been seen.
					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);
					
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
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					
					//Acknowledge the token's self-closing flag, if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "input") == 0)
				{
					element_node *e;
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					
					//Acknowledge the token's self-closing flag, if it is set.
					//If the token does not have an attribute with the name "type", or if it does, but that attribute's value is not
					//an ASCII case-insensitive match for the string "hidden", then: set the frameset-ok flag to "not ok".
				}
				else if((strcmp(tk->stt.tag_name, "menuitem") == 0) ||
						(strcmp(tk->stt.tag_name, "param") == 0) ||
						(strcmp(tk->stt.tag_name, "source") == 0) ||
						(strcmp(tk->stt.tag_name, "track") == 0))
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					//Acknowledge the token's self-closing flag , if it is set.
				}
				else if(strcmp(tk->stt.tag_name, "hr") == 0)
				{
					element_node *match_node;
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);

					//If the stack of open elements has a p element in button scope, 
					//then act as if an end tag with the tag name "p" had been seen.
					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}
					
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					//Acknowledge the token's self-closing flag , if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "image") == 0)
				{
					//parse error
					
					//free(tk->stt.tag_name);
					//tk->stt.tag_name = strdup("img");
					//token_process = REPROCESS;

					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					e = create_element_node("img", tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					
					//Acknowledge the token's self-closing flag, if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "isindex") == 0)
				{
					//parse error
					parse_error(DEPRECATED_TAG, line_number);

					if(form_element_ptr != NULL)
					{
						;	//ignore the token
					}
					else
					{
						//Acknowledge the token's self-closing flag , if it is set.
						
						//Act as if a start tag token with the tag name "form" had been seen.
						/*------------------------------------------------------------------*/
						element_node *match_node;
						element_node *form_e, *hr_e_1, *hr_e_2, *label_e, *input_e;
						text_node *label_text;
						unsigned char *action_attr_val, *prompt_attr_val;

						if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
						{
							pop_elements_up_to(&o_e_stack, "p");
							current_node = open_element_stack_top(o_e_stack);
						}
					
						form_e = create_element_node("form", NULL, HTML);
						add_child_node(current_node, (node *)form_e);
						open_element_stack_push(&o_e_stack, form_e); 
						current_node = open_element_stack_top(o_e_stack);

						//set the form element pointer to point to the element created.
						form_element_ptr = form_e;
						/*------------------------------------------------------------------*/

						action_attr_val = get_attribute_value("action", tk->stt.attributes);
						if(action_attr_val != NULL)
						{
							form_e->attributes = html_attribute_list_cons("action", action_attr_val, DEFAULT, form_e->attributes);
						}
						
						hr_e_1 = create_element_node("hr", NULL, HTML);
						add_child_node(current_node, (node *)hr_e_1);

						current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

						label_e = create_element_node("label", NULL, HTML);
						add_child_node(current_node, (node *)label_e);
						open_element_stack_push(&o_e_stack, label_e); 
						current_node = open_element_stack_top(o_e_stack);
						
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
					
						open_element_stack_pop(&o_e_stack);		//back to form element:
						current_node = open_element_stack_top(o_e_stack);
						//current_node = (node *)form_e;

						input_e = create_element_node("input", NULL, HTML);
						
						input_e->attributes = tk->stt.attributes;
						input_e->attributes = remove_attribute("name", input_e->attributes);
						input_e->attributes = remove_attribute("action", input_e->attributes);
						input_e->attributes = remove_attribute("prompt", input_e->attributes);

						//Set the name attribute of the resulting input element to the value "isindex".
						input_e->attributes = html_attribute_list_cons("name", "isindex", DEFAULT, input_e->attributes);
						add_child_node(current_node, (node *)input_e);		//add input_e to the form element.

						hr_e_2 = create_element_node("hr", NULL, HTML);
						add_child_node(current_node, (node *)hr_e_2);		//add hr_e_2 to the form element.

						//back to the parent of "form"
						//current_node = form_e->parent;
						open_element_stack_pop(&o_e_stack);
						current_node = open_element_stack_top(o_e_stack);


						//set the form_element_ptr to NULL.
						form_element_ptr = NULL;
					}
				}
				else if(strcmp(tk->stt.tag_name, "textarea") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					//ignore newline at the start of the textarea
					if(((curr_buffer_index + 1) < buffer_len) && (file_buf[curr_buffer_index + 1] == LINE_FEED))
					{	
						character_skip += 1;
						line_number += 1;
					}

					current_state = RCDATA_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->stt.tag_name, "xmp") == 0)
				{
					element_node *match_node;
					element_node *e;

					//If the stack of open elementsp has a p element in button scope , 
					//then act as if an end tag with the tag name "p" had been seen.
					if(has_element_in_button_scope(o_e_stack, "p", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}
					
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Set the frameset-ok flag to "not ok".
					//Follow the generic raw text element parsing algorithm .
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "iframe") == 0)
				{
					//Set the frameset-ok flag to "not ok".
					//Follow the generic raw text element parsing algorithm .
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
					
				}
				else if((strcmp(tk->stt.tag_name, "noembed") == 0) ||
						(strcmp(tk->stt.tag_name, "noscript") == 0))	//if the scripting flag is enabled
				{
					//Follow the generic raw text element parsing algorithm .
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_state = RAWTEXT_STATE;
					original_insertion_mode = current_mode;
					current_mode = TEXT;
				}
				else if(strcmp(tk->stt.tag_name, "select") == 0)
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

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

					//If the current node is an option element, 
					//then act as if an end tag with the tag name "option" had been seen.
					if(strcmp(current_node->name, "option") == 0)
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}

					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Insert an HTML element for the token.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

				}
				else if((strcmp(tk->stt.tag_name, "rp") == 0) ||
						(strcmp(tk->stt.tag_name, "rt") == 0))
				{
					element_node *e;
					element_node *match_node;

					if(has_element_in_scope(o_e_stack, "ruby", &match_node) == 1)
					{
						current_node = generate_implied_end_tags(&o_e_stack, NULL);
					}

					if(strcmp(current_node->name, "ruby") != 0)
					{
						//parse error
						parse_error(UNEXPECTED_START_TAG, line_number);
					}

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

				}
				else if(strcmp(tk->stt.tag_name, "math") == 0)
				{
					element_node *e;
					//Reconstruct the active formatting elements , if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);
					
					//Adjust MathML attributes for the token. (This fixes the case of MathML attributes that are not all lowercase.)
					adjust_mathml_attributes(tk->stt.attributes);

					//Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink.)
					adjust_foreign_attributes(tk->stt.attributes);

					//Insert a foreign element for the token, in the MathML namespace .
					//If the token has its self-closing flag set, pop the current node off the stack of open elements and acknowledge the token's self-closing flag .
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, MATHML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}

					if(tk->stt.self_closing_flag == SET)
					{
						;
					}
					else
					{
						open_element_stack_push(&o_e_stack, e); 
						current_node = open_element_stack_top(o_e_stack);
					}
				}
				else if(strcmp(tk->stt.tag_name, "svg") == 0)
				{
					element_node *e;

					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					//Adjust SVG attributes for the token. (This fixes the case of SVG attributes that are not all lowercase.)
					adjust_svg_attributes(tk->stt.attributes);

					//Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink in SVG.)
					adjust_foreign_attributes(tk->stt.attributes);
					
					//Insert a foreign element for the token, in the SVG namespace .
					//If the token has its self-closing flag set, pop the current node off the stack of open elements and acknowledge the token's self-closing flag.
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, SVG);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}

					if(tk->stt.self_closing_flag == SET)
					{
						;
					}
					else
					{
						open_element_stack_push(&o_e_stack, e); 
						current_node = open_element_stack_top(o_e_stack);
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
					//parse error, ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);
				}
				else
				{
					element_node *e;
					//This element will be an ordinary element.

					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);
					
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}

					if(tk->stt.self_closing_flag == SET)
					{
						;
					}
					else
					{
						open_element_stack_push(&o_e_stack, e); 
						current_node = open_element_stack_top(o_e_stack);
					}
				}

			}
			break;
		case TOKEN_END_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;

					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				if(strcmp(tk->ett.tag_name, "body") == 0)
				{
					element_node *match_node;

					if(has_element_in_scope(o_e_stack, "body", &match_node) == 0)
					{
						//parser error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						current_mode = AFTER_BODY;
					}

				}
				else if(strcmp(tk->ett.tag_name, "html") == 0)
				{
					element_node *match_node;

					if(has_element_in_scope(o_e_stack, "body", &match_node) == 0)
					{
						//parser error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						current_mode = AFTER_BODY;
						token_process = REPROCESS;
					}
				}
				else if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					in_head_mode(tk);
				}
				else if(strcmp(tk->ett.tag_name, "p") == 0)
				{
					element_node *match_node;

					if(has_element_in_button_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1)
					{
						//Generate implied end tags, except for elements with the same tag name as the token("p").
						current_node = generate_implied_end_tags(&o_e_stack, "p");

						//If the current node is not an element with the same tag name as that of the token, then this is a parse error
						if(strcmp(current_node->name, "p") != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);	
						}

						pop_elements_up_to(&o_e_stack, "p");
						current_node = open_element_stack_top(o_e_stack);
					}
					else
					{
						element_node *e;
						//parse error, act as if a start tag with the tag name "p" had been seen,
						//then reprocess the current token.
						parse_error(UNEXPECTED_END_TAG, line_number);

						e = create_element_node("p", NULL, HTML);
						if(apply_foster_parenting == 1)
						{
							add_child_to_foster_parent(o_e_stack, (node *)e);
						}
						else
						{
							add_child_node(current_node, (node *)e);
						}
						open_element_stack_push(&o_e_stack, e); 
						current_node = open_element_stack_top(o_e_stack);

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
						(strcmp(tk->ett.tag_name, "dialog") == 0) ||
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
						(strcmp(tk->ett.tag_name, "main") == 0) ||
						(strcmp(tk->ett.tag_name, "menu") == 0) ||
						(strcmp(tk->ett.tag_name, "nav") == 0) ||
						(strcmp(tk->ett.tag_name, "ol") == 0) ||
						(strcmp(tk->ett.tag_name, "pre") == 0) ||
						(strcmp(tk->ett.tag_name, "section") == 0) ||
						(strcmp(tk->ett.tag_name, "summary") == 0) ||
						(strcmp(tk->ett.tag_name, "ul") == 0))
				{
					element_node *match_node;

					if(has_element_in_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = generate_implied_end_tags(&o_e_stack, NULL);

						if(strcmp(current_node->name, tk->ett.tag_name ) != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}

						pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
						current_node = open_element_stack_top(o_e_stack);

					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}

				}
				else if(strcmp(tk->ett.tag_name, "form") == 0)
				{
					element_node *match_node;
					element_node *form_element_node;
					
					form_element_node = form_element_ptr;

					form_element_ptr = NULL;

					if(form_element_node == NULL)
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else if(has_element_in_scope(o_e_stack, "form", &match_node) == 0)
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						current_node = generate_implied_end_tags(&o_e_stack, NULL);

						if(strcmp(current_node->name , form_element_node->name) != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}

						//pop_elements_up_to(&o_e_stack, "form");
						remove_element_from_stack(&o_e_stack, form_element_node);
						current_node = open_element_stack_top(o_e_stack);
					}

				}
				else if((strcmp(tk->ett.tag_name, "h1") == 0) ||
						(strcmp(tk->ett.tag_name, "h2") == 0) ||
						(strcmp(tk->ett.tag_name, "h3") == 0) ||
						(strcmp(tk->ett.tag_name, "h4") == 0) ||
						(strcmp(tk->ett.tag_name, "h5") == 0) ||
						(strcmp(tk->ett.tag_name, "h6") == 0))
				{
					element_node *match_node;

					if((has_element_in_scope(o_e_stack, "h1", &match_node) == 0) &&
					   (has_element_in_scope(o_e_stack, "h2", &match_node) == 0) &&
					   (has_element_in_scope(o_e_stack, "h3", &match_node) == 0) &&
					   (has_element_in_scope(o_e_stack, "h4", &match_node) == 0) &&
					   (has_element_in_scope(o_e_stack, "h5", &match_node) == 0) &&
					   (has_element_in_scope(o_e_stack, "h6", &match_node) == 0))
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						element_node *temp_element;
						int header_found = 0;

						current_node = generate_implied_end_tags(&o_e_stack, NULL);

						if(strcmp(current_node->name, tk->ett.tag_name) != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}
						
						while(o_e_stack != NULL)
						{
							temp_element = open_element_stack_top(o_e_stack);

							if((strcmp(temp_element->name, "h1") == 0) ||
							   (strcmp(temp_element->name, "h2") == 0) ||
							   (strcmp(temp_element->name, "h3") == 0) ||
							   (strcmp(temp_element->name, "h4") == 0) ||
							   (strcmp(temp_element->name, "h5") == 0) ||
							   (strcmp(temp_element->name, "h6") == 0))
							{
								header_found = 1;
							}

							open_element_stack_pop(&o_e_stack);
	
							if(header_found == 1)
							{
								break;
							}
						}
						
						//o_e_stack will not be NULL in the last step, as a header element must be found(one of them must exist), 
						//and loop will exit after it is popped.
						current_node = open_element_stack_top(o_e_stack);
					}
				}
				else if(strcmp(tk->ett.tag_name, "li") == 0)
				{
					element_node *match_node;
					
					if(has_element_in_list_item_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1)
					{
						
						current_node = generate_implied_end_tags(&o_e_stack, "li");

						if(strcmp(current_node->name, "li") != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}
						
						pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
						current_node = open_element_stack_top(o_e_stack);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
				}
				else if((strcmp(tk->ett.tag_name, "dd") == 0) ||
						(strcmp(tk->ett.tag_name, "dt") == 0))
				{
					element_node *match_node;

					if(has_element_in_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1)
					{
						
						current_node = generate_implied_end_tags(&o_e_stack, tk->ett.tag_name);

						if(strcmp(current_node->name, tk->ett.tag_name) != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}

						pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
						current_node = open_element_stack_top(o_e_stack);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
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
					adoption_agency_algorithm(&active_formatting_elements, &o_e_stack, &current_node, tk->ett.tag_name);

				}
				else if((strcmp(tk->ett.tag_name, "applet") == 0) ||
						(strcmp(tk->ett.tag_name, "marquee") == 0) ||
						(strcmp(tk->ett.tag_name, "object") == 0))
				{
					element_node *match_node;

					//If the stack of open elements does not have an element in scope with the same tag name as that
					//of the token, then this is a parse error ; ignore the token.
					if(has_element_in_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1)
					{
						current_node = generate_implied_end_tags(&o_e_stack, NULL);

						if(strcmp(current_node->name, tk->ett.tag_name) != 0)
						{
							//parse error
							parse_error(UNEXPECTED_END_TAG, line_number);
						}

						pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
						current_node = open_element_stack_top(o_e_stack);

						//Clear the list of active formatting elements up to the last marker .
						active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}

				}
				else if(strcmp(tk->ett.tag_name, "br") == 0)
				{
					element_node *e;
					//parse error . Act as if a start tag token with the tag name "br" had been seen. 
					//ignore the end tag token.
					parse_error(INVALID_END_TAG, line_number);
					
					//Reconstruct the active formatting elements, if any.
					current_node = reconstruct_active_formatting_elements(active_formatting_elements, &o_e_stack);

					e = create_element_node("br", NULL, HTML);
					if(apply_foster_parenting == 1)
					{
						add_child_to_foster_parent(o_e_stack, (node *)e);
					}
					else
					{
						add_child_node(current_node, (node *)e);
					}
					
					//Acknowledge the token's self-closing flag, if it is set.
					//Set the frameset-ok flag to "not ok".
				}
				else if(strcmp(tk->ett.tag_name, "sarcasm") == 0)
				{
					//act as described in the "any other end tag" entry below
					element_stack *temp_stack = o_e_stack;
					element_node *temp_element;
					
					while(temp_stack != NULL)
					{
						temp_element = open_element_stack_top(temp_stack);

						if(strcmp(temp_element->name, tk->ett.tag_name) == 0)
						{
							current_node = generate_implied_end_tags(&o_e_stack, tk->ett.tag_name);

							if(strcmp(current_node->name, tk->ett.tag_name) != 0)
							{
								//parse error
								parse_error(UNEXPECTED_END_TAG, line_number);
							}

							pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
							current_node = open_element_stack_top(o_e_stack);
							break;
						}
						else if(is_in_special_category(temp_element) == 1)
						{
							//parse error, ignore the token
							parse_error(UNEXPECTED_END_TAG, line_number);
							break;
						}
						else
						{
							temp_stack = previous_stack_node(temp_stack);
						}
					}
				}
				else
				{
					element_stack *temp_stack = o_e_stack;
					element_node *temp_element;
					
					while(temp_stack != NULL)
					{
						temp_element = open_element_stack_top(temp_stack);

						if(strcmp(temp_element->name, tk->ett.tag_name) == 0)
						{
							current_node = generate_implied_end_tags(&o_e_stack, tk->ett.tag_name);

							if(strcmp(current_node->name, tk->ett.tag_name) != 0)
							{
								//parse error
								parse_error(UNEXPECTED_END_TAG, line_number);
							}

							pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
							current_node = open_element_stack_top(o_e_stack);
							break;
						}
						else if(is_in_special_category(temp_element) == 1)
						{
							//parse error, ignore the token
							parse_error(UNEXPECTED_END_TAG, line_number);
							break;	
						}
						else
						{
							temp_stack = previous_stack_node(temp_stack);
						}

					}
				}
			}

			break;
		case TOKEN_EOF:
			{
				//if the stack of template insertion modes is not empty, 
				//then process the token using the rules for the "in template" insertion mode.
				if(m_stack != NULL)
				{
					in_template_mode(tk);
				}
				else
				{
					;//Otherwise, stop parsing.
				}
			}
			break;
		default:
			break;
	}

}

/*=======================END OF IN BODY MODE==================================*/
/*------------------------------------------------------------------------------------*/
void text_mode(const token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if(is_in_a_script_data_parsing_state())
				{
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						text_node *t = (text_node *)current_node->last_child;
						t->text_data = string_append(t->text_data, tk->cht.ch);
					}
					else
					{
						text_node *t = create_text_node();
						t->text_data = string_append(t->text_data, tk->cht.ch);
						add_child_node(current_node, (node *)t);
					}
				}
				else
				{
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						;
					}
					else
					{
						text_node *t = create_text_node();		//create text node with empty string as data.
						add_child_node(current_node, (node *)t);
					}
				}
				//note: if current node is 'textarea' ignore LINE_FEED characters at the beginning of text

			}
			break;
		case TOKEN_MULTI_CHAR:						//only the SCRIPT_DATA_STATE will produce this type of token
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;
					t->text_data = string_n_append(t->text_data, tk->mcht.mch, tk->mcht.char_count);
				}
				else
				{
					text_node *t = create_text_node();
					t->text_data = string_n_append(t->text_data, tk->mcht.mch, tk->mcht.char_count);
					add_child_node(current_node, (node *)t);
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "script") == 0)
				{
					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = original_insertion_mode;

				}
				else		//any other end tag
				{
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						text_node *t = (text_node *)current_node->last_child;

						//copy text_chunk from file buffer to text_buffer:
						if((text_chunk != NULL) && (text_char_count > 0))
						{
							text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
						}
						text_chunk = NULL;
						text_char_count = 0;

					
						if(text_buffer != NULL)
						{
							t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
							free(text_buffer);
							text_buffer = NULL;
						}
					}

					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);
					current_mode = original_insertion_mode;
				}
			}
			break;
		case TOKEN_EOF:
			{
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);

				open_element_stack_pop(&o_e_stack); 
				current_node = open_element_stack_top(o_e_stack);

				current_mode = original_insertion_mode;
				token_process = REPROCESS;
			}
			break;
		default:
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void in_table_mode(const token *tk)
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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
			break;
		case TOKEN_START_TAG:
		    {
				if(strcmp(tk->stt.tag_name, "caption") == 0)
				{
					//Clear the stack back to a table context.
					//Insert a marker at the end of the list of active formatting elements .
					//Insert an HTML element for the token, then switch the insertion mode to "in caption "
					element_node *e;

					current_node = back_to_table_context(&o_e_stack);  
			
					active_formatting_elements = active_formatting_list_add_marker(active_formatting_elements);

					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_CAPTION;
				}
				else if(strcmp(tk->stt.tag_name, "colgroup") == 0)
				{
					//Clear the stack back to a table context.
					//Insert an HTML element for the token, then switch the insertion mode to "in column group "
					element_node *e;
				
					current_node = back_to_table_context(&o_e_stack);
			
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_COLUMN_GROUP;
				}
				else if(strcmp(tk->stt.tag_name, "col") == 0)
				{	
					//Act as if a start tag token with the tag name "colgroup" had been seen, 
					//then reprocess the current token.
					
					element_node *e;
					
					current_node = back_to_table_context(&o_e_stack);

					e = create_element_node("colgroup", NULL, HTML);
					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_COLUMN_GROUP;
					token_process = REPROCESS;
				}
				else if((strcmp(tk->stt.tag_name, "tbody") == 0) ||
						(strcmp(tk->stt.tag_name, "tfoot") == 0) ||
						(strcmp(tk->stt.tag_name, "thead") == 0))
				{
					element_node *e;
			
					current_node = back_to_table_context(&o_e_stack);
					
					e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_TABLE_BODY;

				}
				else if((strcmp(tk->stt.tag_name, "td") == 0) ||
						(strcmp(tk->stt.tag_name, "th") == 0) ||
						(strcmp(tk->stt.tag_name, "tr") == 0))
				{
					element_node *e;

					current_node = back_to_table_context(&o_e_stack);

					e = create_element_node("tbody", NULL, HTML);
					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_TABLE_BODY;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->stt.tag_name, "table") == 0)
				{
					element_node *match_node;

					//parse error
					//Act as if an end tag token with the tag name "table" had been seen, then, if that token
					//wasn't ignored, reprocess the current token.
					parse_error(UNEXPECTED_START_TAG, line_number);					

					if(has_element_in_table_scope(o_e_stack, "table", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "table");
						current_node = open_element_stack_top(o_e_stack);

						//Reset the insertion mode appropriately
						current_mode = reset_insertion_mode(o_e_stack);
						token_process = REPROCESS;
					}
					else
					{
						//ignore the token
					}	
				}
				else if((strcmp(tk->stt.tag_name, "style") == 0) ||
						(strcmp(tk->stt.tag_name, "script") == 0) ||
						(strcmp(tk->stt.tag_name, "template") == 0))
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
					if(attribute_name_in_list("type", tk->stt.attributes) == 0)
					{
						if((strcmp(current_node->name, "table") == 0) ||
						   (strcmp(current_node->name, "tbody") == 0) ||
						   (strcmp(current_node->name, "tfoot") == 0) ||
						   (strcmp(current_node->name, "thead") == 0) ||
						   (strcmp(current_node->name, "tr") == 0))
						{
							apply_foster_parenting = 1;
						}

						in_body_mode(tk);
				
						apply_foster_parenting = 0;
					}
					else
					{
						unsigned char *attr_val = get_attribute_value("type", tk->stt.attributes);

						if((attr_val != NULL) && (strcasecmp(attr_val, "hidden") == 0))
						{
							element_node *e;

							//parse error
							parse_error(UNEXPECTED_START_TAG, line_number);

							e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
							add_child_node(current_node, (node *)e);

						}
						else
						{
							if((strcmp(current_node->name, "table") == 0) ||
							   (strcmp(current_node->name, "tbody") == 0) ||
							   (strcmp(current_node->name, "tfoot") == 0) ||
							   (strcmp(current_node->name, "thead") == 0) ||
							   (strcmp(current_node->name, "tr") == 0))
							{
								apply_foster_parenting = 1;
							}

							in_body_mode(tk);
				
							apply_foster_parenting = 0;

						}

						free(attr_val);
					}
				}
				else if(strcmp(tk->stt.tag_name, "form") == 0)
				{
					//Parse error
					parse_error(UNEXPECTED_START_TAG, line_number);

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
					//same as "default":
					if((strcmp(current_node->name, "table") == 0) ||
					   (strcmp(current_node->name, "tbody") == 0) ||
					   (strcmp(current_node->name, "tfoot") == 0) ||
				       (strcmp(current_node->name, "thead") == 0) ||
				       (strcmp(current_node->name, "tr") == 0))
					{
						apply_foster_parenting = 1;
					}

					in_body_mode(tk);
				
					apply_foster_parenting = 0;
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "table") == 0)
				{	
					element_node *match_node;
					
					if(has_element_in_table_scope(o_e_stack, "table", &match_node) == 1)
					{
						pop_elements_up_to(&o_e_stack, "table");
						current_node = open_element_stack_top(o_e_stack);

						//Reset the insertion mode appropriately
						current_mode = reset_insertion_mode(o_e_stack);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
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
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
				else if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else
				{
					 //same as "default":
					if((strcmp(current_node->name, "table") == 0) ||
					   (strcmp(current_node->name, "tbody") == 0) ||
					   (strcmp(current_node->name, "tfoot") == 0) ||
				       (strcmp(current_node->name, "thead") == 0) ||
				       (strcmp(current_node->name, "tr") == 0))
					{
						apply_foster_parenting = 1;
					}

					in_body_mode(tk);
				
					apply_foster_parenting = 0;

				}
			}
			break;
		default:
			{
				//Parse error . Process the token using the rules for the "in body " insertion mode , except that
				//if the current node is a table , tbody , tfoot , thead , or tr element, then, whenever a
				//node would be inserted into the current node , it must instead be foster parented .
				if((strcmp(current_node->name, "table") == 0) ||
				   (strcmp(current_node->name, "tbody") == 0) ||
				   (strcmp(current_node->name, "tfoot") == 0) ||
				   (strcmp(current_node->name, "thead") == 0) ||
				   (strcmp(current_node->name, "tr") == 0))
				{
					apply_foster_parenting = 1;
				}

				in_body_mode(tk);
				
				apply_foster_parenting = 0;
   
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void in_table_text_mode(const token *tk)
{
	long i, buf_len;
	text_node *t = NULL;

	if(tk->type == TOKEN_CHARACTER)
	{
		/*
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
		*/
		;
	}
	else
	{	

		//copy text_chunk from file buffer to text_buffer:
		if((text_chunk != NULL) && (text_char_count > 0))
		{
			text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}
		text_chunk = NULL;
		text_char_count = 0;

	
		if(text_buffer != NULL)
		{
	
			buf_len = strlen(text_buffer);
		
			//check and see if foster parenting is to be applied:
			for(i = 0; i < buf_len; i++)
			{
				if((text_buffer[i] != CHARACTER_TABULATION) &&
				   (text_buffer[i] != LINE_FEED) &&
				   (text_buffer[i] != FORM_FEED) &&
				   (text_buffer[i] != CARRIAGE_RETURN) &&
				   (text_buffer[i] != SPACE))
				{
					apply_foster_parenting = 1;
					break;
				}
			}


			t = create_text_node();

			t->text_data = string_n_append(t->text_data, text_buffer, buf_len);
			
		
			/*also need to check if the current_node is a "table", "tbody", "tfoot", "thead", or "tr" */
			if((apply_foster_parenting == 1) && 
			   ((strcmp(current_node->name, "table") == 0) ||
			   (strcmp(current_node->name, "tbody") == 0) ||
			   (strcmp(current_node->name, "tfoot") == 0) ||
			   (strcmp(current_node->name, "thead") == 0) ||
			   (strcmp(current_node->name, "tr") == 0)))
			{	
				add_child_to_foster_parent(o_e_stack, (node *)t);
			}
			else
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t_node = (text_node *)current_node->last_child;
					t_node->text_data = string_n_append(t_node->text_data, text_buffer, buf_len);
				}
				else
				{
					add_child_node(current_node, (node *)t);
				}
			}

			free(text_buffer);
			text_buffer = NULL;


			apply_foster_parenting = 0;
		
		}

		current_mode = original_insertion_mode;
		token_process = REPROCESS;
	}

}


/*------------------------------------------------------------------------------------*/
void in_caption_mode(const token *tk)
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
		element_node *match_node;

		//parse error
		parse_error(UNEXPECTED_START_TAG, line_number);

		//Act as if an end tag with the tag name "caption" had been seen, then, 
		//if that token wasn't ignored, reprocess the current token.
		if(has_element_in_table_scope(o_e_stack, "caption", &match_node) == 1)
		{

			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;
					
				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			pop_elements_up_to(&o_e_stack, "caption");
			current_node = open_element_stack_top(o_e_stack);

			//Clear the list of active formatting elements up to the last marker.
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			current_mode = IN_TABLE;
			token_process = REPROCESS;
		}
		else
		{
			//ignore the token

			//tag token is ignored, there may be more text after the ignored token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;

		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "caption") == 0))
	{
		element_node *match_node;

		if(has_element_in_table_scope(o_e_stack, "caption", &match_node) == 1)
		{
			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;
					
				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			//generate implied end tags
			current_node = generate_implied_end_tags(&o_e_stack, NULL);

			if(strcmp(current_node->name, "caption") != 0)
			{
				//parse error
				parse_error(UNEXPECTED_END_TAG, line_number);
			}

			pop_elements_up_to(&o_e_stack, "caption");
			current_node = open_element_stack_top(o_e_stack);

			//Clear the list of active formatting elements up to the last marker.
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			current_mode = IN_TABLE;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);

			//tag token is ignored, there may be more text after the ignored token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;

		}

	}
	else if((tk->type == TOKEN_END_TAG) &&
		(strcmp(tk->ett.tag_name, "table") == 0))
	{
		element_node *match_node;

		//parse error
		parse_error(UNEXPECTED_END_TAG, line_number);

		//Act as if an end tag with the tag name "caption" had been seen, then, 
		//if that token wasn't ignored, reprocess the current token.

		if(has_element_in_table_scope(o_e_stack, "caption", &match_node) == 1)
		{
			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;
					
				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			pop_elements_up_to(&o_e_stack, "caption");
			current_node = open_element_stack_top(o_e_stack);

			//Clear the list of active formatting elements up to the last marker.
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			current_mode = IN_TABLE;
			token_process = REPROCESS;
		}
		else
		{
			//ignore the token

			//tag token is ignored, there may be more text after the ignored token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;
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
		//parse error, ignore the token
		parse_error(UNEXPECTED_END_TAG, line_number);

		//tag token is ignored, there may be more text after the ignored token
		//copy text_chunk from file buffer to text_buffer:
		if((text_chunk != NULL) && (text_char_count > 0))
		{
			text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}
		text_chunk = NULL;
		text_char_count = 0;
	}
	else
	{
		//Process the token using the rules for the "in body" insertion mode.
		in_body_mode(tk);
	}
}

/*------------------------------------------------------------------------------------*/
void in_column_group_mode(const token *tk)
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

					/*
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						text_node *t = (text_node *)current_node->last_child;
						t->text_data = string_append(t->text_data, tk->cht.ch);
					}
					else
					{
						text_node *t = create_text_node();
						t->text_data = string_append(t->text_data, tk->cht.ch);
						add_child_node(current_node, (node *)t);
					}
					*/
					text_chunk = NULL;
					text_char_count = 0;
				}
				else
				{
					//Act as if an end tag with the tag name "colgroup" had been seen, 
					//and then, if that token wasn't ignored, reprocess the current token.
					if(strcmp(current_node->name, "html") == 0)
					{
						;//ignore the imagined end tag token "colgroup"
					}
					else
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);

						current_mode = IN_TABLE;
						token_process = REPROCESS;
					}
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
			parse_error(UNEXPECTED_DOCTYPE, line_number);

			break;
		case TOKEN_START_TAG:
		    {
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
				else if(strcmp(tk->stt.tag_name, "template") == 0)
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else
				{
					//Act as if an end tag with the tag name "colgroup" had been seen, 
					//and then, if that token wasn't ignored, reprocess the current token.

					if(strcmp(current_node->name, "html") == 0)
					{
						;//ignore the imagined end tag token "colgroup"
					}
					else
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);

						current_mode = IN_TABLE;
						token_process = REPROCESS;
					}
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "colgroup") == 0)
				{
					if(strcmp(current_node->name, "html") == 0)
					{
						//parse error, ignore the token (fragment case)
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);

						current_mode = IN_TABLE;
					}
				}
				else if(strcmp(tk->ett.tag_name, "col") == 0)
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
				else if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else
				{
					//Act as if an end tag with the tag name "colgroup" had been seen, 
					//and then, if that token wasn't ignored, reprocess the current token.
					if(strcmp(current_node->name, "html") == 0)
					{
						;//ignore the imagined end tag token "colgroup"
					}
					else
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);

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
				if(strcmp(current_node->name, "html") == 0)
				{
					;//ignore the imagined end tag token "colgroup"
				}
				else
				{
					open_element_stack_pop(&o_e_stack); 
					current_node = open_element_stack_top(o_e_stack);

					current_mode = IN_TABLE;
					token_process = REPROCESS;
				}
			}
			break;
	}

}

/*------------------------------------------------------------------------------------*/
void in_table_body_mode(const token *tk)
{ 
	if((tk->type == TOKEN_START_TAG) &&
	   (strcmp(tk->stt.tag_name, "tr") == 0))
	{
		element_node *e;
	
		current_node = back_to_table_body_context(&o_e_stack);

		e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
		add_child_node(current_node, (node *)e);
		open_element_stack_push(&o_e_stack, e); 
		current_node = open_element_stack_top(o_e_stack);

		current_mode = IN_ROW;
	}
	else if((tk->type == TOKEN_START_TAG) &&
			((strcmp(tk->stt.tag_name, "th") == 0) ||
			 (strcmp(tk->stt.tag_name, "td") == 0)))
	{
		element_node *e;
		//parse error
		parse_error(UNEXPECTED_START_TAG, line_number);

		//Act as if a start tag with the tag name "tr" had been seen, then reprocess the current token.
		current_node = back_to_table_body_context(&o_e_stack);

		e = create_element_node("tr", NULL, HTML);
		add_child_node(current_node, (node *)e);
		open_element_stack_push(&o_e_stack, e); 
		current_node = open_element_stack_top(o_e_stack);

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
		element_node *match_node;
		if((has_element_in_table_scope(o_e_stack, "tbody", &match_node) == 0) &&
		   (has_element_in_table_scope(o_e_stack, "thead", &match_node) == 0) &&
		   (has_element_in_table_scope(o_e_stack, "tfoot", &match_node) == 0))
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_START_TAG, line_number);
		}
		else
		{
			current_node = back_to_table_body_context(&o_e_stack);
			//current_node should be "tbody", "thead", or"tfoot".

			//Act as if an end tag with the same tag name as the current node("tbody", "tfoot", or "thead") had
			//been seen, then reprocess the current token.
			open_element_stack_pop(&o_e_stack); 
			current_node = open_element_stack_top(o_e_stack);	//back to "table".

			current_mode = IN_TABLE;
			token_process = REPROCESS;

		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "table") == 0))
	{
		element_node *match_node;
		if((has_element_in_table_scope(o_e_stack, "tbody", &match_node) == 0) &&
		   (has_element_in_table_scope(o_e_stack, "thead", &match_node) == 0) &&
		   (has_element_in_table_scope(o_e_stack, "tfoot", &match_node) == 0))
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);
		}
		else
		{
			current_node = back_to_table_body_context(&o_e_stack);
			//current_node should be "tbody", "thead", or"tfoot".

			//Act as if an end tag with the same tag name as the current node("tbody", "tfoot", or "thead") had
			//been seen, then reprocess the current token.
			open_element_stack_pop(&o_e_stack); 
			current_node = open_element_stack_top(o_e_stack);	//back to "table".

			current_mode = IN_TABLE;
			token_process = REPROCESS;

		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			 ((strcmp(tk->ett.tag_name, "tbody") == 0) ||
			  (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			  (strcmp(tk->ett.tag_name, "thead") == 0)))

	{
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1) 
		{
			current_node = back_to_table_body_context(&o_e_stack);
			//current_node should be "tbody", "thead", or"tfoot".

			open_element_stack_pop(&o_e_stack); 
			current_node = open_element_stack_top(o_e_stack);

			current_mode = IN_TABLE;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);
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
		//parse error, ignore the token
		parse_error(UNEXPECTED_END_TAG, line_number);
	}
	else
	{
		//Process the token using the rules for the "in table " insertion mode 
		in_table_mode(tk);
	}
}

/*------------------------------------------------------------------------------------*/
void in_row_mode(const token *tk)
{
	if((tk->type == TOKEN_START_TAG) &&
		((strcmp(tk->stt.tag_name, "th") == 0) ||
		 (strcmp(tk->stt.tag_name, "td") == 0)))
	{
		element_node *e;
	
		current_node = back_to_table_row_context(&o_e_stack);

		e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
		add_child_node(current_node, (node *)e);
		open_element_stack_push(&o_e_stack, e); 
		current_node = open_element_stack_top(o_e_stack);

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
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, "tr", &match_node) == 1) 
		{
			current_node = back_to_table_row_context(&o_e_stack);
			//current_node should be "tr"
			
			open_element_stack_pop(&o_e_stack); 
			current_node = open_element_stack_top(o_e_stack);	// back to parent of "tr" node

			current_mode = IN_TABLE_BODY;

			token_process = REPROCESS;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_START_TAG, line_number);
		}
		
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "tr") == 0))
	{
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1) 
		{
			current_node = back_to_table_row_context(&o_e_stack);
			//current_node should be "tr"
			
			open_element_stack_pop(&o_e_stack); 
			current_node = open_element_stack_top(o_e_stack);	// back to parent of "tr" node

			current_mode = IN_TABLE_BODY;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			(strcmp(tk->ett.tag_name, "table") == 0))
	{
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, "tr", &match_node) == 1) 
		{
			current_node = back_to_table_row_context(&o_e_stack);
			//current_node should be "tr"
			
			open_element_stack_pop(&o_e_stack); 
			current_node = open_element_stack_top(o_e_stack);	// back to parent of "tr" node

			current_mode = IN_TABLE_BODY;

			token_process = REPROCESS;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "tbody") == 0) ||
			 (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			 (strcmp(tk->ett.tag_name, "thead") == 0)))
	{
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1) 
		{
			if(has_element_in_table_scope(o_e_stack, "tr", &match_node) == 1) 
			{
				current_node = back_to_table_row_context(&o_e_stack);
				//current_node should be "tr"
			
				open_element_stack_pop(&o_e_stack); 
				current_node = open_element_stack_top(o_e_stack);	// back to parent of "tr" node

				current_mode = IN_TABLE_BODY;

				token_process = REPROCESS;
			}
			else
			{
				//ignore the imagined end tag token "tr"
			}
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);
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
		//parse error, ignore the token
		parse_error(UNEXPECTED_END_TAG, line_number);
	}
	else
	{
		//Process the token using the rules for the "in table " insertion mode.
		in_table_mode(tk);
	}
 }

/*------------------------------------------------------------------------------------*/
void in_cell_mode(const token *tk)
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
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, "td", &match_node) == 1) 
		{
			//add text node to table cell
			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;

			
				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			//close the cell and reprocess the current token.
			pop_elements_up_to(&o_e_stack, "td");
			current_node = open_element_stack_top(o_e_stack);

			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row ".
			current_mode = IN_ROW;
			token_process = REPROCESS;

		}
		else if(has_element_in_table_scope(o_e_stack, "th", &match_node) == 1) 
		{
			//add text node to table cell
			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;

				
				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			//close the cell and reprocess the current token.
			pop_elements_up_to(&o_e_stack, "th");
			current_node = open_element_stack_top(o_e_stack);

			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row ".
			current_mode = IN_ROW;
			token_process = REPROCESS;
		}
		else
		{
			//parse error, ignore the token.
			parse_error(UNEXPECTED_START_TAG, line_number);

			//tag token is ignored, there may be more text after the ignored token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;

		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "td") == 0) ||
			 (strcmp(tk->ett.tag_name, "th") == 0)))
	{
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1) 
		{
			//add text node to table cell
			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;


				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			//generate implied end tags
			current_node = generate_implied_end_tags(&o_e_stack, NULL);

			if(strcmp(current_node->name, tk->ett.tag_name) != 0)
			{
				//parse error
				parse_error(UNEXPECTED_END_TAG, line_number);
			}

			//close the cell
			pop_elements_up_to(&o_e_stack, tk->ett.tag_name);
			current_node = open_element_stack_top(o_e_stack);

			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row "
			current_mode = IN_ROW;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);

			//tag token is ignored, there may be more text after the ignored token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;
		}
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "body") == 0) ||
			 (strcmp(tk->ett.tag_name, "caption") == 0) ||
			 (strcmp(tk->ett.tag_name, "col") == 0) ||
			 (strcmp(tk->ett.tag_name, "colgroup") == 0) ||
			 (strcmp(tk->ett.tag_name, "html") == 0)))
	{
		//parse error, ignore the token
		parse_error(UNEXPECTED_END_TAG, line_number);

		//tag token is ignored, there may be more text after the ignored token
		//copy text_chunk from file buffer to text_buffer:
		if((text_chunk != NULL) && (text_char_count > 0))
		{
			text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}
		text_chunk = NULL;
		text_char_count = 0;
	}
	else if((tk->type == TOKEN_END_TAG) &&
			((strcmp(tk->ett.tag_name, "table") == 0) ||
			 (strcmp(tk->ett.tag_name, "tbody") == 0) ||
			 (strcmp(tk->ett.tag_name, "tfoot") == 0) ||
			 (strcmp(tk->ett.tag_name, "thead") == 0) ||
			 (strcmp(tk->ett.tag_name, "tr") == 0)))
	{
		element_node *match_node;
		if(has_element_in_table_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1) 
		{
			//add text node to table cell
			if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
			{
				text_node *t = (text_node *)current_node->last_child;

				//copy text_chunk from file buffer to text_buffer:
				if((text_chunk != NULL) && (text_char_count > 0))
				{
					text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
				}
				text_chunk = NULL;
				text_char_count = 0;


				if(text_buffer != NULL)
				{
					t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
					free(text_buffer);
					text_buffer = NULL;
				}
			}

			//close the cell and reprocess the current token.
			if(has_element_in_table_scope(o_e_stack, "td", &match_node) == 1) 
			{
				pop_elements_up_to(&o_e_stack, "td");
				current_node = open_element_stack_top(o_e_stack);
			}
			if(has_element_in_table_scope(o_e_stack, "th", &match_node) == 1) 
			{
				pop_elements_up_to(&o_e_stack, "th");
				current_node = open_element_stack_top(o_e_stack);
			}
		
			//Clear the list of active formatting elements up to the last marker .
			active_formatting_elements = clear_list_up_to_last_marker(active_formatting_elements);
			//Switch the insertion mode to "in row ".
			current_mode = IN_ROW;
			token_process = REPROCESS;
		}
		else
		{
			//parse error, ignore the token
			parse_error(UNEXPECTED_END_TAG, line_number);

			//tag token is ignored, there may be more text after the ignored token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;
		}

	}
	else
	{
		//Process the token using the rules for the "in body" insertion mode
		in_body_mode(tk);
	}
}

/*------------------------------------------------------------------------------------*/
void in_select_mode(const token *tk)
{
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					;
				}
				else
				{
					text_node *t = create_text_node();		//create text node with empty string as data.
					add_child_node(current_node, (node *)t);
				}	
			}
			break;
		case TOKEN_COMMENT:
			{
				comment_node *c;

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);	
			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			parse_error(UNEXPECTED_DOCTYPE, line_number);

			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}
			}
			break;
		case TOKEN_START_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if(strcmp(tk->stt.tag_name, "html") == 0)
				{
					//Process the token using the rules for the "in body " insertion mode .
					in_body_mode(tk);
				}
				else if(strcmp(tk->stt.tag_name, "option") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
	
					if(strcmp(current_node->name, "option") == 0)
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}
					
					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);
				}
				else if(strcmp(tk->stt.tag_name, "optgroup") == 0)
				{
					element_node *e = create_element_node(tk->stt.tag_name, tk->stt.attributes, HTML);
		
					if(strcmp(current_node->name, "option") == 0)
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}

					if(strcmp(current_node->name, "optgroup") == 0)
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}

					add_child_node(current_node, (node *)e);
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);
				}
				else if(strcmp(tk->stt.tag_name, "select") == 0)
				{
					element_node *match_node;
					//parse error . Act as if the token had been an end tag with the tag name "select" instead.
					parse_error(UNEXPECTED_START_TAG, line_number);

					if(has_element_in_select_scope(o_e_stack, "select", &match_node) == 1) 
					{
						pop_elements_up_to(&o_e_stack, "select");
						current_node = open_element_stack_top(o_e_stack);

						//Reset the insertion mode appropriately.
						current_mode = reset_insertion_mode(o_e_stack);
					}
					else
					{
						//ignore the imagined end tag token (fragment case)
					}
				}
				else if((strcmp(tk->stt.tag_name, "input") == 0) ||
					    (strcmp(tk->stt.tag_name, "keygen") == 0) ||
						(strcmp(tk->stt.tag_name, "textarea") == 0))
				{
					element_node *match_node;
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);

					//If the stack of open elementsdoes not have a select element in select scope, ignore the token.
					//Otherwise, act as if an end tag with the tag name "select" had been seen, and reprocess the token.
					if(has_element_in_select_scope(o_e_stack, "select", &match_node) == 1) 
					{
						pop_elements_up_to(&o_e_stack, "select");
						current_node = open_element_stack_top(o_e_stack);

						//Reset the insertion mode appropriately.
						current_mode = reset_insertion_mode(o_e_stack);
						token_process = REPROCESS;
					}
					else
					{
						//ignore the imagined end tag token (fragment case)
					}
				}
				else if((strcmp(tk->stt.tag_name, "script") == 0) ||
					    (strcmp(tk->stt.tag_name, "template") == 0))
				{
					//Process the token using the rules for the "in head " insertion mode
					in_head_mode(tk);
				}
				else
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);

					//assign the text_data back to text_buffer and set text_data to empty string.
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						text_node *t = (text_node *)current_node->last_child;

						text_buffer = t->text_data;
						t->text_data = string_append(NULL, '\0');
					}
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if(strcmp(tk->ett.tag_name, "optgroup") == 0)
				{
					element_node *current_node_parent = (element_node *)current_node->parent;

					if((strcmp(current_node->name, "option") == 0) && 
					   (strcmp(current_node_parent->name, "optgroup") == 0))
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}

					if(strcmp(current_node->name, "optgroup") == 0)
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);

						//assign the text_data back to text_buffer and set text_data to empty string.
						if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
						{
							text_node *t = (text_node *)current_node->last_child;

							text_buffer = t->text_data;
							t->text_data = string_append(NULL, '\0');
						}
					}
				}
				else if(strcmp(tk->ett.tag_name, "option") == 0)
				{
					if(strcmp(current_node->name, "option") == 0)
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);

						//assign the text_data back to text_buffer and set text_data to empty string.
						if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
						{
							text_node *t = (text_node *)current_node->last_child;

							text_buffer = t->text_data;
							t->text_data = string_append(NULL, '\0');
						}
					}

				}
				else if(strcmp(tk->ett.tag_name, "select") == 0)
				{
					element_node *match_node;
					if(has_element_in_select_scope(o_e_stack, "select", &match_node) == 1) 
					{
						pop_elements_up_to(&o_e_stack, "select");
						current_node = open_element_stack_top(o_e_stack);

						//Reset the insertion mode appropriately.
						current_mode = reset_insertion_mode(o_e_stack);
					}
					else
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
				}
				else if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					//Process the token using the rules for the "in head" insertion mode.
					in_head_mode(tk);
				}
				else
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);

					//assign the text_data back to text_buffer and set text_data to empty string.
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						text_node *t = (text_node *)current_node->last_child;

						text_buffer = t->text_data;
						t->text_data = string_append(NULL, '\0');
					}

				}
			}
			break;
		case TOKEN_EOF:
			//stop parsing;
			{
				if(strcmp(current_node->name, "html") != 0)
				{
					//parse error
					parse_error(UNEXPECTED_END_OF_FILE, line_number);
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
void in_select_in_table_mode(const token *tk)
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
		element_node *match_node;

		//parse error . Act as if an end tag with the tag name "select" had been seen, and reprocess the token.
		parse_error(UNEXPECTED_START_TAG, line_number);

		if(has_element_in_select_scope(o_e_stack, "select", &match_node) == 1) 
		{
			pop_elements_up_to(&o_e_stack, "select");
			current_node = open_element_stack_top(o_e_stack);
		}

		//Reset the insertion mode appropriately.
		current_mode = reset_insertion_mode(o_e_stack);
		token_process = REPROCESS;

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
		element_node *match_node;

		//parse error
		parse_error(UNEXPECTED_END_TAG, line_number);

		if(has_element_in_table_scope(o_e_stack, tk->ett.tag_name, &match_node) == 1) 
		{
			if(has_element_in_select_scope(o_e_stack, "select", &match_node) == 1) 
			{
				pop_elements_up_to(&o_e_stack, "select");
				current_node = open_element_stack_top(o_e_stack);
			}

			//Reset the insertion mode appropriately.
			current_mode = reset_insertion_mode(o_e_stack);
			token_process = REPROCESS;
			
		}
		else
		{
			//ignore the token
			//copy text_chunk from file buffer to text_buffer:
			if((text_chunk != NULL) && (text_char_count > 0))
			{
				text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
			}
			text_chunk = NULL;
			text_char_count = 0;
		}
	}
	else
	{
		//Process the token using the rules for the "in select" insertion mode .
		in_select_mode(tk);
	}
			
}

/*------------------------------------------------------------------------------------*/
void in_template_mode(const token *tk)
{
	switch(tk->type)
	{
		case TOKEN_CHARACTER:
			{
				in_body_mode(tk);
			}

			break;
		case TOKEN_MULTI_CHAR:
			{
				in_body_mode(tk);
			}

			break;
		case TOKEN_COMMENT:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;


					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				in_body_mode(tk);
			}

			break;
		case TOKEN_DOCTYPE:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;


					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				in_body_mode(tk);
			}

			break;
		case TOKEN_START_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;


					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if((strcmp(tk->stt.tag_name, "base") == 0) ||
				   (strcmp(tk->stt.tag_name, "basefont") == 0) ||
				   (strcmp(tk->stt.tag_name, "bgsound") == 0) ||
				   (strcmp(tk->stt.tag_name, "link") == 0) ||
				   (strcmp(tk->stt.tag_name, "meta") == 0) ||
				   (strcmp(tk->stt.tag_name, "noframes") == 0) ||
				   (strcmp(tk->stt.tag_name, "script") == 0) ||
				   (strcmp(tk->stt.tag_name, "style") == 0) ||
				   (strcmp(tk->stt.tag_name, "template") == 0) ||
				   (strcmp(tk->stt.tag_name, "title") == 0))
				{
					in_head_mode(tk);
				}
				else if((strcmp(tk->stt.tag_name, "caption") == 0) ||
					    (strcmp(tk->stt.tag_name, "colgroup") == 0) ||
						(strcmp(tk->stt.tag_name, "tbody") == 0) ||
						(strcmp(tk->stt.tag_name, "tfoot") == 0) ||
						(strcmp(tk->stt.tag_name, "thead") == 0))
				{
					mode_stack_pop(&m_stack);
					mode_stack_push(&m_stack, IN_TABLE);
					current_template_insertion_mode = mode_stack_top(m_stack);
					
					current_mode = IN_TABLE;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->stt.tag_name, "col") == 0)
				{
					mode_stack_pop(&m_stack);
					mode_stack_push(&m_stack, IN_COLUMN_GROUP);
					current_template_insertion_mode = mode_stack_top(m_stack);

					current_mode = IN_COLUMN_GROUP;
					token_process = REPROCESS;
				}
				else if(strcmp(tk->stt.tag_name, "tr") == 0)
				{
					mode_stack_pop(&m_stack);
					mode_stack_push(&m_stack, IN_TABLE_BODY);
					current_template_insertion_mode = mode_stack_top(m_stack);
					
					current_mode = IN_TABLE_BODY;
					token_process = REPROCESS;
				}
				else if((strcmp(tk->stt.tag_name, "td") == 0) ||
						(strcmp(tk->stt.tag_name, "th") == 0))
				{
					mode_stack_pop(&m_stack);
					mode_stack_push(&m_stack, IN_ROW);
					current_template_insertion_mode = mode_stack_top(m_stack);
						
					current_mode = IN_ROW;
					token_process = REPROCESS;
				}
				else	//any other start tag
				{
					mode_stack_pop(&m_stack);
					mode_stack_push(&m_stack, IN_BODY);
					current_template_insertion_mode = mode_stack_top(m_stack);
						
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_END_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;


					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if(strcmp(tk->ett.tag_name, "template") == 0)
				{
					in_head_mode(tk);
				}
				else	//any other end tag
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}

			break;
		case TOKEN_EOF:
			{
				if(get_node_by_name(o_e_stack, "template") == NULL)
				{
					;//stop parsing, fragment case
				}
				else
				{
					//parse error
					parse_error(UNEXPECTED_END_OF_FILE, line_number);

					pop_elements_up_to(&o_e_stack, "template");

					active_formatting_elements = 
							clear_list_up_to_last_marker(active_formatting_elements);

					mode_stack_pop(&m_stack);

					current_mode = reset_insertion_mode(o_e_stack);

					token_process = REPROCESS;
				}

			}

			break;
		default:
			break;


	}
}


/*------------------------------------------------------------------------------------*/
void after_body_mode(const token *tk)
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
					parse_error(UNEXPECTED_TEXT, line_number);

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
				element_node *html_node = get_node_by_name(o_e_stack, "html");
				comment_node *c = create_comment_node(tk->cmt.comment);

				if(html_node != NULL)
				{
					add_child_node(html_node, (node *)c);
				}


				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

			}
			break;
		case TOKEN_DOCTYPE:
			//parse error
			//ignore the token
			{
				parse_error(UNEXPECTED_DOCTYPE, line_number);

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}
			}
			break;
		case TOKEN_START_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				if(strcmp(tk->stt.tag_name, "html") == 0)
				{					
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}

			}
			break;
		case TOKEN_END_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

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
					parse_error(UNEXPECTED_END_TAG, line_number);
					//Switch the insertion mode to "in body" and reprocess the token.
					current_mode = IN_BODY;
					token_process = REPROCESS;
				}
			}

			break;
		case TOKEN_EOF:
			//stop parsing;
			break;
		default:
			{
				current_mode = IN_BODY;
				token_process = REPROCESS;
			}
			break;
	}

}



/*------------------------------------------------------------------------------------*/
void in_frameset_mode(const token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				//parse error, ignore the token
				text_chunk = NULL;
				text_char_count = 0;
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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
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
					open_element_stack_push(&o_e_stack, e); 
					current_node = open_element_stack_top(o_e_stack);
				}
				else if(strcmp(tk->stt.tag_name, "frame") == 0)
				{
					//Insert an HTML element for the token. 
					//Immediately pop the current node off the stack of open elements .
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
					//parse error, ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				if(strcmp(tk->ett.tag_name, "frameset") == 0)
				{
					if(strcmp(current_node->name, "html") == 0)
					{
						//parse error, ignore the token
						parse_error(UNEXPECTED_END_TAG, line_number);
					}
					else
					{
						open_element_stack_pop(&o_e_stack); 
						current_node = open_element_stack_top(o_e_stack);
						//If the parser was not originally created as part of the HTML fragment parsing algorithm (fragment case ), 
						//and the current node is no longer a frameset element, then switch the insertion mode to "after frameset".
						if(strcmp(current_node->name, "frameset") != 0)
						{
							current_mode = AFTER_FRAMESET;
						}
					}
				}
				else
				{
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
				}
			}
			break;
		case TOKEN_EOF:
			//stop parsing
			{
				if(strcmp(current_node->name, "html") != 0)
				{
					//parse error
					parse_error(UNEXPECTED_END_OF_FILE, line_number);
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
void after_frameset_mode(const token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				//parse error, ignore the token
				parse_error(UNEXPECTED_TEXT, line_number);
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
			parse_error(UNEXPECTED_DOCTYPE, line_number);
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
					//parse error, ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);
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
					//parse error, ignore the token
					parse_error(UNEXPECTED_END_TAG, line_number);
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
void after_after_body_mode(const token *tk)
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
					parse_error(UNEXPECTED_TEXT, line_number);
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
				add_child_node(doc_root, (node *)c);

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}
			}
			break;
		case TOKEN_DOCTYPE:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}

				in_body_mode(tk);
			}
			break;
		case TOKEN_START_TAG:
			{
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if(strcmp(tk->stt.tag_name, "html") == 0)
				{					
					//Process the token using the rules for the "in body " insertion mode
					in_body_mode(tk);
				}
				else
				{
					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);
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
				parse_error(UNEXPECTED_END_TAG, line_number);

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				current_mode = IN_BODY;
				token_process = REPROCESS;
			}
			break;
		case TOKEN_EOF:
			//stop parsing;
			break;
		default:
			{
				current_mode = IN_BODY;
				token_process = REPROCESS;
			}
			break;
	}
}

/*------------------------------------------------------------------------------------*/
void after_after_frameset_mode(const token *tk)
{ 
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				//parse error, ignore the token
				parse_error(UNEXPECTED_TEXT, line_number);
			}
			break;
		case TOKEN_COMMENT:
			{
				//Append a Comment node to the Document object with the data attribute set to the data given in the comment token.
				comment_node *c = create_comment_node(tk->cmt.comment);
				add_child_node(doc_root, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			{
				//Process the token using the rules for the "in body " insertion mode
				in_body_mode(tk);
			}
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
					//parse error, ignore the token
					parse_error(UNEXPECTED_START_TAG, line_number);
				}
			}
			break;
		case TOKEN_END_TAG:
			{
				//parse error, ignore the token
				parse_error(UNEXPECTED_END_TAG, line_number);
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
/*use this function in non-fragment cases*/
insertion_mode reset_insertion_mode(element_stack *st)
{
	element_node *temp_element;

	while(st != NULL)
	{
		temp_element = open_element_stack_top(st);

		if(strcmp(temp_element->name, "select") == 0)
		{
			return IN_SELECT;
		}
		else if((strcmp(temp_element->name, "td") == 0) ||
				(strcmp(temp_element->name, "th") == 0))
		{
			return IN_CELL;
		}
		else if(strcmp(temp_element->name, "tr") == 0)
		{
			return IN_ROW;
		}
		else if((strcmp(temp_element->name, "tbody") == 0) ||
				(strcmp(temp_element->name, "thead") == 0) ||
				(strcmp(temp_element->name, "tfoot") == 0))
		{
			return IN_TABLE_BODY;
		}
		else if(strcmp(temp_element->name, "caption") == 0)
		{
			return IN_CAPTION;
		}
		else if(strcmp(temp_element->name, "colgroup") == 0)
		{
			return IN_COLUMN_GROUP;
		}
		else if(strcmp(temp_element->name, "table") == 0)
		{
			return IN_TABLE;
		}
		else if(strcmp(temp_element->name, "template") == 0)
		{
			//return current_template_insertion_mode;
			if(m_stack != NULL)
			{
				return mode_stack_top(m_stack);
			}
			else
			{
				return IN_TEMPLATE;
			}
			//hopefully, whenever there is a "template" element in
			//in the stack of open elements, m_stack will never be
			//NULL. But we do not want to risk it. So we test m_stack for NULL to be safe.
		}
		else if(strcmp(temp_element->name, "head") == 0)
		{
			return IN_HEAD;
		}
		else if(strcmp(temp_element->name, "body") == 0)
		{
			return IN_BODY;
		}
		else if(strcmp(temp_element->name, "frameset") == 0)
		{
			return IN_FRAMESET;
		}
		else if(strcmp(temp_element->name, "html") == 0)
		{
			return BEFORE_HEAD;
		}
		else
		{
			;
		}

		st = previous_stack_node(st);
	}

	return IN_BODY;
}

/*------------------------------------------------------------------------------------*/
/*use this function in fragment cases*/
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
	else if(strcmp(context_element_name, "template") == 0)
	{
		mode_stack_push(&m_stack, IN_TEMPLATE);
		current_template_insertion_mode = mode_stack_top(m_stack);

		return current_template_insertion_mode;
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


/*------------------------------------------------------------------------------------*/
void parse_token_in_foreign_content(const token *tk)
{
	switch (tk->type) 
	{
		case TOKEN_CHARACTER:
			{
				if(tk->cht.ch == NULL_CHARACTER)
				{
					//Parse error. Insert a U+FFFD REPLACEMENT CHARACTER character into the current node.
					unsigned char byte_seq[5];

					parse_error(NULL_CHARACTER_IN_TEXT, line_number);

					utf8_byte_sequence(0xFFFD, byte_seq);
					
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						;
					}
					else
					{
						text_node *t = create_text_node();		//create text node with empty string as data.
						add_child_node(current_node, (node *)t);
					}

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;

					//append replacement character U+FFFD to the text_buffer
					text_buffer = string_n_append(text_buffer, byte_seq, strlen(byte_seq));

				}
				else if((tk->cht.ch == CHARACTER_TABULATION) ||
						(tk->cht.ch == LINE_FEED) ||
						(tk->cht.ch == FORM_FEED) ||
						(tk->cht.ch == CARRIAGE_RETURN) ||
						(tk->cht.ch == SPACE))
				{
					//insert the character into the current node
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						;
					}
					else
					{
						text_node *t = create_text_node();		//create text node with empty string as data.
						add_child_node(current_node, (node *)t);
					}

				}
				else
				{
					//Insert the token's character into the current node .
					//Set the frameset-ok flag to "not ok".
					if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
					{
						;
					}
					else
					{
						text_node *t = create_text_node();		//create text node with empty string as data.
						add_child_node(current_node, (node *)t);
					}

				}
			}
			break;
		case TOKEN_COMMENT:
			{
				//Append a Comment node to the current node with the data attribute set to the data given in the comment token.
				comment_node *c;

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}
					
				c = create_comment_node(tk->cmt.comment);
				add_child_node(current_node, (node *)c);
			}
			break;
		case TOKEN_DOCTYPE:
			{
				//parse error, ignore the token
				parse_error(UNEXPECTED_DOCTYPE, line_number);

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}
			}
			break;
		case TOKEN_START_TAG:
		    {
				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if((strcmp(tk->stt.tag_name, "b") == 0) ||
				   (strcmp(tk->stt.tag_name, "big") == 0) ||
				   (strcmp(tk->stt.tag_name, "blockquote") == 0) ||
				   (strcmp(tk->stt.tag_name, "body") == 0) ||
				   (strcmp(tk->stt.tag_name, "br") == 0) ||
				   (strcmp(tk->stt.tag_name, "center") == 0) ||
				   (strcmp(tk->stt.tag_name, "code") == 0) ||
				   (strcmp(tk->stt.tag_name, "dd") == 0) ||
				   (strcmp(tk->stt.tag_name, "div") == 0) ||
				   (strcmp(tk->stt.tag_name, "dl") == 0) ||
				   (strcmp(tk->stt.tag_name, "dt") == 0) ||
				   (strcmp(tk->stt.tag_name, "em") == 0) ||
				   (strcmp(tk->stt.tag_name, "embed") == 0) ||
				   (strcmp(tk->stt.tag_name, "h1") == 0) ||
				   (strcmp(tk->stt.tag_name, "h2") == 0) ||
				   (strcmp(tk->stt.tag_name, "h3") == 0) ||
				   (strcmp(tk->stt.tag_name, "h4") == 0) ||
				   (strcmp(tk->stt.tag_name, "h5") == 0) ||
				   (strcmp(tk->stt.tag_name, "h6") == 0) ||
				   (strcmp(tk->stt.tag_name, "head") == 0) ||
				   (strcmp(tk->stt.tag_name, "hr") == 0) ||
				   (strcmp(tk->stt.tag_name, "i") == 0) ||
				   (strcmp(tk->stt.tag_name, "img") == 0) ||
				   (strcmp(tk->stt.tag_name, "li") == 0) ||
				   (strcmp(tk->stt.tag_name, "listing") == 0) ||
				   (strcmp(tk->stt.tag_name, "menu") == 0) ||
				   (strcmp(tk->stt.tag_name, "meta") == 0) ||
				   (strcmp(tk->stt.tag_name, "nobr") == 0) ||
				   (strcmp(tk->stt.tag_name, "ol") == 0) ||
				   (strcmp(tk->stt.tag_name, "p") == 0) ||
				   (strcmp(tk->stt.tag_name, "pre") == 0) ||
				   (strcmp(tk->stt.tag_name, "ruby") == 0) ||
				   (strcmp(tk->stt.tag_name, "s") == 0) ||
				   (strcmp(tk->stt.tag_name, "small") == 0) ||
				   (strcmp(tk->stt.tag_name, "span") == 0) ||
				   (strcmp(tk->stt.tag_name, "strong") == 0) ||
				   (strcmp(tk->stt.tag_name, "strike") == 0) ||
				   (strcmp(tk->stt.tag_name, "sub") == 0) ||
				   (strcmp(tk->stt.tag_name, "sup") == 0) ||
				   (strcmp(tk->stt.tag_name, "table") == 0) ||
				   (strcmp(tk->stt.tag_name, "tt") == 0) ||
				   (strcmp(tk->stt.tag_name, "u") == 0) ||
				   (strcmp(tk->stt.tag_name, "ul") == 0) ||
				   (strcmp(tk->stt.tag_name, "var") == 0) ||
				   ((strcmp(tk->stt.tag_name, "font") == 0) && 
							((attribute_name_in_list("color", tk->stt.attributes) == 1) || 
							 (attribute_name_in_list("face", tk->stt.attributes) == 1) || 
							 (attribute_name_in_list("size", tk->stt.attributes) == 1))))
				{
					element_stack *stack_node = o_e_stack;
					int use_rules_for_in_body_mode = 0;

					//parse error
					parse_error(UNEXPECTED_START_TAG, line_number);

					if(strcmp(stack_node->e->name, "html") == 0)
					{
						use_rules_for_in_body_mode = 1;
					}
					else
					{
						while(stack_node != NULL)
						{
						
							if(is_mathml_text_integration_point(stack_node->e) ||
							    is_html_integration_point(stack_node->e) ||
							    (stack_node->e->name_space == HTML))
							{
								use_rules_for_in_body_mode = 0;
								break;
							}
							else if(((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "applet") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "caption") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "html") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "table") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "td") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "th") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "marquee") == 0)) ||
									((stack_node->e->name_space == HTML) && (strcmp(stack_node->e->name, "object") == 0)) ||
									((stack_node->e->name_space == MATHML) && (strcmp(stack_node->e->name, "mi") == 0)) ||
									((stack_node->e->name_space == MATHML) && (strcmp(stack_node->e->name, "mo") == 0)) ||
									((stack_node->e->name_space == MATHML) && (strcmp(stack_node->e->name, "mn") == 0)) ||
									((stack_node->e->name_space == MATHML) && (strcmp(stack_node->e->name, "ms") == 0)) ||
									((stack_node->e->name_space == MATHML) && (strcmp(stack_node->e->name, "mtext") == 0)) ||
									((stack_node->e->name_space == MATHML) && (strcmp(stack_node->e->name, "annotation-xml") == 0)) ||
									((stack_node->e->name_space == SVG) && (strcmp(stack_node->e->name, "foreignObject") == 0)) ||
									((stack_node->e->name_space == SVG) && (strcmp(stack_node->e->name, "desc") == 0)) ||
									((stack_node->e->name_space == SVG) && (strcmp(stack_node->e->name, "title") == 0)))
							{
								use_rules_for_in_body_mode = 1;
								break;	
							}
							else
							{
								stack_node = stack_node->tail;
							}
						}
					}

					

					if(use_rules_for_in_body_mode)
					{
						in_body_mode(tk);
					}
					else
					{
						//------------------------------------------------
						//Pop an element from the stack of open elements
						open_element_stack_pop(&o_e_stack);
						current_node = open_element_stack_top(o_e_stack);

						//and then keep popping more elements from the stack of open elements
						//until the current node is a MathML text integration point, an HTML integration point, or an element in the HTML namespace.
						while((is_mathml_text_integration_point(current_node) != 1) && 
							  (is_html_integration_point(current_node) != 1) && 
							  (current_node->name_space != HTML))
						{
							open_element_stack_pop(&o_e_stack);
							current_node = open_element_stack_top(o_e_stack);
						}
					
						//Then, reprocess the token.
						token_process = REPROCESS;

						//------------------------------------------------
					}
				}
				else
				{
					element_node *e;
					unsigned char *token_start_tag_name = tk->stt.tag_name;

					if(((context_node != NULL) && 
						(context_node->name_space == MATHML) && 
						(strcmp(current_node->name, "html") == 0)) 
						|| 
					   (current_node->name_space == MATHML))
					{
						adjust_mathml_attributes(tk->stt.attributes);
					}
					else if(((context_node != NULL) && 
							 (context_node->name_space == SVG) && 
							 (strcmp(current_node->name, "html") == 0)) 
							|| 
						    (current_node->name_space == SVG))
					{
						//tk->stt.tag_name = adjust_svg_start_tag_name(tk->stt.tag_name);
						
						//function will either return an adjusted name, or tk->stt.tag_name itself.
						token_start_tag_name = adjust_svg_start_tag_name(tk->stt.tag_name);

						adjust_svg_attributes(tk->stt.attributes);
					}
					else
					{
						;
					}

					//Adjust foreign attributes for the token.
					adjust_foreign_attributes(tk->stt.attributes);

					//Insert a foreign element for the token, in the same namespace as the adjusted current node or current node
					if((strcmp(current_node->name, "html") == 0) && (context_node != NULL))
					{
						e = create_element_node(token_start_tag_name, tk->stt.attributes, context_node->name_space);
					}
					else
					{
						e = create_element_node(token_start_tag_name, tk->stt.attributes, current_node->name_space);
					}


					add_child_node(current_node, (node *)e);
					
					open_element_stack_push(&o_e_stack, e);
					current_node = open_element_stack_top(o_e_stack);

					//If the token has its self-closing flag set, 
					//pop the current node off the stack of open elements and
					//acknowledge the token's self-closing flag .
					if(tk->stt.self_closing_flag == SET)
					{
						open_element_stack_pop(&o_e_stack);
						current_node = open_element_stack_top(o_e_stack);
					}

					//free the adjusted name returned, only if it is different from original name.
					if(token_start_tag_name != tk->stt.tag_name)
					{
						free(token_start_tag_name);		
					}
				}
			}
			break;
		case TOKEN_END_TAG:
			{

				if((current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
				{
					text_node *t = (text_node *)current_node->last_child;

					//copy text_chunk from file buffer to text_buffer:
					if((text_chunk != NULL) && (text_char_count > 0))
					{
						text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
					}
					text_chunk = NULL;
					text_char_count = 0;
					
					if(text_buffer != NULL)
					{
						t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
						free(text_buffer);
						text_buffer = NULL;
					}
				}


				if(strcmp(tk->ett.tag_name, "script") == 0)
				{
					if((current_node->name_space == SVG) &&
					   (strcmp(current_node->name, "script") == 0))
					{
						open_element_stack_pop(&o_e_stack);
						current_node = open_element_stack_top(o_e_stack);
						//Let the old insertion point have the same value as the current insertion point. 
						//Let the insertion point be just before the next input character .
						//Increment the parser's script nesting level by one. Set the parser pause flag to true.
						//Process the script element according to the SVG rules, if the user agent supports SVG.
					}
				}
				else
				{
					element_stack *temp_stack = o_e_stack;
					element_node *temp_element;
					unsigned char *lowercase_tag_name;
					int i, len;


					while(temp_stack != NULL)
					{
						temp_element = open_element_stack_top(temp_stack);

						lowercase_tag_name = strdup(temp_element->name);
						len = strlen(lowercase_tag_name);
						for(i = 0; i < len; i++)
						{
							lowercase_tag_name[i] = tolower(lowercase_tag_name[i]);
						}

						if(strcmp(lowercase_tag_name, tk->ett.tag_name) == 0)
						{
							pop_ele_up_to(&o_e_stack, temp_element);
							current_node = open_element_stack_top(o_e_stack);
							break;
						}
						else
						{
							//Set node to the previous entry in the stack of open elements .
							//If node is not an element in the HTML namespace, return to the step labeled loop.
							//Otherwise, process the token according to the rules given in the 
							//section corresponding to the current insertion mode in HTML content.
							temp_stack = previous_stack_node(temp_stack);

							if(temp_stack != NULL)
							{
								element_node *next_element;

								next_element = open_element_stack_top(temp_stack);
							
								if(next_element->name_space == HTML)
								{
									pop_ele_up_to(&o_e_stack, temp_element); //pop elements up to, but not including "next_element", 
									current_node = open_element_stack_top(o_e_stack);

									token_process = REPROCESS;
									return;
								}
							}
						}

						free(lowercase_tag_name);

					}
					
				}
			}
			break;
		default:
			{
				;
			}
			break;
	}
}


/*------------------------------------------------------------------------------------*/
void process_trailing_text(void)
{
	if((current_node != NULL) && (current_node->last_child != NULL) && (current_node->last_child->type == TEXT_N))
	{
		text_node *t = (text_node *)current_node->last_child;

		//copy text_chunk from file buffer to text_buffer:
		if((text_chunk != NULL) && (text_char_count > 0))
		{
			text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
		}
		text_chunk = NULL;
		text_char_count = 0;
					
		if(text_buffer != NULL)
		{
			t->text_data = string_n_append(t->text_data, text_buffer, strlen(text_buffer));
			free(text_buffer);
			text_buffer = NULL;
		}
	}

}


/*------------------------------------------------------------------------------------*/
void process_token(token *tk)
{
	if(tk != NULL)
	{
		element_node *curr_node;

			
		do
		{
			token_process = NOT_REPROCESS;

			//pass context_node to parsing_token_in_html_content() when:
			//parsing HTML fragment and the stack has only one element in it.
			if((context_node != NULL) &&											//parsing HTML fragment
			   ((current_node != NULL) && (strcmp(current_node->name, "html") == 0))		//the stack has only one element in it
			  )
			{
				curr_node = context_node;
			}
			else
			{
				curr_node = current_node;
			}


			//parse token in html content or foreign content?
			if(parsing_token_in_html_content(curr_node, tk) == 1)
			{
				tree_cons_state_functions[current_mode](tk);
			}
			else
			{
				parse_token_in_foreign_content(tk);
			}

		}while(token_process == REPROCESS);

		free_token(tk);
	}
}


/*------------------------------------------------------------------------------------*/
void process_eof(void)
{

	switch(current_state)
	{
		case DATA_STATE:
		case RCDATA_STATE:
		case RAWTEXT_STATE:
		case SCRIPT_DATA_STATE:
		case PLAINTEXT_STATE:
			{
				;
			}
			break;
		case TAG_NAME_STATE:
			{
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case CHARACTER_REFERENCE_IN_DATA_STATE:
		case CHARACTER_REFERENCE_IN_RCDATA_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//append the ampersand(&) to the text_buffer
				text_buffer = string_append(text_buffer, AMPERSAND);
				//have a text node created if not yet created
				process_token(create_character_token(AMPERSAND));
			}
			break;
		case RCDATA_LESS_THAN_SIGN_STATE:
		case RAWTEXT_LESS_THAN_SIGN_STATE:
		case SCRIPT_DATA_LESS_THAN_SIGN_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//add the '<' to the text_buffer (text_buffer will be processed by process_trailing_text()).
				text_buffer = string_append(text_buffer, LESS_THAN_SIGN);
				//have a text node created if not yet created
				process_token(create_character_token(LESS_THAN_SIGN));
			}
			break;
		case TAG_OPEN_STATE:
		case SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//add the '<' to the text_buffer (text_buffer will be processed by process_trailing_text()).
				text_buffer = string_append(text_buffer, LESS_THAN_SIGN);
				//have a text node created if not yet created
				process_token(create_character_token(LESS_THAN_SIGN));
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case RCDATA_END_TAG_OPEN_STATE:
		case RAWTEXT_END_TAG_OPEN_STATE:
		case SCRIPT_DATA_END_TAG_OPEN_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//add the '<' to the text_buffer (text_buffer will be processed by process_trailing_text()).
				text_buffer = string_append(text_buffer, LESS_THAN_SIGN);
				//add the '/' to the text_buffer.
				text_buffer = string_append(text_buffer, SOLIDUS);
				//have a text node created if not yet created
				process_token(create_character_token(LESS_THAN_SIGN));
			}
			break;
		case END_TAG_OPEN_STATE:
		case SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//add the '<' to the text_buffer (text_buffer will be processed by process_trailing_text()).
				text_buffer = string_append(text_buffer, LESS_THAN_SIGN);
				//add the '/' to the text_buffer.
				text_buffer = string_append(text_buffer, SOLIDUS);
				//have a text node created if not yet created
				process_token(create_character_token(LESS_THAN_SIGN));
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case RCDATA_END_TAG_NAME_STATE:
		case RAWTEXT_END_TAG_NAME_STATE:
		case SCRIPT_DATA_END_TAG_NAME_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//add the '<' to the text_buffer (text_buffer will be processed by process_trailing_text()).
				text_buffer = string_append(text_buffer, LESS_THAN_SIGN);
				//add the '/' to the text_buffer.
				text_buffer = string_append(text_buffer, SOLIDUS);
				//append the temporary buffer to the text_buffer
				if(temp_buf != NULL)
				{
					long len = strlen(temp_buf);
					text_buffer = string_n_append(text_buffer, temp_buf, len);

					free(temp_buf);
					temp_buf = NULL;
				}
				//have a text node created if not yet created 
				process_token(create_character_token(LESS_THAN_SIGN));

			}
			break;
		case SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE:
			{
				//copy text_chunk from file buffer to text_buffer:
				copy_text_chunk();
				//add the '<' to the text_buffer (text_buffer will be processed by process_trailing_text()).
				text_buffer = string_append(text_buffer, LESS_THAN_SIGN);
				//add the '/' to the text_buffer.
				text_buffer = string_append(text_buffer, SOLIDUS);
				//append the temporary buffer to the text_buffer
				if(temp_buf != NULL)
				{
					long len = strlen(temp_buf);
					text_buffer = string_n_append(text_buffer, temp_buf, len);

					free(temp_buf);
					temp_buf = NULL;
				}
				//have a text node created if not yet created 
				process_token(create_character_token(LESS_THAN_SIGN));

				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);

			}
			break;
		case SCRIPT_DATA_ESCAPE_START_STATE:
		case SCRIPT_DATA_ESCAPE_START_DASH_STATE:
			{
				;
			}
			break;
		case SCRIPT_DATA_ESCAPED_STATE:
		case SCRIPT_DATA_ESCAPED_DASH_STATE:
		case SCRIPT_DATA_ESCAPED_DASH_DASH_STATE:
		case SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE:
		case SCRIPT_DATA_DOUBLE_ESCAPED_STATE:
		case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE:
		case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE:
		case SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE:
		case SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE:
			{
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case BEFORE_ATTRIBUTE_NAME_STATE:
		case ATTRIBUTE_NAME_STATE:
		case AFTER_ATTRIBUTE_NAME_STATE:
		case BEFORE_ATTRIBUTE_VALUE_STATE:
		case ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE:
		case ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE:
		case ATTRIBUTE_VALUE_UNQUOTED_STATE:
		case CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE_STATE:
		case AFTER_ATTRIBUTE_VALUE_QUOTED_STATE:
		case SELF_CLOSING_START_TAG_STATE:
			{
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case BOGUS_COMMENT_STATE:
			{
				curr_token = create_comment_token();
				//include the last character of the buffer in the comment
				curr_token->cmt.comment = string_append(curr_token->cmt.comment, file_buf[buffer_len - 1]);
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case MARKUP_DECLARATION_OPEN_STATE:
			{
				curr_token = create_comment_token();

				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case COMMENT_START_STATE:
		case COMMENT_START_DASH_STATE:
		case COMMENT_STATE:
		case COMMENT_END_DASH_STATE:
		case COMMENT_END_STATE:
		case COMMENT_END_BANG_STATE:
			{
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case DOCTYPE_STATE:
		case BEFORE_DOCTYPE_NAME_STATE:
			{
				process_token(create_doctype_token('\0'));

				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case DOCTYPE_NAME_STATE:
		case AFTER_DOCTYPE_NAME_STATE:
		case AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE:
		case BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE:
		case DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE:
		case DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE:
		case AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE:
		case BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE:
		case AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE:
		case BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE:
		case DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE:
		case DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE:
		case AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE:
			{
				//At this stage, a DOCTYPE token should've been created and it's name should not be empty.
				if((curr_token != NULL) && (curr_token->type  == TOKEN_DOCTYPE))
				{
					process_token(curr_token);
				}
				//parse error
				parse_error(UNEXPECTED_END_OF_FILE, line_number);
			}
			break;
		case BOGUS_DOCTYPE_STATE:
			{
				//At this stage, a DOCTYPE token should've been created and it's name should not be empty.
				if((curr_token != NULL) && (curr_token->type  == TOKEN_DOCTYPE))
				{
					process_token(curr_token);
				}
			}
			break;
		case CDATA_SECTION_STATE:
			{
				;
			}
			break;

		default:
			break;
	}

	process_token(create_eof_token());

	//there may be trailing text, or trailing comment. But not both.
	process_trailing_text();
	process_trailing_comment();
}


/*------------------------------------------------------------------------------------*/
void process_trailing_comment(void)
{
	if((curr_token != NULL) && (curr_token->type == TOKEN_COMMENT))
	{
		comment_node *c = create_comment_node(curr_token->cmt.comment);

		if((current_mode == INITIAL) ||
		   (current_mode == BEFORE_HTML) ||
		   (current_mode == AFTER_AFTER_BODY))
		{
			add_child_node(doc_root, (node *)c);
		}
		else if(current_mode == AFTER_BODY)
		{
			element_node *html_node = get_node_by_name(o_e_stack, "html");

			if(html_node != NULL)
			{
				add_child_node(html_node, (node *)c);
			}
		}
		else
		{
			if(current_node != NULL)
			{
				add_child_node(current_node, (node *)c);
			}
		}
	}
}

/*------------------------------------------------------------------------------------*/
void copy_text_chunk(void)
{
	if((text_chunk != NULL) && (text_char_count > 0))
	{
		text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
	}

	text_chunk = NULL;
	text_char_count = 0;
}
/*------------------------------------------------------------------------------------*/
void append_null_replacement(void)
{
	unsigned char byte_seq[5];

	if((text_chunk != NULL) && (text_char_count > 0))	//there is text before the null character
	{
		//copy the text from file buffer to text_buffer:
		text_buffer = string_n_append(text_buffer, text_chunk, text_char_count);
	}

	text_chunk = NULL;
	text_char_count = 0;

	//append replacement char(0xFFFD) to text_buffer
	utf8_byte_sequence(0xFFFD, byte_seq);

	text_buffer = string_n_append(text_buffer, byte_seq, strlen(byte_seq));

	//this is only useful and necessary if the NULL char is the first char of the text node.
	//if the NULL char is NOT the first char of the text node, the returned tokens will be ignored.
	process_token(create_character_token(byte_seq[0]));

}



/*------------------------------------------------------------------------------------*/
void append_null_replacement_for_attribute_value(void)
{
	unsigned char byte_seq[5];


	//copy attr_value_chunk from file buffer and append it to curr_attr_value:
	if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
	{
		curr_attr_value = string_n_append(curr_attr_value, attr_value_chunk, attr_value_char_count);
	}
	attr_value_chunk = NULL;
	attr_value_char_count = 0;
		

	//Append a U+FFFD REPLACEMENT CHARACTER to the current attribute's value.
	utf8_byte_sequence(0xFFFD, byte_seq);
	curr_attr_value = string_n_append(curr_attr_value, byte_seq, strlen(byte_seq));

}


/*------------------------------------------------------------------------------------*/
int is_in_a_script_data_parsing_state(void)
{
	if((current_state == SCRIPT_DATA_STATE) ||
	   (current_state == SCRIPT_DATA_LESS_THAN_SIGN_STATE) ||
	   (current_state == SCRIPT_DATA_END_TAG_OPEN_STATE) ||
	   (current_state == SCRIPT_DATA_END_TAG_NAME_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPE_START_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPE_START_DASH_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPED_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPED_DASH_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPED_DASH_DASH_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE) ||
	   (current_state == SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE) ||
	   (current_state == SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE) ||
	   (current_state == SCRIPT_DATA_DOUBLE_ESCAPED_STATE) ||
	   (current_state == SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE) ||
	   (current_state == SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE) ||
	   (current_state == SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE) ||
	   (current_state == SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*--------------------------------------------------------------------------------------*/
void parse_error(error_message_type err_msg, unsigned long line_num)
{
	printf("Line: %lu  Error: ", line_num);

	if(err_msg == NULL_CHARACTER_IN_TEXT)
	{
		printf("NULL_CHARACTER_IN_TEXT\n");
	}
	else if(err_msg == UNEXPECTED_END_OF_FILE)
	{
		printf("UNEXPECTED_END_OF_FILE\n");
	}
	else if(err_msg == BOGUS_COMMENT)
	{
		printf("BOGUS_COMMENT\n");
	}
	else if(err_msg == UNESCAPED_LESS_THAN)
	{
		printf("UNESCAPED_LESS_THAN\n");
	}
	else if(err_msg == EMPTY_END_TAG)
	{
		printf("EMPTY_END_TAG\n");
	}
	else if(err_msg == BAD_CHARACTER_FOR_ATTRIBUTE_NAME)
	{
		printf("BAD_CHARACTER_FOR_ATTRIBUTE_NAME\n");
	}
	else if(err_msg == MISSING_ATTRIBUTE_VALUE)
	{
		printf("MISSING_ATTRIBUTE_VALUE\n");
	}
	else if(err_msg == BAD_CHARACTER_FOR_ATTRIBUTE_VALUE)
	{
		printf("BAD_CHARACTER_FOR_ATTRIBUTE_VALUE\n");
	}
	else if(err_msg == MISSING_SPACE_AFTER_ATTRIBUTE_VALUE)
	{
		printf("MISSING_SPACE_AFTER_ATTRIBUTE_VALUE\n");
	}
	else if(err_msg == UNEXPECTED_SOLIDUS)
	{
		printf("UNEXPECTED_SOLIDUS\n");
	}
	else if(err_msg == INVALID_COMMENT)
	{
		printf("INVALID_COMMENT\n");
	}
	else if(err_msg == INVALID_DOCTYPE)
	{
		printf("INVALID_DOCTYPE\n");
	}
	else if(err_msg == BOGUS_DOCTYPE)
	{
		printf("BOGUS_DOCTYPE\n");
	}
	else if(err_msg == MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE)
	{
		printf("MISSING_SPACE_AFTER_KEYWORD_IN_DOCTYPE\n");
	}
	else if(err_msg == MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE)
	{
		printf("MISSING_PUBLIC_IDENTIFIER_IN_DOCTYPE\n");
	}
	else if(err_msg == MISSING_SPACE_AFTER_PUBLIC_IDENTIFIER)
	{
		printf("MISSING_SPACE_AFTER_PUBLIC_IDENTIFIER\n");
	}
	else if(err_msg == MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE)
	{
		printf("MISSING_SYSTEM_IDENTIFIER_IN_DOCTYPE\n");
	}
	else if(err_msg == UNEXPECTED_TEXT)
	{
		printf("UNEXPECTED_TEXT\n");
	}
	else if(err_msg == UNEXPECTED_DOCTYPE)
	{
		printf("UNEXPECTED_DOCTYPE\n");
	}
	else if(err_msg == UNEXPECTED_START_TAG)
	{
		printf("UNEXPECTED_START_TAG\n");
	}
	else if(err_msg == UNEXPECTED_END_TAG)
	{
		printf("UNEXPECTED_END_TAG\n");
	}
	else if(err_msg == DEPRECATED_TAG)
	{
		printf("DEPRECATED_TAG\n");
	}
	else if(err_msg == INVALID_END_TAG)
	{
		printf("INVALID_END_TAG\n");
	}
	else
	{
		;	//unknown error message
	}
}


/*---------------- test function -----------------*/
void print_stack(element_stack *st)
{
	printf("stack-->");
	while(st != NULL)
	{
		printf("%s->", st->e->name);

		st= previous_stack_node(st);
	}
	printf("-|\n");
}

