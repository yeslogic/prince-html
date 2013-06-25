// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <iconv.h>

#include "util.h"
#include "token.h"
#include "encode.h"

#ifdef WIN32
#define strcasecmp stricmp
#endif


int get_attribute(unsigned char *file_buffer, long buffer_length, long *buf_index,  
				   unsigned char **attr_name, unsigned char **attr_value);
unsigned char *get_charset_from_content(unsigned char *attr_value);


int get_encoding_length(unsigned char leading_byte);
int is_valid_utf8_trailing_byte(unsigned char trailing_byte);
int get_utf8_code_point(unsigned char *encoding_sequence, unsigned long remaining_chars, unsigned int *code_point);
unsigned int get_code_point_encoding_1(unsigned char *encoding_sequence);
unsigned int get_code_point_encoding_2(unsigned char *encoding_sequence);
unsigned int get_code_point_encoding_3(unsigned char *encoding_sequence);
unsigned int get_code_point_encoding_4(unsigned char *encoding_sequence);
unsigned char *append_replacement_char(unsigned char *buffer, long buf_len, int *char_len);

int is_utf8_5_byte_encoding(unsigned char *encoding_sequence, unsigned long remaining_chars);
int is_utf8_6_byte_encoding(unsigned char *encoding_sequence, unsigned long remaining_chars);
int has_a_valid_trailing_byte(unsigned char *encoding_sequence, unsigned long remaining_chars);
int has_only_one_valid_trailing_byte(unsigned char *encoding_sequence, unsigned long remaining_chars);
int has_only_two_valid_trailing_bytes(unsigned char *encoding_sequence, unsigned long remaining_chars);
int has_only_three_valid_trailing_bytes(unsigned char *encoding_sequence, unsigned long remaining_chars);
int has_only_four_valid_trailing_bytes(unsigned char *encoding_sequence, unsigned long remaining_chars);

unsigned char *string_n_append_2(unsigned char *char_array_1, unsigned char *char_array_2, long len1, long len2);


int convert_to_utf8(unsigned char *input_buffer, long input_buffer_length, unsigned char *from_encoding, 
					unsigned char **output_buffer, long *output_length);

/*--------------------------------------------------------------------*/
/*file_name is the input file.
  returns NULL if the file cannnot be read into file_buffer.
  Otherwise returns the buffer with a verified, correct version of UTF-8 encoded file*/
unsigned char *verify_file_utf8_encoding(unsigned char *file_name, long *out_buf_len)
{
	unsigned char *file_buffer;
	long buffer_length;

	if((file_buffer = read_file(file_name, &buffer_length)) == NULL)
	{
		return NULL;
	}
	else
	{
		return verify_utf8_encoding(file_buffer, buffer_length, out_buf_len);
	}
}

/*--------------------------------------------------------------------*/
/*file_name is the input file.
  returns a string representing the character encoding, or NULL if no character encoding can be found*/
unsigned char *get_file_encoding(unsigned char *file_name)
{
	unsigned char *file_buffer;
	long buffer_length;

	if((file_buffer = read_file(file_name, &buffer_length)) == NULL)
	{
		return NULL;
	}
	
	return get_encoding(file_buffer, buffer_length);
}

