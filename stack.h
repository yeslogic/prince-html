typedef enum {
	ADDRESS, 
	APPLET,
	AREA,
	ARTICLE,
	ASIDE,
	BASE,
	BASEFONT,
	BGSOUND,
	BLOCKQUOTE,
	BODY,
	BR,
	BUTTON,
	CAPTION,
	CENTER,
	COL,
	COLGROUP,
	COMMAND,
	DD,
	DETAILS,
	DIR,
	DIV,
	DL,
	DT,
	EMBED,
	FIELDSET,
	FIGCAPTION,
	FIGURE,
	FOOTER,
	FORM,
	FRAME,
	FRAMESET,
	H1,
	H2,
	H3,
	H4,
	H5,
	H6,
	HEAD,
	HEADER,
	HGROUP,
	HR,
	HTML,
	IFRAME,
	IMG,
	INPUT,
	KEYGEN,
	ISINDEX,
	LIP168,
	LINK,
	LISTING,
	MARQUEE,
	MENU,
	META,
	NAV,
	NOEMBED,
	NOFRAMES,
	NOSCRIPT,
	OBJECT,
	OL,
	P,
	PARAM,
	PLAINTEXT,
	PRE,
	SOURCE,
	SCRIPT,
	SECTION,
	SELECT,
	STYLE,
	SUMMARY,
	TABLE,
	TBODY,
	TD,
	TEXTAREA,
	TFOOT,
	TH,
	THEAD,
	TITLE,
	TR,
	TRACK,
	UL,
	WBR,
	XMP,
	TEXT_NODE,
	A,
	B,
	BIG,
	CODE,
	EM,
	FONT,
	I,
	NOBR,
	S,
	SMALL,
	STRIKE,
	STRONG,
	TT,
	U
} element;


typedef struct element_stack_s element_stack;

/*if st is empty, return 1, otherwise return 0*/
int html_element_stack_is_empty(element_stack *st);

void html_element_stack_push(element_stack **st, element el);

/*if element_stack is empty, return 0, otherwise pop the top element and return 1 */
int html_element_stack_pop(element_stack **st, element *el);

/*if element_stack is empty, return 0, otherwise put the top element in *el and return 1 */
int html_element_stack_top(element_stack *st, element *el);

/*if el is on the stack, return 1, otherwise return 0*/
int html_element_on_stack(element_stack *st, element el);

/*if element_name is valid, put element in *el and return 1*/
/*otherwise return 0 and *el is undefined*/
int html_get_element(unsigned char *element_name, element *el);

void html_print_element(element el);