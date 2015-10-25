// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


#define NUM_DISALLOWED_CHARS 34

const unsigned int disallowed_chars[NUM_DISALLOWED_CHARS][2] = {
	{0x00, 0xFFFD},
	{0x0D, 0x000D},
	{0x80, 0x20AC},
	{0x81, 0x0081},
	{0x82, 0x201A},
	{0x83, 0x0192},
	{0x84, 0x201E},
	{0x85, 0x2026},
	{0x86, 0x2020},
	{0x87, 0x2021},
	{0x88, 0x02C6},
	{0x89, 0x2030},
	{0x8A, 0x0160},
	{0x8B, 0x2039},
	{0x8C, 0x0152},
	{0x8D, 0x008D},
	{0x8E, 0x017D},
	{0x8F, 0x008F},
	{0x90, 0x0090},
	{0x91, 0x2018},
	{0x92, 0x2019},
	{0x93, 0x201C},
	{0x94, 0x201D},
	{0x95, 0x2022},
	{0x96, 0x2013},
	{0x97, 0x2014},
	{0x98, 0x02DC},
	{0x99, 0x2122},
	{0x9A, 0x0161},
	{0x9B, 0x203A},
	{0x9C, 0x0153},
	{0x9D, 0x009D},
	{0x9E, 0x017E},
	{0x9F, 0x0178},
};


/*pass in char[len] as str, len should be 5 or greater*/
void utf8_byte_sequence(unsigned int code_point, unsigned char *str)
{
	unsigned char c1, c2, c3, c4;

	if( code_point <= 0x007F)
	{
		c1 = code_point;
		
		str[0] = c1;
		str[1] = '\0';
	}
	else if (code_point <= 0x07FF)
	{
		c1 = 0xC0 | (code_point >> 6);	//1100 0000 | 000y yyyy

		c2 = 0x80 | (code_point & 0x003F); //1000 0000 | 00xx xxxx

		str[0] = c1;
		str[1] = c2;
		str[2] = '\0';
	}
	else if ((code_point <= 0xD7FF) || ((code_point >= 0xE000) && (code_point <= 0xFFFF)))

	{
		c1 = 0xE0 | (code_point >> 12);		//1110 0000 | 0000 zzzz

		c2 = 0x80 | ((code_point & 0x0FC0) >> 6);	//1000 0000 | 00yy yyyy

		c3 = 0x80 | (code_point & 0x003F);	//1000 0000 | 00xx xxxx

		str[0] = c1;
		str[1] = c2;
		str[2] = c3;
		str[3] = '\0';
	}
	else if((code_point >= 0x010000) && (code_point <= 0x10FFFF))
	{
		c1 = 0xF0 | (code_point >> 18);			//1111 0000 | 0000 0www

		c2 = 0x80 | ((code_point & 0x03F000) >> 12);	//1000 0000 | 00zz zzzz

		c3 = 0x80 | ((code_point & 0x0FC0) >> 6);	//1000 0000 | 00yy yyyy

		c4 = 0x80 | (code_point & 0x003F);			//1000 0000 | 00xx xxxx

		str[0] = c1;
		str[1] = c2;
		str[2] = c3;
		str[3] = c4;
		str[4] = '\0';
	}
	else
	{
		str[0] = '\0';
	}

}

/*--------------------------------------------------------------*/
/*if char_to_check is disallowed and replacement_char found, return 1, otherwise return 0*/
int replacement_for_disallowed_char(unsigned int char_to_check, unsigned int *replacement_char)
{
	int i;

	for(i = 0; i < NUM_DISALLOWED_CHARS; i++)
	{
		if(disallowed_chars[i][0] == char_to_check)
		{
			*replacement_char = disallowed_chars[i][1];
			return 1;
		}
	}

	return 0;
}

/*--------------------------------------------------------------*/
/*returns pointer to extended block if successful, 
  or NULL if unsuccessful(in this case, str is unaltered, 
  so is any previously allocated block str maybe pointing to)*/