/*--------------------------------------------------------------------*/
/*returns -1 if file cannot be read into buffer, 1 if encoding is verified to be utf-8, 0 otherwise*/
int check_file_utf8_encoding(unsigned char *file_name)
{
	unsigned char *file_buffer;
	long buffer_length;

	if((file_buffer = read_file(file_name, &buffer_length)) == NULL)
	{
		return -1;
	}
	
	return check_utf8_encoding(file_buffer, buffer_length);
}
/*--------------------------------------------------------------------*/
/*returns a string representing the character encoding, or NULL if no character encoding can be found*/
unsigned char *get_encoding(unsigned char *file_buffer, long buffer_length)
{
	long buf_index;
	
	//check utf-8 Byte Order Mark:
	if((buffer_length >= 3) && 
	   ((file_buffer[0] == 0xEF) && (file_buffer[1] == 0xBB) && (file_buffer[2] == 0xBF)))
	{
		return strdup("utf-8");
	}

	buf_index = 0;
	while(buf_index < buffer_length)
	{
		//A sequence of bytes starting with "<!--" and ending with "-->"
		if(((buf_index + 4) < buffer_length) &&
		   ((file_buffer[buf_index] == LESS_THAN_SIGN) &&
			(file_buffer[buf_index + 1] == EXCLAMATION_MARK) &&
			(file_buffer[buf_index + 2] == HYPHEN_MINUS) &&
			(file_buffer[buf_index + 3] == HYPHEN_MINUS)))
		{

			//move past "<!--"
			buf_index = buf_index + 4;

			while(buf_index < buffer_length)
			{		
				if((file_buffer[buf_index] == GREATER_THAN_SIGN) &&
					   (file_buffer[buf_index - 1] == HYPHEN_MINUS) &&
					   (file_buffer[buf_index - 2] == HYPHEN_MINUS))
				{
					break;
				}
					
				buf_index += 1;
			}
			//buf_index now points at ">" sign.
		}
		//A sequence of bytes starting with: 
		//case-insensitive ASCII '<meta' followed by a space character or slash
		else if(((buf_index + 6) < buffer_length) 
				&& 
			    ((file_buffer[buf_index] == LESS_THAN_SIGN) &&
			     ((file_buffer[buf_index + 1] == CAPITAL_M) || (file_buffer[buf_index + 1] == SMALL_M)) &&
			     ((file_buffer[buf_index + 2] == CAPITAL_E) || (file_buffer[buf_index + 2] == SMALL_E)) &&
			     ((file_buffer[buf_index + 3] == CAPITAL_T) || (file_buffer[buf_index + 3] == SMALL_T)) &&
			     ((file_buffer[buf_index + 4] == CAPITAL_A) || (file_buffer[buf_index + 4] == SMALL_A)) &&
			     ((file_buffer[buf_index + 5] == CHARACTER_TABULATION) || 
			      (file_buffer[buf_index + 5] == LINE_FEED) || 
			      (file_buffer[buf_index + 5] == FORM_FEED) || 
				  (file_buffer[buf_index + 5] == CARRIAGE_RETURN) || 
				  (file_buffer[buf_index + 5] == SPACE) || 
				  (file_buffer[buf_index + 5] == SOLIDUS))
				))
		{
			
			unsigned int got_pragma;
			int need_pragma;	//(-1 means NULL, 0 means false, 1 means true)
			unsigned  char *charset, *attr_name, *attr_value;
			attribute_list *attr_list;

			buf_index += 5;		//advance pointer to point at where space character or slash is
				
			got_pragma = 0;
			need_pragma = -1;
			charset = NULL;
			attr_list = NULL;
			attr_name = NULL;
			attr_value = NULL;

			while(buf_index < buffer_length)
			{
				get_attribute(file_buffer, buffer_length, &buf_index, &attr_name, &attr_value);

				if((attr_name == NULL) || (attr_value == NULL))	//no attribute found.
				{
					break;	//jump to "processing"
				}
				else
				{
					if(attribute_name_in_list(attr_name, attr_list))
					{
						; //go back to the loop and try to get an attribute.
					}
					else
					{
						attr_list = html_attribute_list_cons(attr_name, attr_value, HTML, attr_list);

						if((strcmp(attr_name, "http-equiv") == 0) && (strcasecmp(attr_value, "content-type") == 0))
						{
							got_pragma = 1;
						}
						else if(strcmp(attr_name, "content") == 0)
						{
							if(charset == NULL)
							{
								charset = get_charset_from_content(attr_value);
							}

							if(charset != NULL)		//there is a character encoding returned.
							{
								need_pragma = 1;
							}
						}
						else if(strcmp(attr_name, "charset") == 0)
						{
							charset = strdup(attr_value);
							need_pragma = 0;	
						}
						else
						{
							;	//attr_name is not one of the three above.
								//go back to the loop and try to get an attribute.
						}
					}
					//attr_name and attr_value should be freed here.
					free(attr_name);
					free(attr_value);
					attr_name = NULL;
					attr_value = NULL;
				}
				//do not need to increment buf_index here, as the get_attribute() function will do that.
				//will break out of loop when no attribute is found.
			}
			free_attributes(attr_list);

			//processing:
			if(need_pragma == -1)
			{
				; //proceed to next byte in buffer
			}
			else if((need_pragma == 1) && (got_pragma == 0))
			{
				free(charset);
				charset = NULL;
				//proceed to next byte in buffer
			}
			else
			{
				return charset;
			}


		}
		//A sequence of bytes starting with a 0x3C byte (ASCII <), optionally a 0x2F byte (ASCII /),
		//and finally a byte in the range 0x41-0x5A or 0x61-0x7A (an ASCII letter)
		else if(((buf_index + 2) < buffer_length) 
			    &&
				(((file_buffer[buf_index] == LESS_THAN_SIGN) 
				  && 
				  (file_buffer[buf_index + 1] == SOLIDUS) 
				  && 
				  (((file_buffer[buf_index + 2] >= CAPITAL_A) && (file_buffer[buf_index + 2] <= CAPITAL_Z)) ||
				   ((file_buffer[buf_index + 2] >= SMALL_A) && (file_buffer[buf_index + 2] <= SMALL_Z)))) 
				 ||
			     ((file_buffer[buf_index] == LESS_THAN_SIGN) 
				  && 
				  (((file_buffer[buf_index + 1] >= CAPITAL_A) && (file_buffer[buf_index + 1] <= CAPITAL_Z)) || 
				   ((file_buffer[buf_index + 1] >= SMALL_A) && (file_buffer[buf_index + 1] <= SMALL_Z))))
				))
		{
			
			while(buf_index < buffer_length)
			{
				if((file_buffer[buf_index] == CHARACTER_TABULATION) || 
						(file_buffer[buf_index] == LINE_FEED) || 
						(file_buffer[buf_index] == FORM_FEED) || 
						(file_buffer[buf_index] == CARRIAGE_RETURN) || 
						(file_buffer[buf_index] == SPACE) ||
						(file_buffer[buf_index] == GREATER_THAN_SIGN))
				{
					unsigned char *attr_name = NULL, *attr_value = NULL;
					//Repeatedly get an attribute until no further attributes can be found, 
					//then jump to the step below labeled next byte.
					
					while(buf_index < buffer_length)
					{
						if(get_attribute(file_buffer, buffer_length, &buf_index, &attr_name, &attr_value) == 0)
						{
							break;
						}
						free(attr_name);
						free(attr_value);
						attr_name = NULL;
						attr_value = NULL;
					}
					break;
				}
				buf_index += 1;
			}
		}
		//A sequence of bytes starting with: 0x3C 0x21 (ASCII '<!')
		//A sequence of bytes starting with: 0x3C 0x2F (ASCII '</')
		//A sequence of bytes starting with: 0x3C 0x3F (ASCII '<?')
		else if(((buf_index + 1) < buffer_length) 
			    && 
				   ((file_buffer[buf_index] == LESS_THAN_SIGN) 
				    &&
			        ((file_buffer[buf_index + 1] == EXCLAMATION_MARK) || 
				     (file_buffer[buf_index + 1] == SOLIDUS) ||
				     (file_buffer[buf_index + 1] == QUESTION_MARK)
					)
				   )
			   )
		{
			while(buf_index < buffer_length)
			{
				if(file_buffer[buf_index] == GREATER_THAN_SIGN)
				{
					break;
				}
				buf_index += 1;
			}

			
		}
		//Any other byte:	do nothing;
		else
		{
			;
		}


		//next byte
		buf_index += 1;
	}

	return NULL;
}


/*-------------------------------------------------------------------*/
/*returns 1 if an attribute is found, *attr_name and *attr_value point to attribute name and value respectively. 
  Returns 0 if no attribute is found, *attr_name and *attr_value remain unchanged*/
