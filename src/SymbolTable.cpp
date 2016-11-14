/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "SymbolTable.h"
#include "CompilerException.h"
#include <stdio.h>
#include <string.h>

Scanner *SymbolTable::scanner = NULL;

SymbolTable::SymbolTable(Scanner *scanner){
	this->initialize(NULL,scanner);
}

SymbolTable::SymbolTable(SymbolTable *st){
	this->initialize(st,NULL);
}

void SymbolTable::initialize(SymbolTable *st,Scanner *scanner){
	if(scanner != NULL)
		this->scanner = scanner;
	this->parentSymbolTable = st;
	this->maxNoOfVars = 0;
	this->noUsedRegs = 0;
	this->noOfVars = 0;
	this->noOfLocalStackTemps = 0;
	this->maxNoOfLocalStackTemps = 0;
	for(int i = 0 ; i < 5 ; i++)
		this->regsStatus[i] = 0;
	if(st != NULL){
		this->nextAddress = st->nextAddress;
		this->level = st->level + 1;
	}
	else {
		this->nextAddress = 0;
		this->level = 0;
	}
	this->symbolsPointer = 0;
	memset(&(this->symbols),0,sizeof(this->symbols));
	this->tempsPointer = 0;
	memset(&(this->temps),0,sizeof(this->temps));
	this->noTemps = 0;
	this->lastSymbolTable = this;
	memset(&(this->functionResultSymbol),0,sizeof(this->functionResultSymbol));
	strcpy(this->functionResultSymbol.name,"eax");
	this->functionResultSymbol.isValid = 1;
	this->functionResultSymbol.isRegister = 1;
}

char *SymbolTable::getVarAddress(_symbol *symbol,int effectiveAddress,int effectiveAddressOnReg){
	char *result = new char[30];
	char *sizePtr = NULL;
	if(symbol->type == TYPE_CHAR)
		sizePtr = "BYTE PTR";
	else
		sizePtr = "DWORD PTR";
	strcpy(result,"INVALID MODE");
	unsigned int *floatVal;
	if(symbol->isConstant){
		switch(symbol->type){
			case TYPE_INT:
				sprintf(result,"%d",symbol->initialValue.intVal); break;
			case TYPE_CHAR:
				sprintf(result,"'%c'",symbol->initialValue.charVal); break;
			case TYPE_FLOAT:
				floatVal = (unsigned int *)(&symbol->initialValue.floatVal);
				sprintf(result,"%Xh",*floatVal); break;
			default:
				sprintf(result,"%d",symbol->initialValue.intVal); break;
		}	
	} else if((symbol->isTemp || !strcmp(symbol->name,"eax")) && symbol->isRegister) { // it is a register
		if((!effectiveAddress && !effectiveAddressOnReg) || (symbol->isPointer == 1)) 
			sprintf(result,"%s [%s]",sizePtr,symbol->name); // we will never use a register for char type, so this is correct
		else {
			if(symbol->type == TYPE_CHAR)
				strcpy(result,getRegByte(symbol->name));
			else strcpy(result,symbol->name);
		}
	} else {
		if(symbol->isTemp){
			if(effectiveAddress)
				sprintf(result,"%s_EBP-%d",symbol->name,symbol->address * 4); // TODO: fix size
			else {
				sprintf(result,"%s [ebp-%d]",sizePtr,symbol->address + 8*4); // TODO: fix size
			}
		} else {
			if(effectiveAddress)
				sprintf(result,"%s_%d",symbol->name,symbol->address);
			else {
				if(symbol->level > 0) // local variable of some block
					sprintf(result,"%s [ebp-%d]",sizePtr,symbol->address + 8*4); // 4 for ebp, 8*4 for pusha
				else
					sprintf(result,"%s",symbol->name);
			}
		}
	}
	return result;
}

char *SymbolTable::getVarAddress(int constant){
	char *result = new char[30];
	sprintf(result,"%d",constant);
	return result;
}

