// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "token.h"
#include "encode.h"


int get_attribute(unsigned char *file_buffer, long buffer_length, long *buf_index,  
				   unsigned char **attr_name, unsigned char **attr_value);

unsigned char *get_charset_from_content(unsigned char *attr_value);


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
/*returns a string representing the character encoding, or NULL if no character encoding can be found*/
unsigned char *get_encoding(unsigned char *file_buffer, long buffer_length)
{
	long buf_index;
	
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

						if((strcmp(attr_name, "http-equiv") == 0) && (strcmp(attr_value, "content-type") == 0))
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
						return 1;		//attribute name is *attr_name, value is *attr_value.
					}
					else if((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z))
					{
						unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;
						*attr_value = string_append(*attr_value, c);
					}
					else
					{
						unsigned char c = file_buffer[*buf_index];
						*attr_value = string_append(*attr_value, c);
					}

					*buf_index += 1;
				}
			}
			else if((*buf_index < buffer_length) && (file_buffer[*buf_index] == GREATER_THAN_SIGN))
			{
				return 1;  //attribute name is *attr_name, value is the empty string.
			}
			else if((*buf_index < buffer_length) && ((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z)))
			{
				unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;
				*attr_value = string_append(*attr_value, c);
				*buf_index += 1;
			}	
			else
			{
				if(*buf_index < buffer_length)
				{
					unsigned char c = file_buffer[*buf_index];
					*attr_value = string_append(*attr_value, c);
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
					return 1;  //attribute name is *attr_name, value is *attr_value.
				}
				else if((file_buffer[*buf_index] >= CAPITAL_A) && (file_buffer[*buf_index] <= CAPITAL_Z))
				{
					unsigned char c = file_buffer[*buf_index] + CAPITAL_TO_SMALL;
					*attr_value = string_append(*attr_value, c);
				}
				else
				{
					unsigned char c = file_buffer[*buf_index];
					*attr_value = string_append(*attr_value, c);
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