int get_attribute(unsigned char *file_buffer, long buffer_length, long *buf_index, 
				   unsigned char **attr_name, unsigned char **attr_value)
{
	unsigned char *attr_value_chunk = NULL;
	long attr_value_char_count = 0;


	while(*buf_index < buffer_length)
	{
		if((file_buffer[*buf_index] == CHARACTER_TABULATION) || 
		   (file_buffer[*buf_index] == LINE_FEED) || 
		   (file_buffer[*buf_index] == FORM_FEED) || 
		   (file_buffer[*buf_index] == CARRIAGE_RETURN) || 
		   (file_buffer[*buf_index] == SPACE) || 
		   (file_buffer[*buf_index] == SOLIDUS))
		{
			;
		}
		else if(file_buffer[*buf_index] == GREATER_THAN_SIGN)
		{
			return 0;		//no attribute found
		}
		else
		{
			//attempt to get the attribute name and value

			*attr_name = string_append(NULL, '\0');
			*attr_value = string_append(NULL, '\0');

			while(*buf_index < buffer_length)
			{
				if((file_buffer[*buf_index] == EQUALS_SIGN) && (strlen(*attr_name) > 0))
				{
					*buf_index += 1;
					break; //jump to step "Value"
					
				}
				else if((file_buffer[*buf_index] == CHARACTER_TABULATION) || 
						(file_buffer[*buf_index] == LINE_FEED) || 
						(file_buffer[*buf_index] == FORM_FEED) || 
						(file_buffer[*buf_index] == CARRIAGE_RETURN) || 
						(file_buffer[*buf_index] == SPACE))
				{
					//space character seen while getting attribute name
					while(*buf_index < buffer_length)
					{
						if((file_buffer[*buf_index] == CHARACTER_TABULATION) || 
						   (file_buffer[*buf_index] == LINE_FEED) || 
						   (file_buffer[*buf_index] == FORM_FEED) || 
						   (file_buffer[*buf_index] == CARRIAGE_RETURN) || 
						   (file_buffer[*buf_index] == SPACE))
						{
							*buf_index += 1;	//skip any space characters
						}
						else
						{
							break;
						}
					}
					//current byte is not a white space
					if((*buf_index < buffer_length) && (file_buffer[*buf_index] != EQUALS_SIGN))
					{	
						return 1;		//attribute name is *attr_name, value is the empty string.
					}
					else
					{
						*buf_index -= 1;	//the end of the current loop will increment *buf_index
					}
				}
				else if((file_buffer[*buf_index] == SOLIDUS) ||
						(file_buffer[*buf_index] == GREATER_THAN_SIGN))
				{
					return 1;		//attribute name is *attr_name, attribute value is the empty string.
				}
				else if((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z))
				{
					unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;

					*attr_name = string_append(*attr_name, c);
				}
				else
				{
					unsigned char c = file_buffer[*buf_index];

					*attr_name = string_append(*attr_name, c);
				}
					
				*buf_index += 1;
			}

			//step: Value
			while(*buf_index < buffer_length)
			{
				if((file_buffer[*buf_index] == CHARACTER_TABULATION) || 
				   (file_buffer[*buf_index] == LINE_FEED) || 
				   (file_buffer[*buf_index] == FORM_FEED) || 
				   (file_buffer[*buf_index] == CARRIAGE_RETURN) || 
				   (file_buffer[*buf_index] == SPACE))
				{
					*buf_index += 1;	//skip any space characters
				}
				else
				{
					break;	//current char is not a space character
				}
			}

			//step 10. Process the byte at position as follows:
			if((*buf_index < buffer_length) && 
					((file_buffer[*buf_index] == QUOTATION_MARK) ||
					 (file_buffer[*buf_index] == APOSTROPHE)))
			{
				unsigned char b = file_buffer[*buf_index];

				*buf_index += 1;
				//Quote loop
				while(*buf_index < buffer_length)
				{
					if(file_buffer[*buf_index] == b)
					{
						*buf_index += 1;

						if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
						{
							*attr_value = string_n_append(*attr_value, attr_value_chunk, attr_value_char_count);
						}
						attr_value_chunk = NULL;
						attr_value_char_count = 0;

						return 1;		//attribute name is *attr_name, value is *attr_value.
					}
					/* do not convert upper-case to lower-case
					else if((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z))
					{
						unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;
						*attr_value = string_append(*attr_value, c);
					}
					*/
					else
					{
						//unsigned char c = file_buffer[*buf_index];
						//*attr_value = string_append(*attr_value, c);
						if(attr_value_char_count == 0)
						{
							attr_value_chunk = &file_buffer[*buf_index];
							attr_value_char_count = 1;
						}
						else
						{
							attr_value_char_count += 1;
						}
					}

					*buf_index += 1;
				}
			}
			else if((*buf_index < buffer_length) && (file_buffer[*buf_index] == GREATER_THAN_SIGN))
			{
				return 1;  //attribute name is *attr_name, value is the empty string.
			}
			/* do not convert upper-case to lower-case
			else if((*buf_index < buffer_length) && ((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z)))
			{
				unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;
				*attr_value = string_append(*attr_value, c);
				*buf_index += 1;
			}
			*/
			else
			{
				if(*buf_index < buffer_length)
				{
					//unsigned char c = file_buffer[*buf_index];
					//*attr_value = string_append(*attr_value, c);

					//start the attr_value_chunk
					attr_value_chunk = &file_buffer[*buf_index];
					attr_value_char_count = 1;

					*buf_index += 1;
				}
			}

			//Step 11. Process the byte at position as follows:
			while(*buf_index < buffer_length)
			{
				if((file_buffer[*buf_index] == CHARACTER_TABULATION) || 
				   (file_buffer[*buf_index] == LINE_FEED) || 
				   (file_buffer[*buf_index] == FORM_FEED) || 
				   (file_buffer[*buf_index] == CARRIAGE_RETURN) || 
				   (file_buffer[*buf_index] == SPACE) ||
				   (file_buffer[*buf_index] == GREATER_THAN_SIGN))
				{
					if((attr_value_chunk != NULL) && (attr_value_char_count > 0))
					{
						*attr_value = string_n_append(*attr_value, attr_value_chunk, attr_value_char_count);
					}	
					attr_value_chunk = NULL;
					attr_value_char_count = 0;

					return 1;  //attribute name is *attr_name, value is *attr_value.
				}
				/* do not convert upper-case to lower-case
				else if((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z))
				{
					unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;
					*attr_value = string_append(*attr_value, c);
				}
				*/
				else
				{
					//unsigned char c = file_buffer[*buf_index];
					//*attr_value = string_append(*attr_value, c);

					//if we are ever here, we must have come from the "else" branch of the "if" statement of Step 10.
					attr_value_char_count += 1;
				}

				*buf_index += 1;
			}
		}
		*buf_index += 1;
	}
	
	free(*attr_name);
	free(*attr_value);
	*attr_name = NULL;
	*attr_value = NULL;

	return 0;
}


