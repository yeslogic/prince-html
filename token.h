// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#ifndef TOKEN_H
#define TOKEN_H


typedef struct attribute_list_s attribute_list;
typedef struct doctype_token_s doctype_token;
typedef struct start_tag_token_s start_tag_token;
typedef struct end_tag_token_s end_tag_token;
typedef struct comment_token_s comment_token;
typedef struct character_token_s character_token;
typedef struct token_s token;
typedef struct token_list_s token_list;



typedef enum {
	TOKEN_DOCTYPE,
	TOKEN_START_TAG,
	TOKEN_END_TAG,
	TOKEN_COMMENT,
	TOKEN_CHARACTER,
	TOKEN_EOF
} token_type;

typedef enum {
	SET,
	UNSET
} self_closing_flag_type;

typedef enum {
	ON,
	OFF
} force_quirks_flag_type;


typedef enum {
	HTML,
	MATHML,
	SVG,
	XLINK,
	XML,
	XMLNS,
	DEFAULT
} namespace_type;

/***********************************************/
struct attribute_list_s {
	unsigned char *name;
	unsigned char *value;
	namespace_type attr_ns;
	attribute_list *tail;
};


struct doctype_token_s {
	unsigned char *doctype_name;
	unsigned char *doctype_public_identifier;
	unsigned char *doctype_system_identifier;
	force_quirks_flag_type force_quirks_flag;
};

struct start_tag_token_s {
	unsigned char *tag_name;
	self_closing_flag_type self_closing_flag;
	attribute_list *attributes;
};

struct end_tag_token_s {
	unsigned char *tag_name;
};

struct comment_token_s {
	unsigned char *comment;
};

struct character_token_s {
	unsigned char ch;
};

struct token_s {
	token_type type;
	doctype_token dt;
	start_tag_token stt;
	end_tag_token ett;
	comment_token cmt;
	character_token cht;
};

struct token_list_s {
	token *tk;
	token_list *tail;
};


/***********************************************/	
token *create_doctype_token(unsigned char ch);
token *create_start_tag_token(unsigned char ch);
token *create_end_tag_token(unsigned char ch);
token *create_comment_token(void);
token *create_character_token(unsigned char ch);
token *create_eof_token(void);

void free_token(token *tk);
void free_attributes(attribute_list *list);

token *html_token_list_get_first_token(token_list *tk_list);
token_list *html_token_list_get_tail(token_list *tk_list);

attribute_list *html_attribute_list_cons(unsigned char *name, 
										 unsigned char *value, 
										 namespace_type attr_name_space,
										 attribute_list *attr_list);

int attr_list_comp(attribute_list *attr1, attribute_list *attr2);
int attribute_in_list(unsigned char *name, unsigned char *value, attribute_list *list);
int attribute_name_in_list(unsigned char *attr_name, attribute_list *list);
unsigned char *get_attribute_value(unsigned char *attr_name, attribute_list *list);
attribute_list *remove_attribute(unsigned char *attr_name, attribute_list *list);

token_list *html_token_list_nil(void);
token_list *html_token_list_cons(token *tk, token_list *tail);
token_list *html_token_list_reverse(token_list *tk_list);

void html_print_attribute_list(attribute_list *attributes);
void html_print_force_quirks_flag(force_quirks_flag_type flag);
void html_print_self_closing_flag(self_closing_flag_type flag);
void html_print_token(token *tk);
void html_print_token_list(token_list *tk_list);


#endif

