// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "foreign.h"


#define NUM_SVG_ATTRIBUTES 62
#define NUM_SVG_START_TAG_NAMES 36

const unsigned char *svg_attr_names[NUM_SVG_ATTRIBUTES][2]= {

						{"attributename", 				"attributeName"},
						{"attributetype", 				"attributeType"},
						{"basefrequency", 				"baseFrequency"},
						{"baseprofile", 				"baseProfile"},
						{"calcmode", 					"calcMode"},
						{"clippathunits", 				"clipPathUnits"},
						{"contentscripttype", 			"contentScriptType"},
						{"contentstyletype", 			"contentStyleType"},
						{"diffuseconstant", 			"diffuseConstant"},
						{"edgemode", 					"edgeMode"},
						{"externalresourcesrequired", 	"externalResourcesRequired"},
						{"filterres", 					"filterRes"},
						{"filterunits", 				"filterUnits"},
						{"glyphref", 					"glyphRef"},
						{"gradienttransform", 			"gradientTransform"},
						{"gradientunits", 				"gradientUnits"},
						{"kernelmatrix", 				"kernelMatrix"},
						{"kernelunitlength", 			"kernelUnitLength"},
						{"keypoints", 					"keyPoints"},
						{"keysplines", 					"keySplines"},
						{"keytimes", 					"keyTimes"},
						{"lengthadjust", 				"lengthAdjust"},
						{"limitingconeangle", 			"limitingConeAngle"},
						{"markerheight", 				"markerHeight"},
						{"markerunits", 				"markerUnits"},
						{"markerwidth", 				"markerWidth"},
						{"maskcontentunits", 			"maskContentUnits"},
						{"maskunits", 					"maskUnits"},
						{"numoctaves", 					"numOctaves"},
						{"pathlength", 					"pathLength"},
						{"patterncontentunits", 		"patternContentUnits"},
						{"patterntransform", 			"patternTransform"},
						{"patternunits", 				"patternUnits"},
						{"pointsatx", 					"pointsAtX"},
						{"pointsaty", 					"pointsAtY"},
						{"pointsatz", 					"pointsAtZ"},
						{"preservealpha", 				"preserveAlpha"},
						{"preserveaspectratio", 		"preserveAspectRatio"},
						{"primitiveunits", 				"primitiveUnits"},
						{"refx", 						"refX"},
						{"refy", 						"refY"},
						{"repeatcount", 				"repeatCount"},
						{"repeatdur", 					"repeatDur"},
						{"requiredextensions", 			"requiredExtensions"},
						{"requiredfeatures", 			"requiredFeatures"},
						{"specularconstant", 			"specularConstant"},
						{"specularexponent", 			"specularExponent"},
						{"spreadmethod", 				"spreadMethod"},
						{"startoffset", 				"startOffset"},
						{"stddeviation", 				"stdDeviation"},
						{"stitchtiles", 				"stitchTiles"},
						{"surfacescale", 				"surfaceScale"},
						{"systemlanguage", 				"systemLanguage"},
						{"tablevalues", 				"tableValues"},
						{"targetx", 					"targetX"},
						{"targety", 					"targetY"},
						{"textlength", 					"textLength"},
						{"viewbox", 					"viewBox"},
						{"viewtarget", 					"viewTarget"},
						{"xchannelselector", 			"xChannelSelector"},
						{"ychannelselector", 			"yChannelSelector"},
						{"zoomandpan", 					"zoomAndPan"},
};