/*-------------------------------------------------------------------*/
/*returns a string representing a character encoding, or NULL if no character encoding found*/
unsigned char *get_charset_from_content(unsigned char *attr_value)
{
	int len, char_pos;

	assert(attr_value != NULL);

	len = strlen(attr_value);

	char_pos = 0;
	while(char_pos < len)
	{
		if((char_pos + 7) < len)
		{
			if(((attr_value[char_pos] == 'c') || (attr_value[char_pos] == 'C')) &&
			   ((attr_value[char_pos + 1] == 'h') || (attr_value[char_pos + 1] == 'H')) &&
			   ((attr_value[char_pos + 2] == 'a') || (attr_value[char_pos + 2] == 'A')) &&
			   ((attr_value[char_pos + 3] == 'r') || (attr_value[char_pos + 3] == 'R')) &&
			   ((attr_value[char_pos + 4] == 's') || (attr_value[char_pos + 4] == 'S')) &&
			   ((attr_value[char_pos + 5] == 'e') || (attr_value[char_pos + 5] == 'E')) &&
			   ((attr_value[char_pos + 6] == 't') || (attr_value[char_pos + 6] == 'T')))
			{
				char_pos += 7;

				//skip space characters
				while(char_pos < len)
				{
					if((attr_value[char_pos] == CHARACTER_TABULATION) || 
					   (attr_value[char_pos] == LINE_FEED) || 
				       (attr_value[char_pos] == FORM_FEED) || 
				       (attr_value[char_pos] == CARRIAGE_RETURN) || 
				       (attr_value[char_pos] == SPACE))
					{
						char_pos += 1;
					}
					else
					{
						break;
					}
				}

				//current character is not a space character, check if it is an "=" sign
				if((char_pos < len) && (attr_value[char_pos] != EQUALS_SIGN))			
				{
					//move char_pos to point to the previous character and continue to loop (step: Next byte)
					char_pos -= 1;
				}
				else
				{
					//skip any space characters after the "=" sign
					char_pos += 1;
					if(char_pos < len) //check if there are any more characters after the "=" sign
					{
						//skip space characters
						while(char_pos < len)
						{
							if((attr_value[char_pos] == CHARACTER_TABULATION) || 
							   (attr_value[char_pos] == LINE_FEED) || 
							   (attr_value[char_pos] == FORM_FEED) || 
							   (attr_value[char_pos] == CARRIAGE_RETURN) || 
							   (attr_value[char_pos] == SPACE))
							{
								char_pos += 1;
							}
							else
							{
								break;
							}
						}

						//check if current character is a double quote or single quote:
						if((char_pos < len) && 
							  ((attr_value[char_pos] == QUOTATION_MARK) || (attr_value[char_pos] == APOSTROPHE)))
						{
							unsigned char b = attr_value[char_pos];
							
							char_pos += 1;	//move past the double quote or single quote
							if(char_pos < len)
							{
								unsigned char *encoding;

								encoding = NULL;
								while(char_pos < len)
								{
									if(attr_value[char_pos] == b)  //there is a matching double quote or single quote
									{
										return encoding;
									}
								
									encoding = string_append(encoding, attr_value[char_pos]);
									char_pos += 1;
								}
								free(encoding);
							}
							else
							{
								return NULL;	//no more character
							}
							
						}
						else	//encoding value is unquoted, return the string from the current character up until 
								//first space, or first semicolon, or the end of the input string.
						{
							unsigned char *encoding = NULL;

							
							while(char_pos < len)
							{
								if((attr_value[char_pos] == CHARACTER_TABULATION) || 
								   (attr_value[char_pos] == LINE_FEED) || 
							       (attr_value[char_pos] == FORM_FEED) || 
							       (attr_value[char_pos] == CARRIAGE_RETURN) ||
								   (attr_value[char_pos] == SPACE) || 
								   (attr_value[char_pos] == SEMICOLON))
								{
									return encoding;
								}

								encoding = string_append(encoding, attr_value[char_pos]);

								char_pos += 1;
							}

							return encoding;
						}
					}
					else
					{
						return NULL;  //no more characters
					}
				}

			}

		}
		//Next byte:
		char_pos += 1;
	}

	return NULL;
}