unsigned char *string_append(unsigned char *str, unsigned char ch)
{
	int len;

	unsigned char *extended_str = NULL;

	if(str == NULL)
	{
		if((extended_str = malloc(2)) != NULL)
		{	
			extended_str[0] = ch;
			extended_str[1] = '\0';
		}
	}
	else
	{
		len = strlen(str);

		if((extended_str = malloc(len + 2)) != NULL)
		{
			strcpy(extended_str, str);
			extended_str[len] = ch;
			extended_str[(len + 1)] = '\0';
		}
		
		free(str);
	}

	return extended_str;

}
/*--------------------------------------------------------------*/
/*appends the first num of characters str2 to the end of str1, allocating memory dynamically.
  returns the concatenated string, which might be in a different memory location.
  if str2 is NULL or num is 0,  str1 is returned unchanged.
  if str1 is NULL, return a new string with the first num characters from str2.
 
  pre-condition: num <= length of str2
*/
unsigned char *string_n_append(unsigned char *str1, unsigned char *str2, long num)
{
	if((str2 != NULL) && (num > 0))
	{
		unsigned char *str;
		/*
		long str2_len = strlen(str2);

		if(num > str2_len)
		{
			num = str2_len;
		}
		*/

		if(str1 == NULL)
		{
			str = malloc(num + 1);
			strncpy(str, str2, num);
			str[num] = '\0';
		}
		else
		{
			str = malloc(strlen(str1) + num + 1);
			strcpy(str, str1);
			free(str1);
			strncat(str, str2, num);
		}

		return str;
	}
	else
	{
		return str1;
	}
}
/*--------------------------------------------------------------*/
/*Converts any uppercase letters in str to lowercase.
  The string str must be modifiable.	
*/
void string_lower(unsigned char *str)
{
	long i, len;

	if(str != NULL)
	{
		len = strlen(str);

		for(i = 0; i < len; i++)
		{
			str[i] = tolower(str[i]);
		}
	}
}


