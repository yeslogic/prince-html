#include <stdio.h>
#include <stdlib.h>

#include "parser.h"




int main(int argc, char* argv[])
{
	unsigned char *file_name;
	element_node *doc_root;
	token *doc_type;
	
	if (argc != 2)
	{
		printf("usage: %s FILENAME\n", argv[0]);
		return 1;
	} 

	file_name = argv[1];

	html_parse_file(file_name, &((node *)doc_root), &doc_type);

	printf("------- DOCTYPE --------\n\n");
	html_print_token(doc_type);

	printf("----------- Document Tree -----------\n\n");
	print_tree((node *)doc_root);

	free_token(doc_type);
	free_tree((node *)doc_root);

	return 0;

}