/*--------------------------------------------------------------------*/
/*returns 1 if encoding is utf-8, 0 if encoding is not utf-8*/
int check_utf8_encoding(unsigned char *file_buffer, long buffer_length)
{
	long buf_index;

	
	buf_index = 0;
	while(buf_index < buffer_length)
	{
		unsigned char c = file_buffer[buf_index];

		if((c & 0x80) == 0x00)		//the byte is 0xxx xxxx
		{
			;
		}
		else if((c & 0xC0) == 0x80)	 //the byte is 10xx xxxx
		{
			return 0;
		}
		else if((c & 0xE0) == 0xC0)	 //the byte is 110x xxxx
		{
			//then check the next byte
			if((buf_index + 1) < buffer_length)
			{
				unsigned char nc = file_buffer[buf_index + 1];

				if((nc & 0xC0) == 0x80)	//the next byte is 10xx xxxx
				{
					return 1;	//should be utf-8
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0; //end of buffer, no trailing bytes.
			}
		}
		else if((c & 0xF0) == 0xE0)	//the byte is 1110 xxxx
		{
			//then check the next two bytes
			if((buf_index + 2) < buffer_length)
			{
				unsigned char nc1, nc2;

				nc1 = file_buffer[buf_index + 1];
				nc2 = file_buffer[buf_index + 2];

				if(((nc1 & 0xC0) == 0x80) && ((nc2 & 0xC0) == 0x80))	//both bytes are 10xx xxxx
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;  //end of buffer, no trailing bytes.
			}
		}
		else if((c & 0xF8) == 0xF0)	  //the byte is 1111 0xxx
		{
			//then check the next three bytes
			if((buf_index + 3) < buffer_length)
			{
				unsigned char nc1, nc2, nc3;

				nc1 = file_buffer[buf_index + 1];
				nc2 = file_buffer[buf_index + 2];
				nc3 = file_buffer[buf_index + 3];

				if(((nc1 & 0xC0) == 0x80) && ((nc2 & 0xC0) == 0x80) && ((nc3 & 0xC0) == 0x80)) //all three bytes are 10xx xxxx
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0; //end of buffer, no trailing bytes.
			}
		}
		else
		{
			return 0;
		}


		buf_index += 1;
	}

	return 1;	//file buffer exhausted.
}


/*---------------------------------------------------------------------------*
/*This function return 1 if encoding_sequence is a valid and complete UTF-8 encoding, the decoded code point is stored in code_point.
  Otherwise it returns zero, with code_point undefined.
  When the buffer has fewer characters left than required for a complete UTF-8 encoding, it returns 0.
  5-byte and 6-byte UTF-8 encodings will not be processed and 0 is returned.
  When the function returns 0, *code_point is undefined.
 */
int get_utf8_code_point(unsigned char *encoding_sequence, unsigned long remaining_chars, unsigned int *code_point)
{
	int encoding_length = get_encoding_length(encoding_sequence[0]);

	switch (encoding_length)
	{
		case 1:
			{
				*code_point = get_code_point_encoding_1(encoding_sequence);
				return 1;
			}
		case 2:
			{
				if((remaining_chars >= 2) && is_valid_utf8_trailing_byte(encoding_sequence[1]))
				{
					*code_point = get_code_point_encoding_2(encoding_sequence);
					return 1;
				}
				else
				{
					return 0;
				}
			}
		case 3:
			{
				if((remaining_chars >= 3) && 
				   is_valid_utf8_trailing_byte(encoding_sequence[1]) && 
				   is_valid_utf8_trailing_byte(encoding_sequence[2]))
				{
					*code_point = get_code_point_encoding_3(encoding_sequence);
					return 1;
				}
				else
				{
					return 0;
				}
			}
		case 4:
			{
				if((remaining_chars >= 4) &&
				   is_valid_utf8_trailing_byte(encoding_sequence[1]) && 
				   is_valid_utf8_trailing_byte(encoding_sequence[2]) && 
				   is_valid_utf8_trailing_byte(encoding_sequence[3]))
				{
					*code_point = get_code_point_encoding_4(encoding_sequence);
					return 1;
				}
				else
				{
					return 0;
				}
			}
		default:
			return 0;
	}

}

/*This function decodes a 1-byte UTF-8 encoding byte sequence and
  returns the UTF-8 code point.
  precondition:	1)encoding_sequence is a valid 1-byte long UTF-8 encoding. 
*/
unsigned int get_code_point_encoding_1(unsigned char *encoding_sequence)
{
	unsigned int code_point;
	unsigned char bt1;

	bt1 = encoding_sequence[0];

	code_point = bt1;

	return code_point;
}


/*This function decodes a 2-byte UTF-8 encoding byte sequence and
  returns the UTF-8 code point.
  precondition:	1)encoding_sequence is a valid 2-byte long UTF-8 encoding. 
*/
unsigned int get_code_point_encoding_2(unsigned char *encoding_sequence)
{
	unsigned int code_point, to_int;
	unsigned char bt2, bt1;

	bt2 = encoding_sequence[1];
	bt2 = bt2 & 0x3F;

	code_point = bt2;

	bt1 = encoding_sequence[0];
	bt1 = bt1 & 0x1F;

	to_int = bt1;
	to_int = to_int << 6;

	code_point = code_point | to_int;

	return code_point;

}


/*This function decodes a 3-byte UTF-8 encoding byte sequence and
  returns the UTF-8 code point.
  precondition:	1)encoding_sequence is a valid 3-byte long UTF-8 encoding. 
*/
unsigned int get_code_point_encoding_3(unsigned char *encoding_sequence)
{
	unsigned int code_point, to_int;
	unsigned char bt3, bt2, bt1;

	bt3 = encoding_sequence[2];
	bt3 = bt3 & 0x3F;

	code_point = bt3;

	bt2 = encoding_sequence[1];
	bt2 = bt2 & 0x3F;
	
	to_int = bt2;
	to_int = to_int << 6;
	 
	code_point = code_point | to_int;

	bt1 = encoding_sequence[0];
	bt1 = bt1 & 0x0F;

	to_int = bt1;
	to_int = to_int << 12;

	code_point = code_point | to_int;
	
	return code_point;

}



/*This function decodes a 4-byte UTF-8 encoding byte sequence and
  returns the UTF-8 code point.
  precondition:	1)encoding_sequence is a valid 4-byte long UTF-8 encoding. 
*/
unsigned int get_code_point_encoding_4(unsigned char *encoding_sequence)
{
	unsigned int code_point, to_int;
	unsigned char bt4, bt3, bt2, bt1;

	bt4 = encoding_sequence[3];
	bt4 = bt4 & 0x3F;

	code_point = bt4;

	bt3 = encoding_sequence[2];
	bt3 = bt3 & 0x3F;

	to_int = bt3;
	to_int = to_int << 6;

	code_point = code_point | to_int;

	bt2 = encoding_sequence[1];
	bt2 = bt2 & 0x3F;

	to_int = bt2;
	to_int = to_int << 12;

	code_point = code_point | to_int;

	bt1 = encoding_sequence[0];
	bt1 = bt1 & 0x07;

	to_int = bt1;
	to_int = to_int << 18;

	code_point = code_point | to_int;
	
	return code_point;

}


/*The function checks the leading byte of the UTF-8 character sequence and 
  returns the expected number of bytes in the encoding.
  If the first byte of the sequence is an invalid leading byte, it returns zero.
*/
int get_encoding_length(unsigned char leading_byte)
{
	if((leading_byte & 0xFE) == 0xFC)
	{
		return 6;
	}
	else if((leading_byte & 0xFC) == 0xF8)
	{
		return 5;
	}
	else if((leading_byte & 0xF8) == 0xF0)
	{
		return 4;
	}
	else if((leading_byte & 0xF0) == 0xE0)
	{
		return 3;
	}
	else if((leading_byte & 0xE0) == 0xC0)
	{
		return 2;
	}
	else if((leading_byte & 0x80) == 0x0)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

/*The function returns 1 if trailing_byte is valide UTF-8 trailing byte, otherwise returns 0*/
int is_valid_utf8_trailing_byte(unsigned char trailing_byte)
{
	if((trailing_byte & 0xC0) == 0x80)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*This function returns 1 if encoding_sequence is a overlong form of UTF-8 encoding, *seq_len is the length of that sequence.
  Otherwise returns 0. 
  Only 2-byte to 4-byte encodings are checked for overlong. 
  Zero is returned for 5-byte and 6-byte encodings regardless whether they are overlong.
  Zero is returned for other invalid or incomplete UTF-8 encodings.
  When 0 is returned, *seq_len is undefined.
  When 1 is returned, the overlong form has been verified to have the required trailing bytes.
*/
int is_overlong_utf8(unsigned char *encoding_sequence, unsigned long remaining_chars, int *seq_len)
{
	int encoding_length = get_encoding_length(encoding_sequence[0]);

	switch(encoding_length)
	{
		case 2:
			{
				if((remaining_chars >= 2) && 
				   ((encoding_sequence[0] & 0xFE) == 0xC0) &&			//leading byte: 1100 000x
				   (is_valid_utf8_trailing_byte(encoding_sequence[1])))  //trailing byte: 10xx xxxx
				{
					*seq_len = 2;
					return 1;		//sequence is invalid utf-8 because it is overlong.
				}
				else
				{
					return 0;		//sequence is valid and not overlong utf-8, 
									//or sequence is incomplete encoding,
									//or sequence is an invalid encoding for some other reason.
				}
			}
		case 3:
			{
				if((remaining_chars >= 3) && 
				   (encoding_sequence[0] == 0xE0) &&						//leading byte: 1110 0000
				   ((encoding_sequence[1] & 0xE0) == 0x80) &&				//1st trailing byte: 100x xxxx
				   (is_valid_utf8_trailing_byte(encoding_sequence[2])))		//2nd trailing byte: 10xx xxxx
				{
					*seq_len = 3;
					return 1;
				}
				else
				{
					return 0;
				}
			}
		case 4:
			{
				if((remaining_chars >= 4) &&
				   (encoding_sequence[0] == 0xF0) &&						//leading byte:		 1111 0000
				   ((encoding_sequence[1] & 0xF0) == 0x80) &&				//1st trailing byte: 1000 xxxx
				   (is_valid_utf8_trailing_byte(encoding_sequence[2])) &&	//2nd trailing byte: 10xx xxxx
				   (is_valid_utf8_trailing_byte(encoding_sequence[3])))		//3rd trailing byte: 10xx xxxx
				{
					*seq_len = 4;
					return 1;
				}
				else
				{
					return 0;
				}

			}
		default:
			{
				return 0;
			}
	}
}

/*--------------------------------------------------------------------*/
/*This function could return the input buffer if the document is verified as valid UTF-8 without
  any invalid sequences. The output buffer returned will be different to input buffer if there are
  invalid sequences that have been replaced by replacement characters*/
unsigned char *verify_utf8_encoding(unsigned char *buffer, long buf_len, long *out_buf_len)
{
	long buf_pos, char_count, output_char_count;
	unsigned char *chunk, *output;
	int seq_len, repl_char_len, indicated_len;
	unsigned int code_point;

	
	buf_pos = 0; //initialise buf_pos to 3 if there is a byte order mark.
	output = NULL;
	chunk = NULL;
	char_count = 0;
	output_char_count = 0;

	while(buf_pos < buf_len)
	{
		indicated_len = get_encoding_length(buffer[buf_pos]);

		if(indicated_len == 1)											//single byte encoding
		{																//is always valid encoding.
			if(chunk == NULL)											//condition checks can be skipped.
			{
				chunk = &buffer[buf_pos];
			}
			
			char_count += 1;
			buf_pos += 1;
		}
		//One byte in the range FE to FF
		else if((buffer[buf_pos] == 0xFE) || (buffer[buf_pos] == 0xFF))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 1;
		}
		//overlong forms
		else if(((indicated_len >= 2) && (indicated_len <= 4)) && 
				(is_overlong_utf8(&buffer[buf_pos], (buf_len - buf_pos), &seq_len)))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += seq_len;
		}
		//code point above U+10FFFF
		else if((indicated_len == 4) && 
			    (get_utf8_code_point(&buffer[buf_pos], (buf_len - buf_pos), &code_point) == 1) && 
				(code_point > 0x10FFFF))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;

			buf_pos += 4;	//get_utf8_code_point() only processes up to 4-byte encodings.

		}
		//One byte in the range F8 to FB, followed by four bytes in the range 80 to BF
		else if((indicated_len == 5) && (is_utf8_5_byte_encoding(&buffer[buf_pos], (buf_len - buf_pos))))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 5;
		}
		//One byte in the range FC to FD, followed by five bytes in the range 80 to BF
		else if((indicated_len == 6) && (is_utf8_6_byte_encoding(&buffer[buf_pos], (buf_len - buf_pos))))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 6;
		}
		//One byte in the range C0 to FD that is not followed by a byte in the range 80 to BF
		//(a leading byte in range 1100 0000 - 1111 1101  not followed by a valid trailing byte)  
		else if((buffer[buf_pos] >= 0xC0) && (buffer[buf_pos] <= 0xFD) && 
			    !(has_a_valid_trailing_byte(&buffer[buf_pos], (buf_len - buf_pos))))	
		{	
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 1;
				
		}
		//One byte in the range E0 to FD, followed by a byte in the range 80 to BF that is not followed by a byte in the range 80 to BF.
		//(a leading byte in range 1110 0000 - 1111 1101, followed by only one valid trailing byte. Should have at least two.)
		else if((buffer[buf_pos] >= 0xE0) && (buffer[buf_pos] <= 0xFD) &&
			    (has_only_one_valid_trailing_byte(&buffer[buf_pos], (buf_len - buf_pos))))	
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 2;								//consume the leading byte and the trailing byte.
		}
		//One byte in the range F0 to FD, followed by two bytes in the range 80 to BF, the last of which is not followed by a byte in the range 80 to BF
		//(a leading byte in range 1111 0000 - 1111 1101, followed by only two valid trailing bytes. Should have at least three.)
		else if((buffer[buf_pos] >= 0xF0) && (buffer[buf_pos] <= 0xFD) &&
			    (has_only_two_valid_trailing_bytes(&buffer[buf_pos], (buf_len - buf_pos))))	
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 3;								//consume the leading byte and the two trailing bytes.
		}
		//One byte in the range F8 to FD, followed by three bytes in the range 80 to BF, the last of which is not followed by a byte in the range 80 to BF
		//(a leading byte in range 1111 1000 - 1111 1101, followed by only three valid trailing bytes. Should have at least four.)
		else if((buffer[buf_pos] >= 0xF8) && (buffer[buf_pos] <= 0xFD) && 
			    (has_only_three_valid_trailing_bytes(&buffer[buf_pos], (buf_len - buf_pos))))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 4;								//consume the leading byte and the three trailing bytes.
		}
		//One byte in the range FC to FD, followed by four bytes in the range 80 to BF, the last of which is not followed by a byte in the range 80 to BF
		//(a leading byte in range 1111 1100 - 1111 1101, followed by only four valid trailing bytes. Should have five.)
		else if((buffer[buf_pos] >= 0xFC) && (buffer[buf_pos] <= 0xFD) && 
				(has_only_four_valid_trailing_bytes(&buffer[buf_pos], (buf_len - buf_pos))))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 5;								//consume the leading byte and the four trailing bytes.
		}
		//Any byte sequence that represents a code point in the range U+D800 to U+DFFF
		else if((indicated_len == 3) &&
			    (get_utf8_code_point(&buffer[buf_pos], (buf_len - buf_pos), &code_point) == 1) && 
			    (code_point >= 0xD800) && 
				(code_point <= 0xDFFF))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 3;								//invalid code point range, replace the three bytes.
		}
		//A standalone trailing byte (in the range 0x80 - 0xBF)
		else if((buffer[buf_pos] >= 0x80) && (buffer[buf_pos] <= 0xBF))
		{
			if(chunk != NULL)
			{
				output = string_n_append_2(output, chunk, output_char_count, char_count);

				output_char_count += char_count;
				chunk = NULL;
				char_count = 0;
			}

			output = append_replacement_char(output, output_char_count, &repl_char_len);
			output_char_count += repl_char_len;
			buf_pos += 1;								//replace it with the replacement character
		}
		//good utf-8 sequence
		else
		{
			if(chunk == NULL)
			{
				chunk = &buffer[buf_pos];
			}
 
			if(indicated_len == 2)										//2-byte encoding
			{
				char_count += 2;
				buf_pos += 2;
			}
			else if(indicated_len == 3)										//3-byte encoding
			{
				char_count += 3;
				buf_pos += 3;	
			}
			else if(indicated_len == 4)									//4-byte encoding
			{
				char_count += 4;
				buf_pos += 4;
			}
			else
			{
				;
			}
		}
	}
	
	if(buf_len == 0)			//empty document
	{
		output = buffer;
		output_char_count = buf_len;
	}
	else if(chunk == &buffer[0])
	{
		output = buffer;
		output_char_count = buf_len;
	}
	else
	{
		output = string_n_append_2(output, chunk, output_char_count, char_count);
		output_char_count += char_count;

		free(buffer);		//free old buffer.
	}
	

	*out_buf_len = output_char_count;
	return output;
}