/*--------------------------------------------------------------*/
/*returns 1 if ch is a hex digit, otherwise return 0*/
int is_hex_digit(unsigned char ch)
{
	if(((ch >= 48) && (ch <= 57)) ||
	   ((ch >= 65) && (ch <= 70)) ||
	   ((ch >= 97) && (ch <= 102)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*--------------------------------------------------------------*/
/*returns 1 if ch is a decimal digit, otherwise return 0*/
int is_dec_digit(unsigned char ch)
{
	if((ch >= 48) && (ch <= 57))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



/*--------------------------------------------------------------*/
/*preprocess the input buffer of a document, new_len is the length of the buffer after being preprocessed*/
void preprocess_input(unsigned char *buffer, long buf_len, long *new_len)
{
	long i, j, shift_len, num_bytes;
	int check_for_linefeed;
	
	shift_len = 0;
	check_for_linefeed = 1;		//initially, always check for LF.
	for(i = 0; i < buf_len; i++)
	{
		//there is a CR
		if(buffer[i] == CARRIAGE_RETURN)
		{	
			//STEP 1:  Convert the CR into LF
			buffer[i] = LINE_FEED;

			
			//STEP 2:  Determine if the next byte is a LF and calculate the next chunk to shift forward.

			//Find the chunk of bytes:
			//either up to the end of the buffer, or
			//up to and including the next CR. If that CR is followed by an LF, include that LF,
			//then shift that chunk of bytes to the left, by shift_len.

			//check_for_linefeed is the flag that indicates whether there is a LF at the end 
			//of the chunk that was shifted forward.
			//If the CR at the current position is followd by an LF, increment shift_len.
			//shift_len is essentially the number of LF's that have been found. It is also the number
			//of bytes that have been vacated. So the next chunk of bytes will need to be shifted by
			//the distance of shift_len.
			if((check_for_linefeed) && 
			   (((i + 1) < buf_len) && (buffer[i + 1] == LINE_FEED)))
			{
				shift_len += 1;
			}
				

			check_for_linefeed = 0;
			num_bytes = 0;
			for(j = (i + shift_len + 1); j < buf_len; j++)
			{
			    num_bytes += 1;
					
				//byte chunk up to and including the next CR
				if(buffer[j] == CARRIAGE_RETURN)
				{
					//check if next byte is an LF
					if(((j + 1) < buf_len) && (buffer[j + 1] == LINE_FEED))
					{
						num_bytes += 1;		//include the LF in the chunk
						check_for_linefeed = 1;
					}
					break;
				}
			}

			//shift the chunk to the left, only when num_bytes > 0, 
			if(num_bytes == 0)	//no more bytes to shift, the end of the buffer
			{
				*new_len = buf_len - shift_len; 
				return;
			}
			else
			{
				//shift the chunk of bytes to the left by shift_len.
				memmove(&buffer[i + 1], &buffer[i + 1 + shift_len], num_bytes);
				
				//check if the chunk shifted was at the end of the buffer
				if(j == buf_len)
				{
					*new_len = buf_len - shift_len; 
					return;
				}
			}
		}

	}
	*new_len = buf_len;
}


/*--------------------------------------------------------------*/
/*returns pointer to buffer holding entire document, or NULL if unsuccessful*/
unsigned char *read_file(unsigned char *file_name, long *length)
{
	FILE *fp;
	long file_length;
	unsigned char *file_buffer;

	if((fp = fopen(file_name, "rb")) == NULL)
	{
		printf("Could not open file.\n");
		return NULL;
	}

	if(fseek(fp, 0, SEEK_END)) 
	{
		printf("Fseek error.\n");
		return NULL;
	}

	file_length = ftell(fp);
	if (file_length < 0) 
	{
		printf("Ftell error.\n");
		return NULL;
	 }
	fseek(fp, 0, SEEK_SET);
	if((file_buffer = malloc(file_length)) == NULL)
	{
		printf("Could not allocate memory.\n");
		return NULL;
	}
	//fread(file_buffer, file_length, 1, fp);
	*length = fread(file_buffer, 1, file_length, fp);
	fclose(fp);

	//printf("file_length: %lu\n", file_length);
	//printf("length: %lu\n\n", *length);

	//*length = file_length;

	return file_buffer;
}


/*-----------------------------------------------------------------*/
/*chars_consumed is the number of chars consumed after the '&', for the char reference.*/
/*chars consumed might not be up to an ';', even if there is one.*/
/*when the character reference is for an attribute, set is_attr to 1, otherwise set it to 0*/
/*Set has_additional_allowed_char to 1 if there is one, otherwise set it to 0*/
/*after calling this function, the dynamically allocated memory must be free'd*/
unsigned char *html_character_reference(unsigned char *ch, int is_attr, int has_additional_allowed_char, 
										unsigned char additional_allowed_char, int *chars_consumed, 
										long curr_buf_index, long buf_len)
{
	unsigned char c = *ch;
	unsigned char *ref_char_bytes;
	unsigned char *following_char;
	unsigned long code_point, second_code_point;
	unsigned int replacement_char;
	int i;

	//character tabulation, line feed, form feed, space
	if((c == CHARACTER_TABULATION) || (c == LINE_FEED) || (c == FORM_FEED) || (c == SPACE) 
		|| (c == LESS_THAN_SIGN) || (c == AMPERSAND) || ((has_additional_allowed_char == 1) && (c == additional_allowed_char)))
	{
		*chars_consumed = 0;
		return NULL;
		/*Not a character reference. No characters are consumed, and nothing is returned.
		(This is not an error either.)*/
	}
	else if(c == NUMBER_SIGN)
	{

		//if((*(ch + 1) == 'x') || (*(ch + 1) == 'X'))
		if(((curr_buf_index + 1) < buf_len) && ((*(ch + 1) == 'x') || (*(ch + 1) == 'X')))
		{
			int num_hex_digits;
			unsigned char *hex_string;
			//check three things:
			//any characters in 0-9, A-F, a-f
			//until a character NOT in those ranges.
			//last character not in those ranges is semicolon
			//interpret the number as hexadecimal number
			//if the number is in the range 0xD800 to 0xDFFF or greater than 0x10FFFF, then this is a parse error
			num_hex_digits = 0;
			//while(is_hex_digit(*(ch + 1 + num_hex_digits + 1)) == 1)
			while(((curr_buf_index + 1 + num_hex_digits + 1) < buf_len) && 
				  (is_hex_digit(*(ch + 1 + num_hex_digits + 1))))
			{
				num_hex_digits += 1;
			}

			//if((num_hex_digits == 0) || (*(ch + 1 + num_hex_digits + 1) != ';'))
			if(num_hex_digits == 0)
			{
				*chars_consumed = 0;	//consume no characters -- no character reference done
				return NULL;
			}
			else
			{
				hex_string = malloc(num_hex_digits + 1);

				for(i = 0; i < num_hex_digits; i++)
				{
					hex_string[i] = *(ch + 2 + i);
				}
				hex_string[num_hex_digits] = '\0';

				code_point = strtol(hex_string, &following_char, 16);
				free(hex_string);
				
				//if(*(ch + 1 + num_hex_digits + 1) == ';')
				if(((curr_buf_index + 1 + num_hex_digits + 1) < buf_len) && 
				   (*(ch + 1 + num_hex_digits + 1) == ';'))
				{
					*chars_consumed = num_hex_digits + 3;	//number of hex digits plus '#', 'x' and ';'.
				}
				else
				{
					*chars_consumed = num_hex_digits + 2;	//number of hex digits plus '#' and 'x'.
				}
			}
		}
		else
		{
			int num_dec_digits;
			unsigned char *dec_string;

			num_dec_digits = 0;
			//while(is_dec_digit(*(ch + num_dec_digits + 1)) == 1)
			while(((curr_buf_index + num_dec_digits + 1) < buf_len) && 
				  (is_dec_digit(*(ch + num_dec_digits + 1))))
			{
				num_dec_digits += 1;
			}

			//if((num_dec_digits == 0) || (*(ch + num_dec_digits + 1) != ';'))
			if(num_dec_digits == 0)
			{
				*chars_consumed = 0;	//consume no characters -- no character reference done
				return NULL;
			}
			else
			{	
				dec_string = malloc(num_dec_digits + 1);

				for(i = 0; i < num_dec_digits; i++)
				{	
					dec_string[i] = *(ch + 1 + i);
				}
				dec_string[num_dec_digits] = '\0';

				code_point = strtol(dec_string, &following_char, 10);
				free(dec_string);

				//if(*(ch + num_dec_digits + 1) == ';')
				if(((curr_buf_index + num_dec_digits + 1) < buf_len) && 
				   (*(ch + num_dec_digits + 1) == ';'))
				{
					*chars_consumed = num_dec_digits + 2;	//number of dec digits plus '#' and ';'.
				}
				else
				{
					*chars_consumed = num_dec_digits + 1;	//number of dec digits plus '#'.
				}
			}
		}

		if(((code_point >= 0xD800) && (code_point <= 0xDFFF)) || (code_point > 0x10FFFF))
		{
			//parse error
			code_point = 0xFFFD;	//Return a U+FFFD REPLACEMENT CHARACTER
		}

		if(replacement_for_disallowed_char(code_point, &replacement_char) == 1)
		{
			code_point = replacement_char;
		}

		if(code_point_disallowed(code_point) == 1)
		{
			//parse error
			*chars_consumed = 0;
			return NULL;
		}

		ref_char_bytes = malloc(10);
		utf8_byte_sequence(code_point, ref_char_bytes);

		return ref_char_bytes;
	}
	else
	{
		int max_name_len = 31, initial_str_len, matched_str_len;
		unsigned char *initial_str;

		initial_str_len = max_name_len;
		for(i = 0; i <= max_name_len; i++)
		{
			if((curr_buf_index + i) == (buf_len - 1))		//the last char in buffer
			{
				initial_str_len = i + 1;	//get the string starting from *ch, ending at last char in buffer. The length of the string is (i+1).
				break;
			}
			if(*(ch + i) == SEMICOLON)
			{
				initial_str_len = i;	//get the string starting from *ch, ending at char before SEMICOLON. The length of the string is i.
				break;
			}
		}
		//the code above says: get a string of 31 characters starting after the &.
		//or get a string starting after the &,  up to (and including) the last char in buffer, if there are fewer than 31 chars left.
		//or get a string starting after the &,  up to (but not including) the SEMICOLON, if a SEMICOLON is seen within 31 chars from the &.

		if(initial_str_len == 0)
		{
			*chars_consumed = 0;
			return NULL;
		}
		else
		{
			initial_str = malloc(initial_str_len + 1);

			for(i = 0; i < initial_str_len; i++)
			{
				initial_str[i] = *(ch + i);
			}
			initial_str[initial_str_len] = '\0';
		

			matched_str_len = 0;
			for(i = initial_str_len; i > 0; i--)
			{
				initial_str[i] = '\0';

				if(is_named_char_ref(initial_str, &code_point) == 1)
				{
					matched_str_len = i;
					break;
				}
			}

			if((matched_str_len == initial_str_len) && ((curr_buf_index + initial_str_len - 1) == (buf_len - 1)))
			{
				*chars_consumed = initial_str_len;		//consume up to the last char in buffer
			}
			else if((matched_str_len == initial_str_len) && (*(ch + initial_str_len) == SEMICOLON))
			{
				*chars_consumed = initial_str_len + 1;	//matched name length plus ';'
			}
			else
			{
				*chars_consumed = matched_str_len;		//only consume the matching part of the string.

				if(is_attr == 1)	//The character reference is part of an attribute.
				{
					if((*(ch + matched_str_len) == EQUALS_SIGN) ||
					   ((*(ch + matched_str_len) >= DIGIT_ZERO) && (*(ch + matched_str_len) <= DIGIT_NINE)) ||
					   ((*(ch + matched_str_len) >= CAPITAL_A) && (*(ch + matched_str_len) <= CAPITAL_Z)) ||
					   ((*(ch + matched_str_len) >= SMALL_A) && (*(ch + matched_str_len) <= SMALL_Z)))
					{
						*chars_consumed = 0;

						free(initial_str);
						return NULL;
					}
				}
			}

			if(matched_str_len == 0)
			{
				free(initial_str);
				return NULL;
			}
			else
			{
				ref_char_bytes = malloc(10);
				utf8_byte_sequence(code_point, ref_char_bytes);

				if(has_second_code_point(initial_str, &second_code_point) == 1)
				{
					unsigned char more_char_bytes[5];
					utf8_byte_sequence(second_code_point, more_char_bytes); 
				
					strcat(ref_char_bytes, more_char_bytes);
				}

				free(initial_str);
				return ref_char_bytes;
			}

		}
	}
}



/*--------------------------------------------------------------*/