_symbol *SymbolTable::declareVar(const char *name,TYPES type,initialValueType initialValue){
	return this->declareVar(name,type,initialValue,0);
}

_symbol *SymbolTable::declareVar(const char *name,TYPES type,initialValueType initialValue,int declareInGlobal){
	SymbolTable *st = this->lastSymbolTable;
	if(declareInGlobal)
		st = this;
	int i=0;
	for(i = 0 ; i < 512 && st->symbols[i].isValid; i++){
		if(!strcmp(st->symbols[i].name,name)){
			if(st->symbols[i].isFunction && !st->symbols[i].functionBodyFound){
				st->symbols[i].isFunction++;
				return &(st->symbols[i]);
			}
			char tmp[100];
			sprintf(tmp,"Variable is already defined at line %d.",st->symbols[i].defLineNo);
			throw CompilerException(tmp,name,this->scanner->lineNo);
		}
	}
	st->noOfVars++;
	st->updateMaxNoOfVars(i+1);
	memset(&(st->symbols[i]),0,sizeof(st->symbols[0]));
	//sprintf(st->symbols[i].name,"%s_%d",name,this->nextAddress);
	strcpy(st->symbols[i].name,name);
	printf("\t\t\t\t** VAR DECL: %s_%d\n",name,st->nextAddress);
	st->symbols[i].level = st->level;
	st->symbols[i].isValid = 1;
	st->symbols[i].initialValue = initialValue;
	st->symbols[i].type = type;
	if(type != TYPE_CHAR)
		while(st->nextAddress % 4 != 0)
			st->nextAddress++;

//	st->symbols[i].address = st->nextAddress;
	st->symbols[i].isTemp = 0;
	st->symbols[i].arraySize = 0;
	st->symbols[i].noOfDims = 0;
	st->symbols[i].isFunction = 0;
	switch(type){
		case TYPE_INT:	st->nextAddress += 4; break;
		case TYPE_CHAR: st->nextAddress += 1; break;
		case TYPE_STRING: st->nextAddress += (int)strlen(initialValue.strVal) + 1; break;
		case TYPE_FLOAT: st->nextAddress += sizeof(float); break;
		default: st->nextAddress += 4; break;
	}
	st->symbols[i].address = st->nextAddress;
	st->symbols[i].defLineNo = this->scanner->lineNo;
	return &(st->symbols[i]);
}

int SymbolTable::isVarDeclaredInThisBlock(const char *name){
/*	for(int i = 0 ; i < 512 && this->symbols[i].isValid; i++){
		if(!strcmp(this->symbols[i].name,name))
			return 1;
	}*/
	printf("Not Implemented.\n");
	return 0;
}

int SymbolTable::isVarDeclared(const char *name){
	SymbolTable *st = this->lastSymbolTable;
	int finish = 0;
	do {
		if(st == this)
			finish = 1;
		for(int i = 0 ; i < 512 && st->symbols[i].isValid; i++){
			if(!strcmp(st->symbols[i].name,name))
				return 1;
		}
		st = st->parentSymbolTable;
	} while(!finish);
	return 0;
}

/*_symbol SymbolTable::getVar(const char *name){
	if(this->parentSymbolTable == NULL)
		return this->lastSymbolTable->getVar(name);
	
	for(int i = 0 ; i < 512 && this->symbols[i].isValid; i++){
		if(!strcmp(this->symbols[i].name,name))
			return symbols[i];
	}
	if(this->parentSymbolTable != NULL)
		return this->parentSymbolTable->getVar(name);
	else
		throw CompilerException("Var is not defined.",name,this->scanner->lineNo);
}*/

_symbol *SymbolTable::getVarDesc(const char *name){
	return this->lastSymbolTable->getVarDescInternal(name);
}

_symbol *SymbolTable::getVarDescInternal(const char *name){
	for(int i = 0 ; i < 512 && this->symbols[i].isValid; i++){
		if(!strcmp(this->symbols[i].name,name))
			return &symbols[i];
	}
	if(this->parentSymbolTable != NULL)
		return this->parentSymbolTable->getVarDescInternal(name);
	else
		return NULL;
}