/*This function appends the replacement character to the buffer.
  It returns the new buffer with the replacement character appended to it.*/
unsigned char *append_replacement_char(unsigned char *buffer, long buf_len, int *char_len)
{
	unsigned char byte_seq[5];

	utf8_byte_sequence(0xFFFD, byte_seq);

	*char_len = strlen(byte_seq);

	return string_n_append_2(buffer, byte_seq, buf_len, strlen(byte_seq));
}

/*This function returns 1 if encoding_sequence has 5 bytes and is a UTF-8 5-byte encoding.
  It returns 0 if encoding_sequence has fewer than 5 bytes, or is not a UTF-8 5-byte encoding.*/
int is_utf8_5_byte_encoding(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars >= 5) && 
	   ((encoding_sequence[0] & 0xFC) == 0xF8) &&				//leading byte 1111 10xx
	   (is_valid_utf8_trailing_byte(encoding_sequence[1])) &&	//1st trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[2])) &&	//2nd trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[3])) &&	//3rd trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[4])))		//4th trailing byte
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*This function returns 1 if encoding_sequence has 6 bytes and is a UTF-8 6-byte encoding.
  It returns 0 if encoding_sequence has fewer than 6 bytes, or is not a UTF-8 6-byte encoding.*/
int is_utf8_6_byte_encoding(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars >= 6) && 
	   ((encoding_sequence[0] & 0xFE) == 0xFC) &&				//leading byte 1111 110x
	   (is_valid_utf8_trailing_byte(encoding_sequence[1])) &&	//1st trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[2])) &&	//2nd trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[3])) &&   //3rd trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[4])) &&   //4th trailing byte
	   (is_valid_utf8_trailing_byte(encoding_sequence[5])))		//5th trailing byte
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*This function returns 1 if there is a valid trailing byte following the leading byte,
  It returns 0 if the leading byte is the last byte in the buffer, or the byte following it
  is not a valid utf-8 trailing byte*/
