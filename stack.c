#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "stack.h"

struct element_stack_s {
	element el;
	element_stack *tail;
};


/*if st is empty, return 1, otherwise return 0*/
int html_element_stack_is_empty(element_stack *st)
{
	if(st == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void html_element_stack_push(element_stack **st, element el)
{
	element_stack *new_node = malloc(sizeof(element_stack));
	new_node->el = el;
	new_node->tail = *st;

	*st = new_node;
}


/*if element_stack is empty, return 0, otherwise pop the top element and return 1 */
int html_element_stack_pop(element_stack **st, element *el)
{
	element_stack *temp_st_ptr;

	if(*st != NULL)
	{
		*el = (*st)->el;
		temp_st_ptr = (*st)->tail;
		free(*st);
		*st = temp_st_ptr;
		return 1;
	}
	else
	{
		return 0;
	}
}


/*if element_stack is empty, return 0, otherwise put the top element in *el and return 1 */
int html_element_stack_top(element_stack *st, element *el)
{
	if(st != NULL)
	{
		*el = st->el;
		return 1;
	}
	else
	{
		return 0;
	}
}


/*if el is on the stack, return 1, otherwise return 0*/
int html_element_on_stack(element_stack *st, element el)
{
	while(st != NULL)
	{
		if(st->el == el)
		{
			return 1;
		}

		st = st->tail;
	}

	return 0;
}

/********************************************************/
/*if element_name is valid, put element in *el and return 1*/
/*otherwise return 0 and *el is undefined*/
int html_get_element(unsigned char *element_name, element *el)
{
	if(element_name != NULL)
	{
		if(strcmp(element_name, "address") == 0)
		{
			*el = ADDRESS;
		}
		else if(strcmp(element_name, "applet") == 0)	
		{
			*el = APPLET;
		}
		else if(strcmp(element_name, "area") == 0)	
		{
			*el = AREA;	
		}
		else if(strcmp(element_name, "article") == 0)	
		{
			*el = ARTICLE;
		}
		else if(strcmp(element_name, "aside") == 0)
		{
			*el = ASIDE;
		}
		else if(strcmp(element_name, "base") == 0)
		{
			*el = BASE;
		}
		else if(strcmp(element_name, "basefont") == 0)
		{
			*el = BASEFONT;
		}
		else if(strcmp(element_name, "bgsound") == 0)
		{
			*el = BGSOUND;
		}
		else if(strcmp(element_name, "blockquote") == 0)
		{
			*el = BLOCKQUOTE;
		}
		else if(strcmp(element_name, "body") == 0)
		{
			*el = BODY;
		}
		else if(strcmp(element_name, "br") == 0)
		{
			*el = BR;
		}
		else if(strcmp(element_name, "button") == 0)
		{
			*el = BUTTON;
		}
		else if(strcmp(element_name, "caption") == 0)
		{
			*el = CAPTION;
		}
		else if(strcmp(element_name, "center") == 0)
		{
			*el = CENTER;
		}
		else if(strcmp(element_name, "col") == 0)
		{
			*el = COL;
		}
		else if(strcmp(element_name, "colgroup") == 0)
		{
			*el = COLGROUP;
		}
		else if(strcmp(element_name, "command") == 0)
		{
			*el = COMMAND;
		}
		else if(strcmp(element_name, "dd") == 0)
		{
			*el = DD;
		}
		else if(strcmp(element_name, "details") == 0)
		{
			*el = DETAILS;
		}
		else if(strcmp(element_name, "dir") == 0)
		{
			*el = DIR;
		}
		else if(strcmp(element_name, "div") == 0)
		{
			*el = DIV;
		}
		else if(strcmp(element_name, "dl") == 0)
		{
			*el = DL;
		}
		else if(strcmp(element_name, "dt") == 0)
		{
			*el = DT;
		}
		else if(strcmp(element_name, "embed") == 0)
		{
			*el = EMBED;
		}
		else if(strcmp(element_name, "fieldset") == 0)
		{
			*el = FIELDSET;
		}
		else if(strcmp(element_name, "figcaption") == 0)
		{
			*el = FIGCAPTION;
		}
		else if(strcmp(element_name, "figure") == 0)
		{
			*el = FIGURE;
		}
		else if(strcmp(element_name, "footer") == 0)
		{
			*el = FOOTER;
		}
		else if(strcmp(element_name, "form") == 0)
		{
			*el = FORM;
		}
		else if(strcmp(element_name, "frame") == 0)
		{
			*el = FRAME;
		}
		else if(strcmp(element_name, "frameset") == 0)
		{
			*el = FRAMESET;
		}
		else if(strcmp(element_name, "h1") == 0)
		{
			*el = H1;
		}
		else if(strcmp(element_name, "h2") == 0)
		{
			*el = H2;
		}
		else if(strcmp(element_name, "h3") == 0)
		{
			*el = H3;
		}
		else if(strcmp(element_name, "h4") == 0)
		{
			*el = H4;
		}
		else if(strcmp(element_name, "h5") == 0)
		{
			*el = H5;
		}
		else if(strcmp(element_name, "h6") == 0)
		{
			*el = H6;
		}
		else if(strcmp(element_name, "head") == 0)
		{
			*el = HEAD;
		}
		else if(strcmp(element_name, "header") == 0)
		{
			*el = HEADER;
		}
		else if(strcmp(element_name, "hgroup") == 0)
		{
			*el = HGROUP;
		}
		else if(strcmp(element_name, "hr") == 0)
		{
			*el = HR;
		}
		else if(strcmp(element_name, "html") == 0)
		{
			*el = HTML;
		}
		else if(strcmp(element_name, "iframe") == 0)
		{
			*el = IFRAME;
		}
		else if(strcmp(element_name, "img") == 0)
		{
			*el = IMG;
		}
		else if(strcmp(element_name, "input") == 0)
		{
			*el = INPUT;
		}
		else if(strcmp(element_name, "keygen") == 0)
		{
			*el = KEYGEN;
		}
		else if(strcmp(element_name, "isindex") == 0)
		{
			*el = ISINDEX;
		}
		else if(strcmp(element_name, "lip168") == 0)
		{
			*el = LIP168;
		}
		else if(strcmp(element_name, "link") == 0)
		{
			*el = LINK;
		}
		else if(strcmp(element_name, "listing") == 0)
		{
			*el = LISTING;
		}
		else if(strcmp(element_name, "marquee") == 0)
		{
			*el = MARQUEE;
		}
		else if(strcmp(element_name, "menu") == 0)
		{
			*el = MENU;
		}
		else if(strcmp(element_name, "meta") == 0)
		{
			*el = META;
		}
		else if(strcmp(element_name, "nav") == 0)
		{
			*el = NAV;
		}
		else if(strcmp(element_name, "noembed") == 0)
		{
			*el = NOEMBED;
		}
		else if(strcmp(element_name, "noframes") == 0)
		{
			*el = NOFRAMES;
		}
		else if(strcmp(element_name, "noscript") == 0)
		{
			*el = NOSCRIPT;
		}
		else if(strcmp(element_name, "object") == 0)
		{
			*el = OBJECT;
		}
		else if(strcmp(element_name, "ol") == 0)
		{
			*el = OL;
		}
		else if(strcmp(element_name, "p") == 0)
		{
			*el = P;
		}
		else if(strcmp(element_name, "param") == 0)
		{
			*el = PARAM;
		}
		else if(strcmp(element_name, "plaintext") == 0)	
		{
			*el = PLAINTEXT;
		}
		else if(strcmp(element_name, "pre") == 0)
		{
			*el = PRE;
		}
		else if(strcmp(element_name, "source") == 0)
		{
			*el = SOURCE;
		}
		else if(strcmp(element_name, "script") == 0)
		{
			*el = SCRIPT;
		}
		else if(strcmp(element_name, "section") == 0)
		{
			*el = SECTION;
		}
		else if(strcmp(element_name, "select") == 0)
		{
			*el = SELECT;
		}
		else if(strcmp(element_name, "style") == 0)
		{
			*el = STYLE;
		}
		else if(strcmp(element_name, "summary") == 0)
		{
			*el = SUMMARY;
		}
		else if(strcmp(element_name, "table") == 0)
		{
			*el = TABLE;
		}
		else if(strcmp(element_name, "tbody") == 0)
		{
			*el = TBODY;
		}
		else if(strcmp(element_name, "td") == 0)
		{
			*el = TD;
		}
		else if(strcmp(element_name, "textarea") == 0)
		{
			*el = TEXTAREA;
		}
		else if(strcmp(element_name, "tfoot") == 0)
		{
			*el = TFOOT;
		}
		else if(strcmp(element_name, "th") == 0)
		{
			*el = TH;
		}
		else if(strcmp(element_name, "thead") == 0)
		{
			*el = THEAD;
		}
		else if(strcmp(element_name, "title") == 0)
		{
			*el = TITLE;
		}
		else if(strcmp(element_name, "tr") == 0)
		{
			*el = TR;
		}
		else if(strcmp(element_name, "track") == 0)
		{
			*el = TRACK;
		}
		else if(strcmp(element_name, "ul") == 0)
		{
			*el = UL;
		}
		else if(strcmp(element_name, "wbr") == 0)
		{
			*el = WBR;
		}
		else if(strcmp(element_name, "xmp") == 0)
		{
			*el = XMP;
		}
		else if(strcmp(element_name, "text_node") == 0)
		{
			*el = TEXT_NODE;
		}
		else if(strcmp(element_name, "a") == 0)
		{
			*el = A;
		}
		else if(strcmp(element_name, "b") == 0)
		{
			*el = B;
		}
		else if(strcmp(element_name, "big") == 0)
		{
			*el = BIG;
		}
		else if(strcmp(element_name, "code") == 0)
		{
			*el = CODE;
		}
		else if(strcmp(element_name, "em") == 0)
		{
			*el = EM;
		}
		else if(strcmp(element_name, "font") == 0)
		{
			*el = FONT;
		}
		else if(strcmp(element_name, "i") == 0)
		{
			*el = I;
		}
		else if(strcmp(element_name, "nobr") == 0)
		{
			*el = NOBR;
		}
		else if(strcmp(element_name, "s") == 0)
		{
			*el = S;
		}
		else if(strcmp(element_name, "small") == 0)
		{
			*el = SMALL;
		}
		else if(strcmp(element_name, "strike") == 0)
		{
			*el = STRIKE;
		}
		else if(strcmp(element_name, "strong") == 0)
		{
			*el = STRONG;
		}
		else if(strcmp(element_name, "tt") == 0)
		{
			*el = TT;
		}
		else if(strcmp(element_name, "u") == 0)
		{
			*el = U;
		}
		else
		{
			return 0;
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

/************************************************************/
void html_print_element(element el)
{
	if(el == ADDRESS)
	{
 		printf("address");			
	}	
	else if(el == APPLET)
	{
      	printf("applet");      
	}
	else if(el == AREA)
	{
	  	printf("area");      
	}
	else if(el == ARTICLE)
	{
      	printf("article");      
	}
	else if(el == ASIDE)
	{
      	printf("aside");      
	}
	else if(el == BASE)
	{
      	printf("base");      
	}
	else if(el == BASEFONT)
	{
      	printf("basefont");      
	}
	else if(el == BGSOUND)
	{
      	printf("bgsound");      
	}
	else if(el == BLOCKQUOTE)
	{
      	printf("blockquote");      
	}
	else if(el == BODY)
	{
      	printf("body");      
	}
	else if(el == BR)
	{
      	printf("br");      
	}
	else if(el == BUTTON)
	{
      	printf("button");      
	}
	else if(el == CAPTION)
	{
      	printf("caption");      
	}
	else if(el == CENTER)
	{
      	printf("center");      
	}
	else if(el == COL)
	{
      	printf("col");      
	}
	else if(el == COLGROUP)
	{
      	printf("colgroup");      
	}
	else if(el == COMMAND)
	{
      	printf("command");      
	}
	else if(el == DD)
	{
      	printf("dd");      
	}
	else if(el == DETAILS)
	{
      	printf("details");      
	}
	else if(el == DIR)
	{
      	printf("dir");      
	}
	else if(el == DIV)
	{
      	printf("div");      
	}
	else if(el == DL)
	{
      	printf("dl");      
	}
	else if(el == DT)
	{
      	printf("dt");      
	}
	else if(el == EMBED)
	{
      	printf("embed");      
	}
	else if(el == FIELDSET)
	{
      	printf("fieldset");      
	}
	else if(el == FIGCAPTION)
	{
      	printf("figcaption");      
	}
	else if(el == FIGURE)
	{
      	printf("figure");      
	}
	else if(el == FOOTER)
	{
      	printf("footer");      
	}
	else if(el == FORM)
	{
      	printf("form");      
	}
	else if(el == FRAME)
	{
      	printf("frame");      
	}
	else if(el == FRAMESET)
	{
      	printf("frameset");      
	}
	else if(el == H1)
	{
      	printf("h1");      
	}
	else if(el == H2)
	{
      	printf("h2");      
	}
	else if(el == H3)
	{
      	printf("h3");      
	}
	else if(el == H4)
	{
      	printf("h4");      
	}
	else if(el == H5)
	{
      	printf("h5");      
	}
	else if(el == H6)
	{
      	printf("h6");      
	}
	else if(el == HEAD)
	{
      	printf("head");      
	}
	else if(el == HEADER)
	{
      	printf("header");      
	}
	else if(el == HGROUP)
	{
      	printf("hgroup");      
	}
	else if(el == HR)
	{
      	printf("hr");      
	}
	else if(el == HTML)
	{
      	printf("html");      
	}
	else if(el == IFRAME)
	{
      	printf("iframe");      
	}
	else if(el == IMG)
	{
      	printf("img");      
	}
	else if(el == INPUT)
	{
      	printf("input");      
	}
	else if(el == KEYGEN)
	{
      	printf("keygen");      
	}
	else if(el == ISINDEX)
	{
      	printf("isindex");      
	}
	else if(el == LIP168)
	{
      	printf("lip168");      
	}
	else if(el == LINK)
	{
      	printf("link");      
	}
	else if(el == LISTING)
	{
      	printf("listing");      
	}
	else if(el == MARQUEE)
	{
      	printf("marquee");      
	}
	else if(el == MENU)
	{
      	printf("menu");      
	}
	else if(el == META)
	{
      	printf("meta");      
	}
	else if(el == NAV)
	{
      	printf("nav");      
	}
	else if(el == NOEMBED)
	{
      	printf("noembed");      
	}
	else if(el == NOFRAMES)
	{
      	printf("noframes");      
	}
	else if(el == NOSCRIPT)
	{
      	printf("noscript");      
	}
	else if(el == OBJECT)
	{
      	printf("object");      
	}
	else if(el == OL)
	{
      	printf("ol");      
	}
	else if(el == P)
	{
      	printf("p");      
	}
	else if(el == PARAM)
	{
      	printf("param");      
	}
	else if(el == PLAINTEXT)
	{
      	printf("plaintext");      
	}
	else if(el == PRE)
	{
      	printf("pre");      
	}
	else if(el == SOURCE)
	{
      	printf("source");      
	}
	else if(el == SCRIPT)
	{
      	printf("script");      
	}
	else if(el == SECTION)
	{
      	printf("section");      
	}
	else if(el == SELECT)
	{
      	printf("select");      
	}
	else if(el == STYLE)
	{
      	printf("style");      
	}
	else if(el == SUMMARY)
	{
      	printf("summary");      
	}
	else if(el == TABLE)
	{
      	printf("table");      
	}
	else if(el == TBODY)
	{
      	printf("tbody");      
	}
	else if(el == TD)
	{
      	printf("td");      
	}
	else if(el == TEXTAREA)
	{
      	printf("textarea");      
	}
	else if(el == TFOOT)
	{
      	printf("tfoot");      
	}
	else if(el == TH)
	{
      	printf("th");      
	}
	else if(el == THEAD)
	{
      	printf("thead");      
	}
	else if(el == TITLE)
	{
      	printf("title");      
	}
	else if(el == TR)
	{
      	printf("tr");      
	}
	else if(el == TRACK)
	{
      	printf("track");      
	}
	else if(el == UL)
	{
      	printf("ul");      
	}
	else if(el == WBR)
	{
      	printf("wbr");     
	}
	else if(el == XMP)
	{
      	printf("xmp");      
	}
	else if(el == TEXT_NODE)
	{
      	printf("text_node");      
	}
	else if(el == A)
	{
      	printf("a");      
	}
	else if(el == B)
	{
      	printf("b");      
	}
	else if(el == BIG)
	{
      	printf("big");      
	}
	else if(el == CODE)
	{
      	printf("code");      
	}
	else if(el == EM)
	{
      	printf("em");      
	}
	else if(el == FONT)
	{
      	printf("font");      
	}
	else if(el == I)
	{
      	printf("i");      
	}
	else if(el == NOBR)
	{
      	printf("nobr");      
	}
	else if(el == S)
	{
      	printf("s");      
	}
	else if(el == SMALL)
	{
      	printf("small");      
	}
	else if(el == STRIKE)
	{
      	printf("strike");      
	}
	else if(el == STRONG)
	{
      	printf("strong");      
	}
	else if(el == TT)
	{
      	printf("tt");      
	}
	else if(el == U)
	{
      	printf("u");      
	}
	else
	{
		printf("Invalid element.\n");
	}
}