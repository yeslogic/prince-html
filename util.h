#include "char_ref.h"




/*pass in char[len] as str, len should be 5 or greater*/
void utf8_byte_sequence(unsigned int code_point, unsigned char *str);

/*if char_to_check is disallowed and replacement_char found, return 1, otherwise return 0*/
int replacement_for_disallowed_char(unsigned int char_to_check, unsigned int *replacement_char);

/*returns pointer to extended block if successful, 
  or NULL if unsuccessful(in this case, str is unaltered, 
  so is any previously allocated block str maybe pointing to)*/
unsigned char *string_append(unsigned char *str, unsigned char ch);

/*returns pointer to buffer holding entire document, or NULL if unsuccessful*/
unsigned char *read_file(unsigned char *file_name, long *length);

/*returns 1 if ch is a hex digit, otherwise return 0*/
int is_hex_digit(unsigned char ch);

/*returns 1 if ch is a decimal digit, otherwise return 0*/
int is_dec_digit(unsigned char ch);


/*chars_consumed is the number of chars consumed after the '&', for the char reference.*/
/*chars consumed might not be up to an ';', even if there is one.*/
/*when the character reference is for an attribute, set is_attr to 1, otherwise set it to 0*/
/*after calling this function, the dynamically allocated memory must be free'd*/
unsigned char *html_character_reference(unsigned char *ch, int is_attr, int has_additional_allowed_char, 
										unsigned char additional_allowed_char, int *chars_consumed,
										long curr_buf_index, long buf_len);