int has_a_valid_trailing_byte(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars >= 2) && (is_valid_utf8_trailing_byte(encoding_sequence[1])))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*This function returns 1 if there is one and only one valid trailing byte.
  It returns 0 if there is no valid trailing bytes or there are more than one valid trailing bytes*/
int has_only_one_valid_trailing_byte(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars == 2) && (is_valid_utf8_trailing_byte(encoding_sequence[1])))
	{
		return 1;		//there is only one trailing byte(last byte in buffer), and it is valid.
	}
	else if((remaining_chars > 2) && 
		    (is_valid_utf8_trailing_byte(encoding_sequence[1])) && 
		    !(is_valid_utf8_trailing_byte(encoding_sequence[2])))
	{
		return 1;		//1st trailing byte is valid, 2nd trailing byte is not valid
	}
	else
	{
		return 0;		//return 0, for all other cases.
	}
}

/*This function returns 1 if there are two and only two valid trailing bytes.
  It returns 0, if there is no valid trailing bytes or there are valid trailing bytes other than two*/
int has_only_two_valid_trailing_bytes(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars == 3) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[1])) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[2])))
	{
		return 1;
	}
	else if((remaining_chars > 3) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[1])) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[2])) && 
	   !(is_valid_utf8_trailing_byte(encoding_sequence[3])))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

/*This function returns 1 if there are three and only three valid trailing bytes.
  It returns 0, if there is  no valid trailing bytes, or there are valid trailing bytes other than three.*/
int has_only_three_valid_trailing_bytes(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars == 4) &&
	   (is_valid_utf8_trailing_byte(encoding_sequence[1])) &&
	   (is_valid_utf8_trailing_byte(encoding_sequence[2])) &&
	   (is_valid_utf8_trailing_byte(encoding_sequence[3])))
	{
		return 1;
	}
	else if((remaining_chars > 4) && 
		    (is_valid_utf8_trailing_byte(encoding_sequence[1])) && 
			(is_valid_utf8_trailing_byte(encoding_sequence[2])) && 
			(is_valid_utf8_trailing_byte(encoding_sequence[3])) && 
			!(is_valid_utf8_trailing_byte(encoding_sequence[4])))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*This function returns 1 if there are four and only four valid trailing bytes.
  It returns 0, if there is no valid trailing bytes, or there are valid trailing bytes other than four.*/
int has_only_four_valid_trailing_bytes(unsigned char *encoding_sequence, unsigned long remaining_chars)
{
	if((remaining_chars == 5) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[1])) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[2])) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[3])) && 
	   (is_valid_utf8_trailing_byte(encoding_sequence[4])))
	{
		return 1;
	}
	else if((remaining_chars > 5) && 
		    (is_valid_utf8_trailing_byte(encoding_sequence[1])) && 
			(is_valid_utf8_trailing_byte(encoding_sequence[2])) && 
			(is_valid_utf8_trailing_byte(encoding_sequence[3])) && 
			(is_valid_utf8_trailing_byte(encoding_sequence[4])) && 
			!(is_valid_utf8_trailing_byte(encoding_sequence[5])))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

/*This function returns a new char array that contains len1 number of chars from char_array_1 and len2 number of 
  chars from char_array_2, ignoring any terminating NUL characters*/
unsigned char *string_n_append_2(unsigned char *char_array_1, unsigned char *char_array_2, long len1, long len2)
{
	if((char_array_2 != NULL) && (len2 > 0))
	{
		unsigned char *new_buf;

		if((char_array_1 == NULL) || (len1 == 0))
		{
			new_buf = malloc(len2);
			memcpy(new_buf, char_array_2, len2);
		}
		else
		{
			new_buf = malloc(len1 + len2);
			memcpy(new_buf, char_array_1, len1);
			memcpy(&new_buf[len1], char_array_2, len2);

			free(char_array_1);				//reclaim old buffer, when a new buffer is created.
											//do not free char_array_2, as it is the original data not to be lost. 
		}

		return new_buf;
	}
	else
	{
		return char_array_1;
	}
}


/*The function takes a file of arbitrary encoding and converts it to UTF-8 encoding.
  If the file encoding is already UTF-8, it verifies it.
  The verified, or converted UTF-8 file is loaded in output_buffer.
  The function returns 1 if the file is successfully converted, or verified and loaded in buffer.
  Otherwise it returns 0.*/
int file_to_utf8(unsigned char *file_name, unsigned char **output_buffer, long *output_length)
{
	unsigned char *input_buffer;
	long input_buffer_length;
	int conv_result;

	input_buffer = read_file(file_name, &input_buffer_length);

	if(input_buffer == NULL)
	{
		return 0;
	}

	conv_result = memory_to_utf8(input_buffer, input_buffer_length, output_buffer, output_length);

	return conv_result;
}



