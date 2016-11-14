/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"
#include "CompilerException.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


void fatalError(const char *format,...){
	va_list args;
	va_start(args,format);

	char buffer[4096];
	_vsnprintf(buffer,sizeof(buffer),format,args);
	fprintf(stderr,"Fatal Error:\n%s",buffer);
	va_end(args);
	exit(3);
}

TYPES getType(const char *name){
	if(!strcmp(name,"int"))
		return TYPE_INT;
	if(!strcmp(name,"char"))
		return TYPE_CHAR;
	if(!strcmp(name,"float"))
		return TYPE_FLOAT;
	if(!strcmp(name,"void"))
		return TYPE_VOID;
	throw CompilerException("Invalid type name.",name,0);
}

TYPES getType(_tokens token){
	switch(token){
		case NUMBER_INT:
			return TYPE_INT;
		case CHARACTER:
			return TYPE_CHAR;
		case NUMBER_FLOAT:
			return TYPE_FLOAT;
		case VOID:
			return TYPE_VOID;
	}
	throw CompilerException("Invalid type.",0,0);
}

int getTypeSize(TYPES type){
	switch(type){
		case TYPE_INT:
			return 4;
		case TYPE_CHAR:
			return 1;
		case TYPE_FLOAT:
			return sizeof(float);
		case VOID:
			return 0;
		default:
			return 4;
	}
}
const char *getRegByte(const char *regName){
	if(!strcmp(regName,"eax"))
		return "al";
	if(!strcmp(regName,"ebx"))
		return "bl";
	if(!strcmp(regName,"ecx"))
		return "cl";
	if(!strcmp(regName,"edx"))
		return "dl";
	return regName;
}

const char *getRegByte(const char *regName,TYPES type){
	if(type != TYPE_CHAR)
		return regName;
	if(!strcmp(regName,"eax"))
		return "al";
	if(!strcmp(regName,"ebx"))
		return "bl";
	if(!strcmp(regName,"ecx"))
		return "cl";
	if(!strcmp(regName,"edx"))
		return "dl";
	return regName;
}
