// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


typedef enum {
	INITIAL,
	BEFORE_HTML,
	BEFORE_HEAD,
	IN_HEAD,
	IN_HEAD_NOSCRIPT,
	AFTER_HEAD,
	IN_BODY,
	TEXT,
	IN_TABLE,
	IN_TABLE_TEXT,
	IN_CAPTION,
	IN_COLUMN_GROUP,
	IN_TABLE_BODY,
	IN_ROW,
	IN_CELL,
	IN_SELECT,
	IN_SELECT_IN_TABLE,
	IN_TEMPLATE,
	AFTER_BODY,
	IN_FRAMESET,
	AFTER_FRAMESET,
	AFTER_AFTER_BODY,
	AFTER_AFTER_FRAMESET,
	NUM_INSERTION_MODES
} insertion_mode;


typedef struct mode_stack_s mode_stack;


struct mode_stack_s {
		insertion_mode m;
		mode_stack *tail;
};


void mode_stack_push(mode_stack **mst, insertion_mode m);


/*if mode_stack is empty, return 0, otherwise pop the top item and return 1 */
int mode_stack_pop(mode_stack **mst);


/*returns 1 is stack is empty, otherwise returns 0*/
int mode_stack_is_empty(mode_stack *mst);


/*return the top item in the mode_stack. 
  caller needs to make sure mst is not empty.
*/
insertion_mode mode_stack_top(mode_stack *mst);


/*free the stack of insertion modes*/
void free_mode_stack(mode_stack *mst);