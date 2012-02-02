#include "tree.h"





/*--------------------------PARSER FUNCTIONS------------------------------*/
/*------------------------------------------------------------------------*/
/*return 1 if successful, otherwise return 0*/
int html_parse_file(unsigned char *file_name, node **root_ptr, token **doctype_ptr);


void html_parse_memory(unsigned char *file_buffer, long buffer_length, node **root_ptr, token **doctype_ptr);


void html_parse_memory_fragment(unsigned char *string_buffer, long string_length, unsigned char *context, node **root_ptr);

/*------------------------------------------------------------------------*/