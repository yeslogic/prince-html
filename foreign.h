// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include "token.h"
#include "tree.h"

void adjust_svg_attributes(attribute_list *svg_attrs);

void adjust_mathml_attributes(attribute_list *mathml_attrs);

unsigned char *adjust_svg_start_tag_name(unsigned char *svg_start_tag_name);

int parsing_token_in_html_content(element_node *current_node, const token *tk);

int is_mathml_text_integration_point(element_node *current_node);

int is_html_integration_point(element_node *current_node);

int string_is_text_slash_html(unsigned char *str);

int string_is_application_slash_xhtml_plus_xml(unsigned char *str);

void adjust_foreign_attributes(attribute_list *foreign_attrs);