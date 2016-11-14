/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#ifndef DWORD
#define DWORD unsigned int
#endif

#define MAX_STRING_LEN 128

void fatalError(const char *format,...);

enum TYPES { TYPE_INT,TYPE_CHAR,TYPE_STRING,TYPE_FLOAT,TYPE_VOID };
union initialValueType {
	int intVal;
	char charVal;
	char strVal[128];
	float floatVal;
};


#include "Token.h"

enum _tokens;
TYPES getType(const char *name);
TYPES getType(_tokens token);
int getTypeSize(TYPES type);
const char *getRegByte(const char *regName,TYPES type);
const char *getRegByte(const char *regName);
#include "CompilerException.h"
