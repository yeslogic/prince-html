#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "foreign.h"

#define SVG_ATTRIBUTE_MAX_LENGTH 26
#define NUM_SVG_ATTRIBUTES 62
#define NUM_SVG_START_TAG_NAMES 36
#define SVG_START_TAG_NAME_MAX_LENGTH 20

const unsigned char svg_attr_names[NUM_SVG_ATTRIBUTES][2][SVG_ATTRIBUTE_MAX_LENGTH] = {
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


const unsigned char svg_start_tag_names[NUM_SVG_START_TAG_NAMES][2][SVG_START_TAG_NAME_MAX_LENGTH] = {
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
			free(svg_start_tag_name);

			return strdup(svg_start_tag_names[i][1]);
		}
	}

	return svg_start_tag_name;
}


/*----------------------------------------------------------------------*/
/*return 1 if current_node dictates that the next token should be parsed in HTML content*/
/*return 0 if it should be parsed in foreign content*/
int parsing_token_in_html_content(element_node *current_node, token *tk)
{
	if((current_node == NULL) 
	   ||
	   (current_node->name_space == HTML) 
	   ||
	   ((is_mathml_text_integration_point(current_node) == 1) && 
		((tk->type == TOKEN_START_TAG) && ((strcmp(tk->stt.tag_name, "mglyph") != 0) && 
										   (strcmp(tk->stt.tag_name, "malignmark") != 0)))) 
	   ||
	   ((is_mathml_text_integration_point(current_node) == 1) && (tk->type == TOKEN_CHARACTER))
	   ||
	   ((current_node->name_space == MATHML) && (strcmp(current_node->name, "annotation-xml") == 0) &&
	    ((tk->type == TOKEN_START_TAG) && (strcmp(tk->stt.tag_name, "svg") == 0)))
	   ||
	   ((is_html_integration_point(current_node) == 1) && (tk->type == TOKEN_START_TAG))
	   ||
	   ((is_html_integration_point(current_node) == 1) && (tk->type == TOKEN_CHARACTER))
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
		if(strlen(str) == 9)
		{
			if(((str[0] == 't') || (str[0] == 'T')) &&
			   ((str[1] == 'e') || (str[1] == 'E')) &&
			   ((str[2] == 'x') || (str[2] == 'X')) &&
			   ((str[3] == 't') || (str[3] == 'T')) &&
			   (str[4] == '/')						&&
			   ((str[5] == 'h') || (str[5] == 'H')) &&
			   ((str[6] == 't') || (str[6] == 'T')) &&
			   ((str[7] == 'm') || (str[7] == 'M')) &&
			   ((str[8] == 'l') || (str[8] == 'L')))
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
		if(strlen(str) == 21)
		{
			if(((str[0] == 'a') || (str[0] == 'A')) &&
			   ((str[1] == 'p') || (str[1] == 'P')) &&
			   ((str[2] == 'p') || (str[2] == 'P')) &&
			   ((str[3] == 'l') || (str[3] == 'L')) &&
			   ((str[4] == 'i')	|| (str[4] == 'I'))	&&
			   ((str[5] == 'c') || (str[5] == 'C')) &&
			   ((str[6] == 'a') || (str[6] == 'A')) &&
			   ((str[7] == 't') || (str[7] == 'T')) &&
			   ((str[8] == 'i') || (str[8] == 'I'))	&&
			   ((str[9] == 'o') || (str[9] == 'O')) &&
			   ((str[10] == 'n') || (str[10] == 'N')) &&
			   (str[11] == '/')						  &&
			   ((str[12] == 'x') || (str[12] == 'X')) &&
			   ((str[13] == 'h') || (str[13] == 'H')) &&
			   ((str[14] == 't') || (str[14] == 'T')) &&
			   ((str[15] == 'm') || (str[15] == 'M')) &&
			   ((str[16] == 'l') || (str[16] == 'L')) &&
			   (str[17] == '+')						  &&
			   ((str[18] == 'x') || (str[18] == 'X')) &&
			   ((str[19] == 'm') || (str[19] == 'M')) &&
			   ((str[20] == 'l') || (str[20] == 'L')))
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
	else
	{
		return 0;
	}

}