/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Token.h"


short int isWhiteSpace(char ch){
	switch(ch){
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			return 1;
		default:
			return 0;
	}
}

short int isHexDigit(int hex,char ch){
	ch |= 32;
	switch(ch){
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
			return (short int)hex;
		case '0': case '2': case '4': case '6': case '8': 
		case '1': case '3': case '5': case '7': case '9': 
			return 1;
		default:
			return 0;
	}
}

_tokenMap tokenMapInit[] = {
	{ ALPHA			, "id"				, ""		,0},
	{ EQ			, "="				, ""		,0},
	{ EQ_EQ			, "=="				, ""		,0},
	{ NOT_EQ		, "!="				, ""		,0},
	{ PLUS			, "+"				, ""		,0},
	{ PLUS_EQ		, "+="				, ""		,0},
	{ MIN			, "-"				, ""		,0},
	{ MIN_EQ		, "-="				, ""		,0},
	{ ASTERISK		, "*"				, ""		,0},
	{ ASTERISK_EQ	, "*="				, ""		,0},
	{ DIV			, "/"				, ""		,0},
	{ DIV_EQ		, "/="				, ""		,0},
	{ COMMENT_START_SLASH, "//"			, ""		,0},
	{ COMMENT_START_ASTERISK, "/*"		, ""		,0},

	{ CHARACTER		, "CHAR"			, ""		,0},
	{ NUMBER_INT	, "NUMBER_INT"		, ""		,0},
	{ NUMBER_FLOAT	, "NUMBER_FLOAT"	, ""		,0},
	{ VOID			, "VOID"			, ""		,0},
	{ STRING		, "STRING"			, ""		,0},

	{ DOT			, "."				, ""		,0},
	{ SEMI_COLON	, ";"				, ""		,0},
	{ COLON			, ":"				, ""		,0},
	{ PAREN_OPEN	, "("				, ""		,0},
	{ PAREN_CLOSE	, ")"				, ""		,0},
	{ BRACK_OPEN	, "["				, ""		,0},
	{ BRACK_CLOSE	, "]"				, ""		,0},
	{ BRACK_CLOSE_OPEN, "]["			, ""		,0},
	{ AK_OPEN		, "{"				, ""		,0},
	{ AK_CLOSE		, "}"				, ""		,0},

	{ IF			, "if"				, "if"		,0},
	{ ELSE			, "else"			, "else"	,0},
	{ DO			, "do"				, "do"		,0},
	{ WHILE			, "while"			, "while"	,0},
	{ SWITCH		, "switch"			, "switch"	,0},
	{ CASE			, "case"			, "case"	,0},
	{ DEFAULT		, "default"			, "default"	,0},
	{ BREAK			, "break"			, "break"	,0},
	{ FOR			, "for"				, "for"		,0},

	{ SMALLER		, "<"				, ""		,0},
	{ SMALLER_EQ	, "<="				, ""		,0},
	{ BIGGER		, ">"				, ""		,0},
	{ BIGGER_EQ		, ">="				, ""		,0},

	{ TYPE			, "type"			, ""		,0},
	{ COMMA			, ","				, ""		,0},

	{ RETURN		, "return"			, "return"	,0},

	{ _EOL			, "EOL"				, "EOL"		,0},
	{ _EOF			, "$"				, "EOF"		,0},
	{ INVALID		, "INV"				, ""		,0}
};

const int tokenMapSize = sizeof(tokenMapInit) / sizeof(_tokenMap);
