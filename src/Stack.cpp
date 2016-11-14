/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Stack.h"
#include "CompilerException.h"
#include <string.h>

Stack::Stack(){
	this->initialize();
}

void Stack::initialize(){
	this->dataPointer = 0;
}

DWORD Stack::getCount(){
	return this->dataPointer;
}

void Stack::push(_symbol *symbol){
	if(this->dataPointer == 2047)
		throw CompilerException("Stack::push\tStack overflow !",symbol->name,0);
	this->data[this->dataPointer].type = SET_SYMBOL;
	this->data[this->dataPointer++].data.symbol = symbol;
}

void Stack::push(TYPES type){
	if(this->dataPointer == 2047)
		throw CompilerException("Stack::push\tStack overflow !",0,0);
	this->data[this->dataPointer].type = SET_TYPE;
	this->data[this->dataPointer++].data.type = type;
}

void Stack::push(DWORD address,char *name){
	if(this->dataPointer == 2047)
		throw CompilerException("Stack::push\tStack overflow !",name,0);
	this->data[this->dataPointer].type = SET_LABEL;
	strcpy(this->data[this->dataPointer].data.label.name,name);
	this->data[this->dataPointer++].data.label.address = address;
}

void Stack::push(DWORD misc){
	if(this->dataPointer == 2047)
		throw CompilerException("Stack::push\tStack overflow !",0,0);
	this->data[this->dataPointer].type = SET_MISC;
	this->data[this->dataPointer++].data.miscData = misc;
}

_symbol *Stack::topSymbol(int doPop){
	if(this->dataPointer == 0)
		throw CompilerException("Stack::topSymbol\tStack underflow",0,0);
	if(this->data[this->dataPointer - 1].type != SET_SYMBOL)
		throw CompilerException("Stack::top\tStack top is not a symbol",0,0);
	if(doPop)
		return this->data[--this->dataPointer].data.symbol;
	else
		return this->data[this->dataPointer - 1].data.symbol;
}

_label *Stack::topLabel(int doPop){
	if(this->dataPointer == 0)
		throw CompilerException("Stack::topLabel\tStack underflow",0,0);
	if(this->data[this->dataPointer - 1].type != SET_LABEL)
		throw CompilerException("Stack::top\tStack top is not a label",0,0);
	if(doPop)
		return &(this->data[--this->dataPointer].data.label);
	else
		return &(this->data[this->dataPointer - 1].data.label);
}

TYPES Stack::topType(int doPop){
	if(this->dataPointer == 0)
		throw CompilerException("Stack::topType\tStack underflow",0,0);
	if(this->data[this->dataPointer - 1].type != SET_TYPE)
		throw CompilerException("Stack::top\tStack top is not a type",0,0);
	if(doPop)
		return (this->data[--this->dataPointer].data.type);
	else
		return (this->data[this->dataPointer - 1].data.type);
}

DWORD Stack::topMisc(int doPop){
	if(this->dataPointer == 0)
		throw CompilerException("Stack::topType\tStack underflow",0,0);
	if(this->data[this->dataPointer - 1].type != SET_MISC)
		throw CompilerException("Stack::top\tStack top is not a misc data",0,0);
	if(doPop)
		return (this->data[--this->dataPointer].data.miscData);
	else
		return (this->data[this->dataPointer - 1].data.miscData);
}

void Stack::pop(){
	if(this->dataPointer == 0)
		throw CompilerException("Stack::top\tStack underflow",0,0);
	this->dataPointer--;
}
StackElementType Stack::getTopType(){
	if(this->dataPointer == 0)
		throw CompilerException("Stack::getTopType\tStack underflow",0,0);
	return this->data[this->dataPointer - 1].type;
}