/*The function takes a buffer that contains a file of arbitrary encoding and converts it to UTF-8 encoding.
  If the file encoding is already UTF-8, it verifies it.
  The verified, or converted UTF-8 file is loaded in output_buffer.
  The function always returns 1 as the file will be either converted, or verified.
 */
int memory_to_utf8(unsigned char *input_buffer, long input_buffer_length, 
				   unsigned char **output_buffer, long *output_length)
{
	unsigned char *encoding_in_meta;

	assert(input_buffer != NULL);
	
	encoding_in_meta = get_encoding(input_buffer, input_buffer_length);

	if(encoding_in_meta == NULL)
	{
	
		//check utf-8 encoding
		if(check_utf8_encoding(input_buffer, input_buffer_length) == 1)
		{
			//verify utf-8

			*output_buffer = verify_utf8_encoding(input_buffer, input_buffer_length, output_length);

			return 1;
		}
		else
		{
			//assume encoding is cp1252.
			//convert to utf-8:

			convert_to_utf8(input_buffer, input_buffer_length, "cp1252", output_buffer, output_length);

			return 1;
		}

	}
	else
	{

		if(strcasecmp(encoding_in_meta, "utf-8") == 0)
		{
			//verify utf-8

			*output_buffer = verify_utf8_encoding(input_buffer, input_buffer_length, output_length);

			return 1;
			
		}
		else
		{
			//convert to utf-8

			//try convert encoding_in_meta to UTF-8
			if(convert_to_utf8(input_buffer, input_buffer_length, encoding_in_meta, output_buffer, output_length) == 1)
			{
				return 1;
			}
			else	//conversion failed because encoding_in_meta is not supported, try "cp1252". 
			{	
				convert_to_utf8(input_buffer, input_buffer_length, "cp1252", output_buffer, output_length);

				return 1;
			}
		}
	}
}



/*This function returns 1 if conversion is successful, returns 0 if conversion fails.(from_encoding is not a supported encoding)
  Converted document is placed in output_buffer. The input_buffer is left unchanged in both cases*/
int convert_to_utf8(unsigned char *input_buffer, long input_buffer_length, unsigned char *from_encoding, 
					unsigned char **output_buffer, long *output_length)
{
	iconv_t conv_dscrtor;
	long input_buf_len, output_buf_len, max_bytes_out;
	unsigned char *input_buf, *temp_output, *temp_output_buf;

	input_buf = input_buffer;
	input_buf_len = input_buffer_length;

	temp_output_buf = malloc(input_buffer_length * 2);
	temp_output = temp_output_buf;

	output_buf_len = input_buffer_length * 2;
	max_bytes_out = output_buf_len;

	conv_dscrtor = iconv_open("utf-8", from_encoding);

	if(conv_dscrtor == (iconv_t)-1)
	{
		free(temp_output_buf);
		return 0;
	}


	while(iconv(conv_dscrtor, &input_buf, &input_buf_len, &temp_output, &max_bytes_out) == -1)
	{
		switch(errno)
		{
			case EILSEQ:
				{
					//An invalid multibyte sequence is encountered in the input.
					//skip a byte in the input buffer, and put a replacement char in the output buffer.
					unsigned char byte_seq[5];
					int repl_char_len, i;

					utf8_byte_sequence(0xFFFD, byte_seq);

					repl_char_len = strlen(byte_seq);

					if(repl_char_len <= max_bytes_out)
					{
						//append replacement char to temp_output:
						for(i = 0; i < repl_char_len; i++)
						{
							temp_output[i] = byte_seq[i];
						}

						//advance the output buffer pointer
						temp_output += repl_char_len;
						max_bytes_out -= repl_char_len;

						//skip a byte in the input buffer
						input_buf += 1;
						input_buf_len -= 1;
					}
					else
					{
						//output buffer has no more room for the replacement char
						//increase the output buffer
						temp_output_buf = realloc(temp_output_buf, output_buf_len * 2); 
						temp_output = &temp_output_buf[output_buf_len - max_bytes_out];
						max_bytes_out = output_buf_len + max_bytes_out;
						output_buf_len = output_buf_len * 2;
					}

					break;
				}
			case EINVAL:
				{
					//An incomplete multibyte sequence is encountered in the input.
					//This indicates the end of the input buffer has been reached.
					//Apppend a replacement char to the output buffer and terminate conversion:
					unsigned char byte_seq[5];
					int repl_char_len, i;

					utf8_byte_sequence(0xFFFD, byte_seq);

					repl_char_len = strlen(byte_seq);

					if(repl_char_len <= max_bytes_out)
					{
						//append replacement char to temp_output:
						for(i = 0; i < repl_char_len; i++)
						{
							temp_output[i] = byte_seq[i];
						}

						//decrement the max_byte_out
						max_bytes_out -= repl_char_len;

						*output_length = (output_buf_len - max_bytes_out);
						*output_buffer = temp_output_buf;

						return 1;

					}
					else
					{
						//output buffer has no more room for the replacement char
						//increase the output buffer
						temp_output_buf = realloc(temp_output_buf, output_buf_len * 2); 
						temp_output = &temp_output_buf[output_buf_len - max_bytes_out];
						max_bytes_out = output_buf_len + max_bytes_out;
						output_buf_len = output_buf_len * 2;

						break;
					}
				}
			case E2BIG:
				{
					//The output buffer has no more room for the next converted character.
					//increase the buffer:
					temp_output_buf = realloc(temp_output_buf, output_buf_len * 2); 
					temp_output = &temp_output_buf[output_buf_len - max_bytes_out];
					max_bytes_out = output_buf_len + max_bytes_out;
					output_buf_len = output_buf_len * 2;

					break;
				}
			default:
				{
					//unknown error
					//skip a byte in the input buffer, and put a replacement char in the output buffer.
					unsigned char byte_seq[5];
					int repl_char_len, i;

					utf8_byte_sequence(0xFFFD, byte_seq);

					repl_char_len = strlen(byte_seq);

					if(repl_char_len <= max_bytes_out)
					{
						//append replacement char to temp_output:
						for(i = 0; i < repl_char_len; i++)
						{
							temp_output[i] = byte_seq[i];
						}

						//advance the output buffer pointer
						temp_output += repl_char_len;
						max_bytes_out -= repl_char_len;

						//skip a byte in the input buffer
						input_buf += 1;
						input_buf_len -= 1;
					}
					else
					{
						//output buffer has no more room for the replacement char
						//increase the output buffer
						temp_output_buf = realloc(temp_output_buf, output_buf_len * 2); 
						temp_output = &temp_output_buf[output_buf_len - max_bytes_out];
						max_bytes_out = output_buf_len + max_bytes_out;
						output_buf_len = output_buf_len * 2;
					}

					break;
				}
		}
	}

	*output_length = (output_buf_len - max_bytes_out);
	*output_buffer = temp_output_buf;

	return 1;
	
}