_symbol *SymbolTable::getTemp(TYPES type){
	if(type == TYPE_CHAR)
		return this->getTemp(type,1); // dont return ebx // tofix: return bl
	return this->getTemp(type,0);
}

_symbol *SymbolTable::getTemp(TYPES type,int doNotUseReg){
	if(this->level == 0){ // root node
		if(this->lastSymbolTable != this){
			return this->lastSymbolTable->getTemp(type,doNotUseReg);
		}
	} else if(this->level > 1){
		return this->parentSymbolTable->getTemp(type,doNotUseReg);
	}
//	const char regNames[][5] = { "ebx","ecx","edx","edi","esi" };
	int i;
	for(i = 0 ; i < 512 && this->temps[i].isValid; i++);
	memset(&(this->temps[i]),0,sizeof(this->temps[0]));
/*	if(this->noTemps > 4)
		sprintf(this->temps[i].name,"TMP%d",this->noTemps);
	else
		strcpy(this->temps[i].name,regNames[i]);*/
	if(this->noUsedRegs > 4 || doNotUseReg){
		this->noOfLocalStackTemps++;
		if(this->noOfLocalStackTemps > this->maxNoOfLocalStackTemps)
			this->maxNoOfLocalStackTemps = this->noOfLocalStackTemps;
		sprintf(this->temps[i].name,"TMP%d",this->noTemps);
	}
	else {
//		strcpy(this->temps[i].name,regNames[this->noUsedRegs]);
		strcpy(this->temps[i].name,this->getFreeReg());
		//this->noUsedRegs++;
		this->temps[i].isRegister = 1;
	}
	this->noTemps++;
	this->temps[i].isValid = 1;
	this->temps[i].initialValue.intVal = 0;
	this->temps[i].type = type;
	this->temps[i].isTemp = 1;
	this->temps[i].isPointer = 0;
//	this->temps[i].address = this->nextAddress;
	if(!this->temps[i].isRegister){
		switch(type){
			case TYPE_INT:	this->nextAddress += 4; break;
			case TYPE_CHAR: this->nextAddress += 1; break;
	//		case TYPE_STRING: this->nextAddress += (int)strlen(initialValue.strVal) + 1; break;
			case TYPE_FLOAT: this->nextAddress += sizeof(float); break;
			default: this->nextAddress += 4; break;
		}
	}
	this->temps[i].address = this->nextAddress;
	this->temps[i].defLineNo = this->scanner->lineNo;
	return &(this->temps[i]);
}

void SymbolTable::releaseTemp(_symbol *temp){
	if(temp == NULL)
		return;
	if(!temp->isTemp)
		return;
	if(this->level == 0){ // root node
		if(this->lastSymbolTable != this){
			return this->lastSymbolTable->releaseTemp(temp);
		}
	} else if(this->level > 1){
		return this->parentSymbolTable->releaseTemp(temp);
	}
	int i;
	for(i = 0 ; i < 512 && this->temps[i].isValid; i++);
	i--;
	temp->isValid = 0;
	if(strcmp(this->temps[i].name,temp->name))
		fprintf(stderr,"\t\t** WARNING! releaseTemp: last temp is not the one being free!!\n");
	//this->temps[i].isValid = 0;
	if(temp->isRegister)
		this->freeReg(temp->name);
		//this->noUsedRegs--;
	else {
		this->noOfLocalStackTemps--;
		this->nextAddress -= 4;
	}
	temp->isValid = 0;
	this->noTemps--;
	//printf("Release Temp\n");
}

void SymbolTable::enterBlock(){
	SymbolTable *st = new SymbolTable(this->lastSymbolTable);
	if(lastSymbolTable == this){
		st->nextAddress = 0;
		st->noTemps = 0;
		st->maxNoOfLocalStackTemps = 0;
	}
	lastSymbolTable = st;
}