const unsigned char *svg_start_tag_names[NUM_SVG_START_TAG_NAMES][2]= {

						{"altglyph",				"altGlyph"},
						{"altglyphdef",				"altGlyphDef"},
						{"altglyphitem",			"altGlyphItem"},
						{"animatecolor",			"animateColor"},
						{"animatemotion",			"animateMotion"},
						{"animatetransform",		"animateTransform"},
						{"clippath",				"clipPath"},
						{"feblend",					"feBlend"},
						{"fecolormatrix",			"feColorMatrix"},
						{"fecomponenttransfer",		"feComponentTransfer"},
						{"fecomposite",				"feComposite"},
						{"feconvolvematrix",		"feConvolveMatrix"},
						{"fediffuselighting",		"feDiffuseLighting"},
						{"fedisplacementmap",		"feDisplacementMap"},
						{"fedistantlight",			"feDistantLight"},
						{"feflood",					"feFlood"},
						{"fefunca",					"feFuncA"},
						{"fefuncb",					"feFuncB"},
						{"fefuncg",					"feFuncG"},
						{"fefuncr",					"feFuncR"},
						{"fegaussianblur",			"feGaussianBlur"},
						{"feimage",					"feImage"},
						{"femerge",					"feMerge"},
						{"femergenode",				"feMergeNode"},
						{"femorphology",			"feMorphology"},
						{"feoffset",				"feOffset"},
						{"fepointlight",			"fePointLight"},
						{"fespecularlighting",		"feSpecularLighting"},
						{"fespotlight",				"feSpotLight"},
						{"fetile",					"feTile"},
						{"feturbulence",			"feTurbulence"},
						{"foreignobject",			"foreignObject"},
						{"glyphref",				"glyphRef"},
						{"lineargradient",			"linearGradient"},
						{"radialgradient",			"radialGradient"},
						{"textpath",				"textPath"},
};


/*----------------------------------------------------------------------*/
void adjust_svg_attributes(attribute_list *svg_attrs)
{
	int i;

	while(svg_attrs != NULL)
	{
		for(i = 0; i < NUM_SVG_ATTRIBUTES; i++)
		{	
			if(strcmp(svg_attr_names[i][0], svg_attrs->name) == 0)
			{
				free(svg_attrs->name);

				svg_attrs->name = strdup(svg_attr_names[i][1]);

				break; //out of inner loop
			}
		}

		svg_attrs = svg_attrs->tail;
	}
}

/*----------------------------------------------------------------------*/
void adjust_mathml_attributes(attribute_list *mathml_attrs)
{
	while(mathml_attrs != NULL)
	{
		if(strcmp(mathml_attrs->name, "definitionurl") == 0)
		{
			free(mathml_attrs->name);

			mathml_attrs->name = strdup("definitionURL");

			break;
		}

		mathml_attrs = mathml_attrs->tail;
	}
}


/*----------------------------------------------------------------------*/
unsigned char *adjust_svg_start_tag_name(unsigned char *svg_start_tag_name)
{
	int i;

	assert(svg_start_tag_name != NULL);


	for(i = 0; i < NUM_SVG_START_TAG_NAMES; i++)
	{
		if(strcmp(svg_start_tag_names[i][0], svg_start_tag_name) == 0)
		{
			//free(svg_start_tag_name);

			return strdup(svg_start_tag_names[i][1]);
		}
	}

	return svg_start_tag_name;
}


