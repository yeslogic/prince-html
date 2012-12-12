#include <stdio.h>
#include <stdlib.h>

//#include "parser.h"
#include "encode.h"


int main(int argc, char* argv[])
{
	unsigned char *file_name;
	//element_node *doc_root;
	//token *doc_type;
	unsigned char *encoding;
	
	if (argc != 2)
	{
		printf("usage: %s FILENAME\n", argv[0]);
		return 1;
	} 

	file_name = argv[1];

	
	encoding = get_file_encoding(file_name);

	if(encoding != NULL)
	{
		printf("The character encoding for the file is: %s\n", encoding);
		free(encoding);
	}
	else
	{
		printf("No encoding found.\n");
	}

	/*
	html_parse_file(file_name, &((node *)doc_root), &doc_type);

	printf("------- DOCTYPE --------\n\n");
	html_print_token(doc_type);

	printf("----------- Document Tree -----------\n\n");
	print_tree((node *)doc_root);

	free_token(doc_type);
	free_tree((node *)doc_root);
	*/

	return 0;

}