void SymbolTable::exitBlock(){
	SymbolTable *st = this->lastSymbolTable;
	if(lastSymbolTable == this)
		throw CompilerException("SymbolTable::exitBlock\tNo block to exit!",0,this->scanner->lineNo);

	this->lastSymbolTable = this->lastSymbolTable->parentSymbolTable;
	delete st;
}

void SymbolTable::declareArray(_symbol *symbol,int arraySize){
	//if(this->level > 1)
	//	return this->parentSymbolTable->declareArray(symbol,arraySize);
	SymbolTable *st = this->lastSymbolTable;
	symbol->arraySize = arraySize;
	int typeSize = 0,arrayElements = 1;
	switch(symbol->type){
		case TYPE_INT:	st->nextAddress -= 4; typeSize = 4;break;
		case TYPE_CHAR: st->nextAddress -= 1; typeSize = 1;break;
		//case TYPE_STRING: st->nextAddress += (int)strlen(initialValue.strVal) + 1; break;
		case TYPE_FLOAT: st->nextAddress -= sizeof(float); typeSize = sizeof(float);break;
		default: st->nextAddress -= 4; typeSize = 4;break;
	}
	if(symbol->type != TYPE_CHAR)
		while(st->nextAddress % 4 != 0) this->nextAddress++;
	for(int i = 0; i < symbol->noOfDims; i++)
		arrayElements *= symbol->dims[i];
	st->nextAddress += (typeSize * arrayElements);
//	symbol->address = (st->nextAddress - typeSize);
	symbol->address = (st->nextAddress);
}

void SymbolTable::declareFunction(_symbol *symbol){
	symbol->isFunction++;
	if(symbol->isFunction == 1) {// first declaration
		symbol->noOfFunctionParams = 0;
		this->lastSymbolTable->nextAddress -= getTypeSize(symbol->type);
	}
}

int SymbolTable::getNoOfTemps(){
	if(this->level > 1)
		return this->parentSymbolTable->getNoOfTemps();
	if(this->noTemps > 4)
		return this->noTemps - 4 + this->maxNoOfLocalStackTemps;
	return this->maxNoOfLocalStackTemps;
}
int SymbolTable::getMaxNoOfVars(){
	if(this->level == 0)
		return this->maxNoOfVars;
	else if(this->level > 1)
		return this->parentSymbolTable->getMaxNoOfVars();
	return this->maxNoOfVars;
}

void SymbolTable::updateMaxNoOfVars(int noOfVars){
/*	if(this->level > 1)
		return this->parentSymbolTable->updateMaxNoOfVars(noOfVars);
	if(noOfVars > this->maxNoOfVars)
		this->maxNoOfVars = noOfVars;*/
}
char *SymbolTable::getVarAddress(char *symName){
	char *res = new char [strlen(symName) + 1];
	strcpy(res,symName);
	return res;
}

const char *SymbolTable::getFreeReg(){
	if(this->noUsedRegs > 4)
		throw CompilerException("Can't find a free register!",0,this->scanner->lineNo);
	const char regNames[][5] = { "ebx","ecx","edx","edi","esi" };
	for(int i = 0 ; i <= 4 ; i++)
		if(!this->regsStatus[i]){
			this->regsStatus[i] = 1;
			this->noUsedRegs++;
			return regNames[i];
		}
}

void SymbolTable::freeReg(const char *regName){
	const char regNames[][5] = { "ebx","ecx","edx","edi","esi" };
	for(int i = 0 ; i <= 4 ; i++)
		if(!strcmp(regName,regNames[i])){
			this->regsStatus[i] = 0;
			this->noUsedRegs--;
			return;
		}
}

int SymbolTable::getRegStatus(const char *regName){
	const char regNames[][5] = { "ebx","ecx","edx","edi","esi" };
	for(int i = 0 ; i <= 4 ; i++)
		if(!strcmp(regName,regNames[i]))
			return this->regsStatus[i];
}