/*----------------------------------------------------------------------*/
/*return 1 if current_node dictates that the next token should be parsed in HTML content*/
/*return 0 if it should be parsed in foreign content*/
int parsing_token_in_html_content(element_node *current_node, const token *tk)
{
	if((current_node == NULL) 
	   ||
	   (current_node->name_space == HTML) 
	   ||
	   ((is_mathml_text_integration_point(current_node) == 1) && 
		((tk->type == TOKEN_START_TAG) && ((strcmp(tk->stt.tag_name, "mglyph") != 0) && 
										   (strcmp(tk->stt.tag_name, "malignmark") != 0)))) 
	   ||
	   ((is_mathml_text_integration_point(current_node) == 1) && (tk->type == TOKEN_MULTI_CHAR))
	   ||
	   ((current_node->name_space == MATHML) && (strcmp(current_node->name, "annotation-xml") == 0) &&
	    ((tk->type == TOKEN_START_TAG) && (strcmp(tk->stt.tag_name, "svg") == 0)))
	   ||
	   ((is_html_integration_point(current_node) == 1) && (tk->type == TOKEN_START_TAG))
	   ||
	   ((is_html_integration_point(current_node) == 1) && (tk->type == TOKEN_MULTI_CHAR))
	   ||
	   (tk->type == TOKEN_EOF))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*----------------------------------------------------------------------*/
/*return 1 if current_node is a "MathML text integration point", otherwise return 0.*/
int is_mathml_text_integration_point(element_node *current_node)
{
	if((current_node->name_space == MATHML) &&
	   ((strcmp(current_node->name, "mi") == 0) ||
	    (strcmp(current_node->name, "mo") == 0) ||
		(strcmp(current_node->name, "mn") == 0) ||
		(strcmp(current_node->name, "ms") == 0) ||
		(strcmp(current_node->name, "mtext") == 0)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*----------------------------------------------------------------------*/
/*return 1 if current_node is an "HTML integration point", otherwise return 0*/
int is_html_integration_point(element_node *current_node)
{
	unsigned char *value_of_encoding = get_attribute_value("encoding", current_node->attributes);

	if(((current_node->name_space == MATHML) && (strcmp(current_node->name, "annotation-xml") == 0) && 
												(string_is_text_slash_html(value_of_encoding) == 1)) 
		||
	   ((current_node->name_space == MATHML) && (strcmp(current_node->name, "annotation-xml") == 0) && 
												(string_is_application_slash_xhtml_plus_xml(value_of_encoding) == 1)) 
	    ||
	   ((current_node->name_space == SVG) && (strcmp(current_node->name, "foreignObject") == 0)) 
		||
	   ((current_node->name_space == SVG) && (strcmp(current_node->name, "desc") == 0)) 
		||
	   ((current_node->name_space == SVG) && (strcmp(current_node->name, "title") == 0)))
	{
		free(value_of_encoding);
		return 1;
	}
	else
	{
		free(value_of_encoding);
		return 0;
	}

	//the copy of attribute value returned by get_attribute_value() should be free'd.
	//function get_attribute_value(attr_name, list) returns NULL, if list is NULL, or attr_name does not exist in list, or its value is NULL*/
}



/*----------------------------------------------------------------------*/
/*return 1 if str is case-insensitive match of "text/html", otherwise return 0*/
int string_is_text_slash_html(unsigned char *str)
{
	if(str != NULL)
	{	
		if(strcasecmp(str, "text/html") == 0)
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
		return 0;
	}
}


/*----------------------------------------------------------------------*/
/*return 1 if str is case-insensitive match of "application/xhtml+xml", otherwise return 0*/
int string_is_application_slash_xhtml_plus_xml(unsigned char *str)
{
	if(str != NULL)
	{	
		if(strcasecmp(str, "application/xhtml+xml") == 0)
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
		return 0;
	}

}

/*----------------------------------------------------------------------*/
void adjust_foreign_attributes(attribute_list *foreign_attrs)
{
	while(foreign_attrs != NULL)
	{
		if(strcmp(foreign_attrs->name, "xlink:actuate") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("actuate");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xlink:arcrole") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("arcrole");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xlink:href") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("href");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xlink:role") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("role");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xlink:show") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("show");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xlink:title") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("title");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xlink:type") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("type");
			foreign_attrs->attr_ns = XLINK;
		}
		else if(strcmp(foreign_attrs->name, "xml:base") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("base");
			foreign_attrs->attr_ns = XML;
		}
		else if(strcmp(foreign_attrs->name, "xml:lang") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("lang");
			foreign_attrs->attr_ns = XML;
		}
		else if(strcmp(foreign_attrs->name, "xml:space") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("space");
			foreign_attrs->attr_ns = XML;
		}
		else if(strcmp(foreign_attrs->name, "xmlns") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("xmlns");
			foreign_attrs->attr_ns = XMLNS;

		}
		else if(strcmp(foreign_attrs->name, "xmlns:xlink") == 0)
		{
			free(foreign_attrs->name);
			foreign_attrs->name = strdup("xlink");
			foreign_attrs->attr_ns = XMLNS;
		}
		else
		{
			;
		}


		foreign_attrs = foreign_attrs->tail; 
	}
}