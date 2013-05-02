// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include "tree.h"





/*--------------------------PARSER FUNCTIONS------------------------------*/
/*------------------------------------------------------------------------*/
/*return 1 if successful, otherwise return 0*/
int html_parse_file(unsigned char *file_name, node **root_ptr, token **doctype_ptr);


void html_parse_memory(unsigned char *file_buffer, long buffer_length, node **root_ptr, token **doctype_ptr);


void html_parse_memory_fragment(unsigned char *string_buffer, long string_length, unsigned char *context, namespace_type context_namespace, node **root_ptr);

/*------------------------------------------------------------------------*/