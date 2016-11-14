/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"

/*enum _tokenType { 
	NUMBER =  0,
	CHARACTER,
	STRING
};*/

struct token{
	DWORD intVal;
	float floatVal;
	char strVal[MAX_STRING_LEN];
	char charVal;
};


enum _tokens {
	ALPHA = 1,
	EQ = 10,
	EQ_EQ = 11,
	NOT_EQ = 12,
	PLUS = 30,
	PLUS_EQ = 31,
    MIN = 40,
	MIN_EQ = 41,
	ASTERISK = 50,
	ASTERISK_EQ = 51,
	DIV = 60,
	DIV_EQ = 61,
	COMMENT_START_SLASH = 70,
	COMMENT_START_ASTERISK = 71,

	CHARACTER = 80,
	NUMBER_INT = 90,
	NUMBER_FLOAT = 91,
	VOID = 92,
	STRING = 100,

	DOT = 110,
	SEMI_COLON = 120,
	COLON = 121,

	PAREN_OPEN	= 130, // (
	PAREN_CLOSE = 131,
	BRACK_OPEN = 140, // [
	BRACK_CLOSE = 141,
	AK_OPEN		= 142,
	AK_CLOSE	= 143,
	BRACK_CLOSE_OPEN = 144,//][

	IF = 150,
	ELSE = 151,
	DO = 160,
	WHILE = 161,
	SWITCH = 170,
	CASE = 171,
	DEFAULT = 172,
	BREAK = 173,
	FOR = 180,

	SMALLER = 190,
	SMALLER_EQ = 191,
	BIGGER = 192,
	BIGGER_EQ = 193,

	TYPE = 200,
	COMMA = 210,

	RETURN = 220,

	_EOL = -3,
	_EOF = -2,
	INVALID = -1
};

struct _tokenMap {
	_tokens token;
	char symbol[20];
	char keyword[20];
	int PTID;
};

extern _tokenMap tokenMapInit [];
extern const int tokenMapSize;

short int isWhiteSpace(char ch);
short int isHexDigit(int hex,char ch);
