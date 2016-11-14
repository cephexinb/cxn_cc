/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "CodeGenerator.h"
#include <stdio.h>
#include <iostream>
#include <string>


_operation operations []= {
	{ OP_MOV,	"MOV",		0,		2,		0,"",0,"",0,""},
	{ OP_SUB,	"SUB",		0,		3,		0,"",0,"",0,""},
	{ OP_ADD,	"ADD",		0,		3,		0,"",0,"",0,""},
	{ OP_MUL,	"IMUL",		0,		3,		0,"",0,"",0,""},
	{ OP_DIV,	"IDIV",		0,		1,		0,"",0,"",0,""},
	{ OP_CDQ,	"CDQ",		0,		0,		0,"",0,"",0,""},
	{ OP_JMP,	"JMP",		0,		1,		0,"",0,"",0,""},
	{ OP_JZ,	"JZ",		0,		1,		0,"",0,"",0,""},
	{ OP_JNZ,	"JNZ",		0,		1,		0,"",0,"",0,""},
	{ OP_JA,	"JA",		0,		1,		0,"",0,"",0,""},
	{ OP_JAE,	"JAE",		0,		1,		0,"",0,"",0,""},
	{ OP_JL,	"JL",		0,		1,		0,"",0,"",0,""},
	{ OP_JLE,	"JLE",		0,		1,		0,"",0,"",0,""},
	{ OP_JPE,	"JPE",		0,		1,		0,"",0,"",0,""},
	{ OP_JPO,	"JPO",		0,		1,		0,"",0,"",0,""},
	{ OP_CMP,	"CMP",		0,		2,		0,"",0,"",0,""},
	{ OP_CALL,	"CALL",		0,		1,		0,"",0,"",0,""},
	{ OP_PUSH,	"PUSH",		0,		1,		0,"",0,"",0,""}, 
	{ OP_PUSHA,	"PUSHA",	0,		0,		0,"",0,"",0,""}, 
	{ OP_POPA,	"POPA",		0,		0,		0,"",0,"",0,""}, 
	{ OP_POP,	"POP",		0,		1,		0,"",0,"",0,""},
	{ OP_LEA,	"LEA",		0,		2,		0,"",0,"",0,""},
	{ OP_NEG,	"NEG",		0,		1,		0,"",0,"",0,""},
	{ OP_FLD,	"FLD",		0,		1,		0,"",0,"",0,""},
	{ OP_FSTP,	"FSTP",		0,		1,		0,"",0,"",0,""},
	{ OP_FILD,	"FILD",		0,		1,		0,"",0,"",0,""},
	{ OP_FISUB,	"FISUB",	0,		1,		0,"",0,"",0,""},
	{ OP_FSUB,	"FSUB",		0,		1,		0,"",0,"",0,""},
	{ OP_FIMUL,	"FIMUL",	0,		1,		0,"",0,"",0,""},
	{ OP_FMUL,	"FMUL",		0,		1,		0,"",0,"",0,""},
	{ OP_FIDIV,	"FIDIV",	0,		1,		0,"",0,"",0,""},
	{ OP_FDIV,	"FDIV",		0,		1,		0,"",0,"",0,""},
	{ OP_FIADD,	"FIADD",	0,		1,		0,"",0,"",0,""},
	{ OP_FADD,	"FADD",		0,		1,		0,"",0,"",0,""},
	{ OP_SWITCH,"switch",	0,		1,		0,"",0,"",0,""},
	{ OP_CASE,	"case",		0,		1,		0,"",0,"",0,""},
	{ OP_DEFAULT,"default",	0,		0,		0,"",0,"",0,""},
	{ OP_SWEND,	"endsw",	0,		0,		0,"",0,"",0,""},
	{ OP_FUCOMPP,"FUCOMPP",	0,		0,		0,"",0,"",0,""},
	{ OP_FCOMP,	"FCOMP",	0,		1,		0,"",0,"",0,""},
	{ OP_FSTSW,	"FSTSW",	0,		1,		0,"",0,"",0,""},
	{ OP_TEST,	"TEST",		0,		2,		0,"",0,"",0,""},
	{ OP_RET,	"RET",		0,		0,		0,"",0,"",0,""}
};

const int operationsSize = sizeof(operations) / sizeof(_operation);

CodeGenerator::CodeGenerator(Scanner *scanner){
	this->initialize(scanner);
}

void CodeGenerator::initialize(Scanner *scanner){
	this->nextAddress = 0;
	this->globalSymbolTable = new SymbolTable(scanner);
	this->currentArray = NULL;
	this->scanner = scanner;
	this->ifLbl = 0;
	this->whileLbl = 0;
	this->noOfConstFloatVars = 0;
	this->doLbl = 0;
	this->forLbl = 0;
	this->PC = 0;
	this->currentFunction = NULL;

	for(int i = 0 ; i < operationsSize ; i++)
		operationsMap.insert(operationPair(operations[i].operand,operations[i]));
}

void CodeGenerator::generateCode(string action){
	_symbol *symbol;
	char tmpBuffer[1000];
	//if(action != "NoSem")
	//	printf("%s\n",action.c_str());
	if(action == "@PushType"){
		this->isDeclaration = 1;
		this->currentDeclarationType = getType(this->scanner->lastTokenValue.strVal);
	} else if(action == "@PopType"){
		this->isDeclaration = 0;
		//this->parseStack.pop(); // pop the last var defenition 
	} else if(action == "@VarDef1Push"){
		initialValueType ivt;
		ivt.intVal = 0;
		symbol = this->globalSymbolTable->declareVar(this->scanner->lastTokenValue.strVal,this->currentDeclarationType,ivt);
		this->parseStack.push(symbol);
	} else if(action == "@VarDef1ArrayStart"){
		symbol = this->parseStack.topSymbol(0);
		symbol->noOfDims = 1;
		this->currentArray = symbol;
	} else if(action == "@VarDef1Array"){
		symbol = this->parseStack.topSymbol(1);
		if(symbol->type != TYPE_INT || symbol->isConstant != 1)
			throw CompilerException("Array size must be an integer constant.",0,this->scanner->lineNo);

		_symbol *var = this->parseStack.topSymbol(1);
		if(this->currentArray->noOfDims == 1)
			var->arraySize = this->scanner->lastTokenValue.intVal;
		this->currentArray->dims[this->currentArray->noOfDims - 1] = this->scanner->lastTokenValue.intVal;

		this->globalSymbolTable->declareArray(var,var->arraySize);
		this->globalSymbolTable->releaseTemp(symbol);
		this->currentArray = NULL;
	} else if(action == "@VarDef1ArrayNextDim"){
		symbol = this->parseStack.topSymbol(1);
		if(symbol->type != TYPE_INT || symbol->isConstant != 1)
			throw CompilerException("Array size must be an integer constant.",0,this->scanner->lineNo);
		if(this->currentArray->noOfDims == 1){
			this->currentArray->arraySize = this->scanner->lastTokenValue.intVal;
			//_symbol *var = this->parseStack.topSymbol(1);
			//this->globalSymbolTable->declareArray(var,arraySize);
		}
		this->currentArray->dims[this->currentArray->noOfDims - 1] = this->scanner->lastTokenValue.intVal;
		this->currentArray->noOfDims++;
	} else if(action == "@PushConstantF1"){
		_symbol *newSymbol = new _symbol();
		_symbol *floatSymbol;
		newSymbol->isConstant = 1;
		newSymbol->type = getType(this->scanner->lastToken);
		switch(newSymbol->type){
			case TYPE_INT:
				newSymbol->initialValue.intVal = this->scanner->lastTokenValue.intVal;
				sprintf(newSymbol->name,"CINT_%d",newSymbol->initialValue.intVal);
				break;
			case TYPE_CHAR:
				newSymbol->initialValue.charVal = this->scanner->lastTokenValue.charVal;
				sprintf(newSymbol->name,"CCHAR_%c",newSymbol->initialValue.charVal);
				break;
			case TYPE_FLOAT:
				newSymbol->initialValue.floatVal = this->scanner->lastTokenValue.floatVal;
				sprintf(newSymbol->name,"CF_%d_%d",this->PC,this->noOfConstFloatVars++);
//				sprintf(newSymbol->name,"CFLOAT_%f",newSymbol->initialValue.floatVal);
				floatSymbol = this->globalSymbolTable->declareVar(newSymbol->name,TYPE_FLOAT,newSymbol->initialValue,1);
				/*this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(floatSymbol,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(newSymbol,0,1);
				this->PC++;*/
				newSymbol = floatSymbol;
				//floatSymbol->initialValue.floatVal = 0;
				break;
		}
		this->parseStack.push(newSymbol);
	} else if(action == "@ArrayNextDimEval"){
		_symbol *index = this->parseStack.topSymbol(1);		
		int size = 1;
		//if(this->currentArrayTempIndex->noOfDims != this->currentArray->noOfDims - 1) // last dimension ?
			for(int i = this->currentArrayTempIndex->noOfDims + 1 ; i < this->currentArray->noOfDims; i++)
				size *= this->currentArray->dims[i];

		this->currentArrayTempIndex->noOfDims++;

		if(index->isPointer == 1){ // it is a pointer, so its a register
			this->CODE[this->PC] = findOperation(OP_MOV);
			index->isPointer = 0;	
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(index,1,1);
			index->isPointer = 1;
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(index,0,1);
			index->isPointer = 0;
			this->PC++;
		} else if(!index->isTemp && !index->isRegister){
			_symbol *temp = this->globalSymbolTable->getTemp(index->type);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(index,0,1);
			this->PC++;
			index = temp;
		}

		this->CODE[this->PC] = findOperation(OP_MUL);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(size);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_ADD);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
		this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->PC++;
		this->globalSymbolTable->releaseTemp(index);
	} else if(action == "@ArrayIndexStart"){
		if(this->currentArray != NULL){
			this->arraysStack.push(this->currentArray);
			this->arraysStack.push(this->currentArrayTempIndex);
		}

		this->currentArray = this->parseStack.topSymbol(0);
		this->currentArrayTempIndex = this->globalSymbolTable->getTemp(TYPE_INT);
//		currentArrayTempIndex->noOfDims = this->currentArray->noOfDims - 1;
		currentArrayTempIndex->noOfDims = 0;
		this->CODE[this->PC] = findOperation(OP_MOV);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
	} else if(action == "@PushArrayF1"){
			_symbol *index = this->parseStack.topSymbol(1);		
			if(index->type != TYPE_INT)
				throw CompilerException("Array index must be an integer.",index->name,this->scanner->lineNo);

			this->CODE[this->PC] = findOperation(OP_ADD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(index,0,1);
			this->PC++;
			this->globalSymbolTable->releaseTemp(index);


			symbol = this->parseStack.topSymbol(1);
			int arrayTypeSize = getTypeSize(symbol->type);
			printf("MUL %s,%d\n",index->name,arrayTypeSize);
		
			this->CODE[this->PC] = findOperation(OP_MUL);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(arrayTypeSize);
			this->PC++;

			printf("ADD %s,%d\t(Start address of %s)\n",index->name,symbol->address,symbol->name);
			_symbol *tmpSymbol = this->globalSymbolTable->getTemp(TYPE_INT);
			this->CODE[this->PC] = findOperation(OP_LEA);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(tmpSymbol,0,1);
			if(symbol->level == 0)
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol->name);
			else
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_ADD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(this->currentArrayTempIndex,0,1);
			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(tmpSymbol,0,1);
			this->PC++;
			index->isPointer = 1; // now mark it as a pointer
			index->type = symbol->type; // it is a pointer to TYPE
			this->globalSymbolTable->releaseTemp(tmpSymbol);

			this->currentArrayTempIndex->isPointer = 1;
			this->currentArrayTempIndex->type = this->currentArray->type;
		this->parseStack.push(this->currentArrayTempIndex);

		if(this->arraysStack.getCount() != 0){
			this->currentArrayTempIndex = this->arraysStack.topSymbol(1);
			this->currentArray = this->arraysStack.topSymbol(1);
		}

/*
		_symbol *index = this->parseStack.topSymbol(1);		
		if(index->type != TYPE_INT)
			throw CompilerException("Array index must be an integer.",index->name,this->scanner->lineNo);
		symbol = this->parseStack.topSymbol(1);
		int arrayTypeSize = getTypeSize(symbol->type);
		printf("MUL %s,%d\n",index->name,arrayTypeSize);
		this->CODE[this->PC] = findOperation(OP_MUL);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(arrayTypeSize);
		this->PC++;

		printf("ADD %s,%d\t(Start address of %s)\n",index->name,symbol->address,symbol->name);
		_symbol *tmpSymbol = this->globalSymbolTable->getTemp(TYPE_INT);
		this->CODE[this->PC] = findOperation(OP_LEA);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(tmpSymbol,0,1);
		if(symbol->level == 0)
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol->name);
		else
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_ADD);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(tmpSymbol,0,1);
		this->PC++;
		index->isPointer = 1; // now mark it as a pointer
		index->type = symbol->type; // it is a pointer to TYPE
		this->globalSymbolTable->releaseTemp(tmpSymbol);
		this->parseStack.push(index);*/

	} else if(action == "@Evaluate"){
		_symbol *sym = this->parseStack.topSymbol(0);
		/*if(sym->isPointer == 1){ // it is a pointer, so its a register
			this->CODE[this->PC] = findOperation(OP_MOV);
			sym->isPointer = 0;	
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(sym,1,1);
			sym->isPointer = 1;
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(sym,0,1);
			sym->isPointer = 0;
			this->PC++;
			} else */if(!sym->isTemp && !sym->isRegister){
			_symbol *temp = this->globalSymbolTable->getTemp(sym->type);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(sym,0,1);
			this->PC++;
			this->parseStack.pop();
			this->parseStack.push(temp);
		}
	} else if(action == "@VarDef1PopDef"){
		if(this->currentDeclarationType == TYPE_VOID)
			throw CompilerException("void type is only for functions",0,this->scanner->lineNo);
		symbol = this->parseStack.topSymbol(0);
		if(symbol->isFunction) // declared as function
			throw CompilerException("id is already declared as function",symbol->name,this->scanner->lineNo);
		this->parseStack.pop();
	} else if(action == "@VarDef1NextDef"){
		if(this->currentDeclarationType == TYPE_VOID)
			throw CompilerException("void type is only for functions",0,this->scanner->lineNo);
		symbol = this->parseStack.topSymbol(0);
		if(symbol->isFunction) // declared as function
			throw CompilerException("id is already declared as function",symbol->name,this->scanner->lineNo);
		this->parseStack.pop(); 
	} else if(action == "@VarDef1FunctionDefEnd"){
		_symbol *function = this->parseStack.topSymbol(0);
		if(function->isFunction > 1) // declared twice
			throw CompilerException("function is declared twice",function->name,this->scanner->lineNo);
		this->parseStack.pop();
	} else if(action == "@VarDef1FunctionDefStart"){
		_symbol *functionName = this->parseStack.topSymbol(0);
		functionName->address = -1;
		this->globalSymbolTable->declareFunction(functionName);
	} else if(action == "@PushParamFunctionDef"){
		this->parseStack.push(getType(this->scanner->lastTokenValue.strVal));
	} else if(action == "@DefParamFunctionDef"){
		TYPES paramType = this->parseStack.topType(1);
		if(paramType == TYPE_VOID)
			throw CompilerException("void type is only for functions",0,this->scanner->lineNo);
		_symbol *functionName = this->parseStack.topSymbol(0);
		if(functionName->isFunction == 1){ // only get params at first declaration
			strcpy(functionName->functionParamsName[functionName->noOfFunctionParams],this->scanner->lastTokenValue.strVal);
			functionName->functionParams[functionName->noOfFunctionParams] = paramType;
			functionName->noOfFunctionParams++;
		}
	} else if(action == "@FunctionBodyStart"){
		_symbol *function = this->parseStack.topSymbol(0);
		if(function->functionBodyFound)
			throw CompilerException("function (body) declared twice",function->name,this->scanner->lineNo);
		function->functionBodyFound = 1;
		this->globalSymbolTable->functionResultSymbol.type = function->type;
		if(function->type == TYPE_CHAR)
			strcpy(this->globalSymbolTable->functionResultSymbol.name,getRegByte("eax"));
		else
			strcpy(this->globalSymbolTable->functionResultSymbol.name,"eax");
		this->currentFunction = function;
		function->address = this->PC;
		this->globalSymbolTable->enterBlock();

		//char *_ebp = new char[4];
		//char *_ebp = new char[4];
		//strcpy(_ebp,"ebp");
		initialValueType ivt;
		ivt.intVal = 0;

		this->CODE[this->PC] = findOperation(OP_PUSH);
		this->CODE[this->PC].isFunctionStart = 1;
		strcpy(this->CODE[this->PC].functionName,function->name);
		this->CODE[this->PC].arg0Name = "ebp";
		printf("PUSH ebp\n");
		this->PC++;

		this->CODE[this->PC] = findOperation(OP_MOV);
		this->CODE[this->PC].arg0Name = "ebp";
		this->CODE[this->PC].arg1Name = "esp";
		printf("MOV ebp,esp\n");
		this->PC++;

		this->CODE[this->PC] = findOperation(OP_PUSHA);
		this->PC++;
		if(function->type != TYPE_VOID)
			this->globalSymbolTable->declareVar("0_RESULT",function->type,ivt);

		this->CODE[this->PC] = findOperation(OP_SUB);
		this->CODE[this->PC].arg0Name = "esp";
		this->CODE[this->PC].arg1Name = "esp";
		//this->CODE[this->PC].arg1Name = "";
		printf("SUB esp,??\n"); // local vars
		function->functionEntranceSUBESPPC = this->PC;
		this->PC++;

		for(int i = 0 ; i < function->noOfFunctionParams; i++){
			_symbol *tempSym = this->globalSymbolTable->getTemp(function->functionParams[i]);
			char *address = new char[10];
			_symbol *param = this->globalSymbolTable->declareVar(function->functionParamsName[i],
					function->functionParams[i],ivt);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(tempSym,0,1);
			sprintf(address,"[ebp+%d]",(function->noOfFunctionParams + 2 - (i+1))*4);
			this->CODE[this->PC].arg1Name = address;
			printf("MOV %s,%s\n",this->globalSymbolTable->getVarAddress(tempSym,0,1),address);
			this->PC++;

			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(param,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(tempSym,0,1);
			printf("MOV %s,%s\n",this->globalSymbolTable->getVarAddress(param,0,1),address);
			this->PC++;
			this->globalSymbolTable->releaseTemp(tempSym);
		}
	} else if(action == "@GenerateReturnCode"){
//		_symbol *function = this->parseStack.topSymbol(1);
		_symbol *result = NULL;
/*		if(this->scanner->lastToken == SEMI_COLON){ // return;
			if(this->currentFunction->type != TYPE_VOID){
				sprintf(tmpBuffer,"%s must return something!",this->currentFunction->name);
				throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
			}
		}
		else if(this->currentFunction->type == TYPE_VOID){
			sprintf(tmpBuffer,"%s can't return value (void)",this->currentFunction->name);
			throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
		} else {*/
		if(this->currentFunction->type != TYPE_VOID){
			result = this->parseStack.topSymbol(1);
			printf("MOV %s,%s\n",this->globalSymbolTable->functionResultSymbol.name,result->name);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(&(this->globalSymbolTable->functionResultSymbol),0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->PC++;

			_symbol *functionResult = this->globalSymbolTable->getVarDesc("0_RESULT");
			printf("MOV %s,%s\n",this->globalSymbolTable->functionResultSymbol.name,result->name);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(functionResult,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(&(this->globalSymbolTable->functionResultSymbol),0,1	);
			this->PC++;
			this->globalSymbolTable->releaseTemp(result);
		}
		//if(this->scanner->currentToken != AK_CLOSE){ // no jump at the end of function
			this->CODE[this->PC] = findOperation(OP_JMP);
			this->addressStack.push(this->PC);
			//this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(functionResult,0,1);
			this->PC++;
		//}
	} else if(action == "@FunctionBodyEnd"){
		_symbol *function = this->parseStack.topSymbol(1);
		_symbol *result;
		if(function->isFunction == 0){
			result = function; // function return value
			function = this->parseStack.topSymbol(1);
		}
		this->currentFunction->noOfVars += this->globalSymbolTable->lastSymbolTable->noOfVars;
		this->currentFunction->noOfVars += this->globalSymbolTable->lastSymbolTable->getNoOfTemps();
		this->currentFunction->noOfVars *= 4;
		for(int i = 0 ; i < this->globalSymbolTable->lastSymbolTable->noOfVars; i++){
			if(this->globalSymbolTable->lastSymbolTable->symbols[i].arraySize != 0){
				int noOfElements = 1;
				for(int j = 0; j < this->globalSymbolTable->lastSymbolTable->symbols[i].noOfDims; j++)
					noOfElements *= this->globalSymbolTable->lastSymbolTable->symbols[i].dims[j];

				this->currentFunction->noOfVars-=4; //
				this->currentFunction->noOfVars += (noOfElements * 
					getTypeSize(this->globalSymbolTable->lastSymbolTable->symbols[i].type));
			}
		}
		while(function->noOfVars % 4 != 0)
			function->noOfVars++;
		char *tmp = new char[10];
		sprintf(tmp,"%d",function->noOfVars);
		this->CODE[function->functionEntranceSUBESPPC].arg2Name = tmp; // local vars
		this->globalSymbolTable->functionResultSymbol.type = function->type;
/*
		if(this->scanner->lastToken == SEMI_COLON){ // return;
			if(function->type != TYPE_VOID){
				sprintf(tmpBuffer,"%s must return something!",function->name);
				throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
			}
		}
		else if(function->type == TYPE_VOID){
			sprintf(tmpBuffer,"%s can't return value (void)",function->name);
			throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
		} else {
			printf("MOV %s,%s\n",this->globalSymbolTable->functionResultSymbol.name,result->name);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(&(this->globalSymbolTable->functionResultSymbol),0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->PC++;

			_symbol *functionResult = this->globalSymbolTable->getVarDesc("0_RESULT");
			printf("MOV %s,%s\n",this->globalSymbolTable->functionResultSymbol.name,result->name);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(functionResult,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(&(this->globalSymbolTable->functionResultSymbol),0,1	);
			this->PC++;
		}*/
		while(this->addressStack.getCount()){
			DWORD jmpToEnd = this->addressStack.topMisc(1);
			this->CODE[jmpToEnd].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);
		}

		printf("ADD ESP,%d\n",function->noOfVars);
		this->CODE[this->PC] = findOperation(OP_ADD);
		this->CODE[this->PC].arg1Name = "esp";
		this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(function->noOfVars);
		this->PC++;

		this->CODE[this->PC] = findOperation(OP_POPA);
		this->PC++;

		if(function->type != TYPE_VOID){
			_symbol *functionResult = this->globalSymbolTable->getVarDesc("0_RESULT");
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(&(this->globalSymbolTable->functionResultSymbol),0,1	);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(functionResult,0,1);
			this->PC++;
		}

		printf("POP EBP\n");
		this->CODE[this->PC] = findOperation(OP_POP);
		this->CODE[this->PC].arg0Name = "ebp";//_ebp;
		this->PC++;
		printf("RET\n");
		this->CODE[this->PC] = findOperation(OP_RET);
		this->CODE[this->PC].isFunctionEnd = 1;
		strcpy(this->CODE[this->PC].functionName,function->name);
		this->PC++;
		this->globalSymbolTable->exitBlock();
	} else if(action == "@PushIDF1"){
		symbol = this->globalSymbolTable->getVarDesc(this->scanner->lastTokenValue.strVal);
		if(symbol == NULL)
			throw CompilerException("Var is not defined.",this->scanner->lastTokenValue.strVal,this->scanner->lineNo);
		this->parseStack.push(symbol);
	} else if(action == "@FunctionCallStart"){
		this->parseStack.push((DWORD)0); // function parameter index
	} else if(action == "@FunctionCallParam"){
		_symbol *currentParam = this->parseStack.topSymbol(1);
		DWORD currentParameterIndex = this->parseStack.topMisc(1);
		_symbol *function = this->parseStack.topSymbol(0);
		if(currentParam->type != function->functionParams[currentParameterIndex]){
			sprintf(tmpBuffer,"Type mismatch at param %d for function %s",currentParameterIndex,function->name);
			throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
		}
		this->parseStack.push(currentParameterIndex+1);
		printf("PUSH %s\n",currentParam->name);
		this->CODE[this->PC] = findOperation(OP_PUSH);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(currentParam,0,1);
		this->PC++;
		this->globalSymbolTable->releaseTemp(currentParam);
	} else if(action == "@FunctionCallEnd"){
		DWORD currentParamIndex = this->parseStack.topMisc(1);
		_symbol *function = this->parseStack.topSymbol(1);
		_symbol *result;
		if(currentParamIndex != function->noOfFunctionParams){
			sprintf(tmpBuffer,"%s requires %d params, but got only %d params",function->name,
 				function->noOfFunctionParams,currentParamIndex);
			throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
		}
		/*if(this->globalSymbolTable->functionResultSymbol.isValid == 1){ // we can't use eax, must save it somewhere
			_symbol *temp = this->globalSymbolTable->getTemp(this->globalSymbolTable->functionResultSymbol.type);
		}*/
		this->globalSymbolTable->functionResultSymbol.type = function->type;
		this->globalSymbolTable->functionResultSymbol.isValid = 1;
		printf("CALL %s\n",function->name);
		this->CODE[this->PC] = findOperation(OP_CALL);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(function,0,1);
		char *tmpChar = new char[strlen(function->name) + 1];
		strcpy(tmpChar,function->name);
		this->CODE[this->PC].arg1Name = tmpChar;
		this->PC++;

		this->CODE[this->PC] = findOperation(OP_ADD); // FIX ESP
		this->CODE[this->PC].arg0Name = "esp";
		this->CODE[this->PC].arg1Name = "esp";
		this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(function->noOfFunctionParams * 4);
		this->PC++;

		if(function->type != TYPE_FLOAT){
			result = this->globalSymbolTable->getTemp(function->type);
		} else
			result = this->globalSymbolTable->getTemp(function->type,1);
		printf("MOV %s,%s\n",result->name,
			getRegByte(this->globalSymbolTable->functionResultSymbol.name,result->type));
		this->CODE[this->PC] = findOperation(OP_MOV);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(&(this->globalSymbolTable->functionResultSymbol),0,1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@PopFunctionRetVal"){
		_symbol *result = this->parseStack.topSymbol(1);
		this->globalSymbolTable->releaseTemp(result);
	} else if(action == "@Add"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		_symbol *result;
		_symbol *temp = NULL,*temp1 = NULL;
		checkType(symbol1,symbol2);
		if(symbol2->type == TYPE_FLOAT){
			result = this->globalSymbolTable->getTemp(symbol2->type,1); // don't get a register
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			if(symbol1->type == TYPE_FLOAT){
				this->CODE[this->PC] = findOperation(OP_FADD);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else { // TYPE_INT
				if(symbol1->isConstant){
					sprintf(symbol1->name,"CF_%d_%d",this->PC,this->noOfConstFloatVars++);
					temp1 = this->globalSymbolTable->declareVar(symbol1->name,TYPE_INT,symbol1->initialValue,1);
					this->globalSymbolTable->releaseTemp(symbol1);
					symbol1 = temp1;
				}
				this->CODE[this->PC] = findOperation(OP_FIADD);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_FSTP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->PC++;
		} else { // INT
			temp = this->globalSymbolTable->getTemp(symbol1->type);
			result = this->globalSymbolTable->getTemp(symbol1->type);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_ADD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->PC++;
			this->globalSymbolTable->releaseTemp(temp);
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		printf("ADD %s,%s,%s\n",result->name,symbol1->name,symbol2->name);
		this->parseStack.push(result);
	} else if(action == "@Sub"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		_symbol *result;
		checkType(symbol1,symbol2);
		_symbol *temp = NULL,*temp1;

		if(symbol2->type == TYPE_FLOAT){
			result = this->globalSymbolTable->getTemp(symbol2->type,1); // don't get a register
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			if(symbol1->type == TYPE_FLOAT){
				this->CODE[this->PC] = findOperation(OP_FSUB);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else { // TYPE_INT
				if(symbol1->isConstant){
					sprintf(symbol1->name,"CF_%d_%d",this->PC,this->noOfConstFloatVars++);
					temp1 = this->globalSymbolTable->declareVar(symbol1->name,TYPE_INT,symbol1->initialValue,1);
					this->globalSymbolTable->releaseTemp(symbol1);
					symbol1 = temp1;
				}
				this->CODE[this->PC] = findOperation(OP_FISUB);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_FSTP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->PC++;
		} else { // INT
			temp = this->globalSymbolTable->getTemp(symbol1->type);
			result = this->globalSymbolTable->getTemp(symbol2->type);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_SUB);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->PC++;
		}
		printf("SUB %s,%s,%s\n",result->name,symbol2->name,symbol1->name);
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		this->globalSymbolTable->releaseTemp(temp);
		this->parseStack.push(result);
	} else if(action == "@Mult"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		_symbol *result;
		_symbol *temp = NULL,*temp1 = NULL;
		checkType(symbol1,symbol2);
		if(symbol2->type == TYPE_FLOAT){
			result = this->globalSymbolTable->getTemp(symbol2->type,1); // don't get a register
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			if(symbol1->type == TYPE_FLOAT){
				this->CODE[this->PC] = findOperation(OP_FMUL);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else { // TYPE_INT
				if(symbol1->isConstant){
					sprintf(symbol1->name,"CF_%d_%d",this->PC,this->noOfConstFloatVars++);
					temp1 = this->globalSymbolTable->declareVar(symbol1->name,TYPE_INT,symbol1->initialValue,1);
					this->globalSymbolTable->releaseTemp(symbol1);
					symbol1 = temp1;
				}
				this->CODE[this->PC] = findOperation(OP_FIMUL);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_FSTP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->PC++;
		} else { // INT
			temp = this->globalSymbolTable->getTemp(symbol1->type);
			result = this->globalSymbolTable->getTemp(symbol1->type);
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_MUL);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->PC++;
			this->globalSymbolTable->releaseTemp(temp);
		}
		printf("MUL %s,%s,%s\n",result->name,symbol1->name,symbol2->name);
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		this->parseStack.push(result);
	} else if(action == "@Div"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		_symbol *result;
		checkType(symbol1,symbol2);
		_symbol *temp = NULL,*temp1;

		if(symbol2->type == TYPE_FLOAT){
			result = this->globalSymbolTable->getTemp(symbol2->type,1); // don't get a register
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			if(symbol1->type == TYPE_FLOAT){
				this->CODE[this->PC] = findOperation(OP_FDIV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else { // TYPE_INT
				if(symbol1->isConstant){
					sprintf(symbol1->name,"CF_%d_%d",this->PC,this->noOfConstFloatVars++);
					temp1 = this->globalSymbolTable->declareVar(symbol1->name,TYPE_INT,symbol1->initialValue,1);
					this->globalSymbolTable->releaseTemp(symbol1);
					symbol1 = temp1;
				}
				this->CODE[this->PC] = findOperation(OP_FIDIV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_FSTP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->PC++;
		} else { // INT
			//temp = this->globalSymbolTable->getTemp(symbol1->type);
			result = this->globalSymbolTable->getTemp(symbol2->type);
			if(strcmp(result->name,"edx")){
				this->CODE[this->PC] = findOperation(OP_PUSH);
				this->CODE[this->PC].arg0Name = "edx";
				this->PC++;
			}
			if(strcmp(result->name,"ecx")){
				this->CODE[this->PC] = findOperation(OP_PUSH);
				this->CODE[this->PC].arg0Name = "ecx";
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_MOV);
//			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->CODE[this->PC].arg0Name = "eax";
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = "ecx";
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_CDQ);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_DIV);
			this->CODE[this->PC].arg0Name = "ecx";
//			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
//			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
//			this->CODE[this->PC].arg2Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_MOV);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
			this->CODE[this->PC].arg1Name = "eax";
//			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(temp,0,1);
			this->PC++;
			if(strcmp(result->name,"ecx")){
				this->CODE[this->PC] = findOperation(OP_POP);
				this->CODE[this->PC].arg0Name = "ecx";
				this->PC++;
			}
			if(strcmp(result->name,"edx")){
				this->CODE[this->PC] = findOperation(OP_POP);
				this->CODE[this->PC].arg0Name = "edx";
				this->PC++;
			}
		}
		printf("DIV %s,%s,%s\n",result->name,symbol2->name,symbol1->name);
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		this->globalSymbolTable->releaseTemp(temp);
		this->parseStack.push(result);
	} else if(action == "@Negative"){
		symbol = this->parseStack.topSymbol(1);
		if(symbol->isConstant){
			switch(symbol->type){
				case TYPE_INT:	symbol->initialValue.intVal = -(symbol->initialValue.intVal); break;
				case TYPE_CHAR:	symbol->initialValue.charVal = -(symbol->initialValue.charVal); break;
				case TYPE_FLOAT:symbol->initialValue.floatVal = -(symbol->initialValue.floatVal); break;
			}
		} else{ 
			if(symbol->isPointer || !symbol->isTemp){ // code from evaluate
				if(symbol->isPointer == 1){ // it is a pointer, so its a register
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,1,1);
					symbol->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
					symbol->isPointer = 0;
					this->PC++;
				} else if(!symbol->isTemp){
					_symbol *temp = this->globalSymbolTable->getTemp(symbol->type);
					this->CODE[this->PC] = findOperation(OP_MOV);
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(temp,0,1);
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
					this->PC++;
					symbol = temp;
				}
			}
			this->CODE[this->PC] = findOperation(OP_NEG);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
			this->PC++;
		}
		this->parseStack.push(symbol);
	} else if(action == "@Assignment"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		if(symbol2->type == TYPE_INT && symbol1->type == TYPE_FLOAT){
			sprintf(tmpBuffer,"Can't convert int to float: %s,%s.",symbol1->name,symbol2->name);
			throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
		}
		checkType(symbol1,symbol2);
		printf("MOV [%s],%s\n",symbol2->name,symbol1->name);
		if(symbol2->type == TYPE_FLOAT){
			if(symbol1->type == TYPE_FLOAT || (symbol1->type == TYPE_INT && symbol1->isConstant)){
				if(symbol1->isConstant && symbol1->type == TYPE_INT){
					_symbol *temp1;
					sprintf(symbol1->name,"CF_%d_%d",this->PC,this->noOfConstFloatVars++);
					temp1 = this->globalSymbolTable->declareVar(symbol1->name,TYPE_INT,symbol1->initialValue,1);
					this->globalSymbolTable->releaseTemp(symbol1);
					symbol1 = temp1;
					this->CODE[this->PC] = findOperation(OP_FILD);
				} else
					this->CODE[this->PC] = findOperation(OP_FLD);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else { // TYPE_INT
				if(symbol1->isRegister){
					_symbol *regToStack = this->globalSymbolTable->getTemp(TYPE_INT,1);
					this->CODE[this->PC] = findOperation(OP_MOV);
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(regToStack,0,1);
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					this->PC++;
					this->CODE[this->PC] = findOperation(OP_FILD);
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(regToStack,0,1);
					this->PC++;
					this->globalSymbolTable->releaseTemp(regToStack);
				} else {
					this->CODE[this->PC] = findOperation(OP_FILD);
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					this->PC++;
				}
			}
			this->CODE[this->PC] = findOperation(OP_FSTP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
		} else if(symbol2->type == TYPE_CHAR){
			if((!symbol2->isRegister && !symbol1->isRegister && !symbol1->isConstant)
				|| (symbol1->isPointer && symbol2->isPointer)
				){ // can't do VAR,[ebp]
/*				if(this->globalSymbolTable->getRegStatus("edx")) {// if it is used store it
					this->CODE[this->PC] = findOperation(OP_PUSH);
					this->CODE[this->PC].arg0Name = "edx";
					this->PC++;
				}*/
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = "al";
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = "al";
				this->PC++;
/*				if(this->globalSymbolTable->getRegStatus("edx")) {// if it is used store it
					this->CODE[this->PC] = findOperation(OP_POP);
					this->CODE[this->PC].arg0Name = "edx";
					this->PC++;
				}*/
			} else {
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
		} else { // TYPE_INT
			this->CODE[this->PC] = findOperation(OP_PUSH);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_POP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
/*			if((symbol1->isPointer || !symbol1->isTemp) && (symbol2->isPointer || !symbol2->isTemp)){ // can't mov [x],[x]
				_symbol *tempSym = this->globalSymbolTable->getTemp(symbol1->type);
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(tempSym,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(tempSym,0,1);
				this->PC++;
				this->globalSymbolTable->releaseTemp(tempSym);
			} else {
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,0);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}*/
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		//this->parseStack.push(result);
	} else if(action == "@IFJZ"){
		this->globalSymbolTable->enterBlock();
		sprintf(tmpBuffer,"IFLbl%d",this->ifLbl++);
		symbol = this->parseStack.topSymbol(1);
		printf("CMP %s,0\n",symbol->name);
		this->CODE[this->PC] = findOperation(OP_CMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress((int)0);
		this->PC++;
        printf("JZ %s\n",tmpBuffer);
		this->parseStack.push(this->PC,tmpBuffer);
		this->CODE[this->PC] = findOperation(OP_JZ);
		//this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(index,0,1);
		this->PC++;
		this->globalSymbolTable->releaseTemp(symbol);
	} else if(action == "@IFCompleteJZ"){
		_label *lbl = this->parseStack.topLabel(1);
		lastIfLabel = *lbl;
		printf(" %s:\n",lbl->name);
		this->CODE[lbl->address].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);
		if(this->globalSymbolTable->lastSymbolTable->noOfVars > this->currentFunction->noOfVars)
			this->currentFunction->noOfVars = this->globalSymbolTable->lastSymbolTable->noOfVars;
		this->globalSymbolTable->exitBlock();
	} else if(action == "@IFELSE"){
		this->globalSymbolTable->enterBlock();
		sprintf(tmpBuffer,"IFLbl%d",this->ifLbl++);
        printf("JMP %s\n",tmpBuffer);
		this->parseStack.push(this->PC,tmpBuffer);
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->PC++;
		printf(" * %s:\n",lastIfLabel.name);
		this->CODE[lastIfLabel.address].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);
	} else if(action == "@IFCompleteElse"){
		_label *lbl = this->parseStack.topLabel(1);
		printf(" %s:\n",lbl->name);
		this->CODE[lbl->address].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);
		if(this->globalSymbolTable->lastSymbolTable->noOfVars > this->currentFunction->noOfVars)
			this->currentFunction->noOfVars = this->globalSymbolTable->lastSymbolTable->noOfVars;
		this->globalSymbolTable->exitBlock();
	} else if(action == "@Less"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		checkType(symbol1,symbol2);
		if((symbol1->type == TYPE_FLOAT && symbol1->type == TYPE_INT) ||
			(symbol2->type == TYPE_FLOAT && symbol1->type == TYPE_INT))
			throw CompilerException("Can't compare an int and a float",0,this->scanner->lineNo);
		else if(symbol1->type == TYPE_FLOAT){
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FCOMP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FSTSW);
			this->CODE[this->PC].arg0Name = "ax";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_TEST);
			this->CODE[this->PC].arg0Name = "ah";
			this->CODE[this->PC].arg1Name = "5h";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_JPO);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		} else{
			if((!symbol2->isRegister && !symbol2->isConstant && !symbol1->isRegister && !symbol1->isConstant)// can't do cmp [xx],[xx]
				)//|| (symbol1->isConstant && symbol2->isConstant) ) // can't do cmp 1,2 // BUG
			{ 
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else {
				if(symbol1->isPointer && symbol2->isPointer){
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol1->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,1,1);
					symbol1->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					symbol1->isPointer = 0;
					this->PC++;
				}
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_JL);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		_symbol *result = this->globalSymbolTable->getTemp(symbol1->type);
		printf("%s < %s ? -> %s\n",symbol2->name,symbol1->name,result->name);
		this->CODE[this->PC] = findOperation(OP_MOV); //FALSE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 2);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_MOV); //TRUE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@Less_EQ"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		checkType(symbol1,symbol2);
		if((symbol1->type == TYPE_FLOAT && symbol1->type == TYPE_INT) ||
			(symbol2->type == TYPE_FLOAT && symbol1->type == TYPE_INT))
			throw CompilerException("Can't compare an int and a float",0,this->scanner->lineNo);
		else if(symbol1->type == TYPE_FLOAT){
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FCOMP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FSTSW);
			this->CODE[this->PC].arg0Name = "ax";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_TEST);
			this->CODE[this->PC].arg0Name = "ah";
			this->CODE[this->PC].arg1Name = "41h";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_JPO);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		} else{
			if((!symbol2->isRegister && !symbol2->isConstant && !symbol1->isRegister && !symbol1->isConstant)// can't do cmp [xx],[xx]
				)//|| (symbol1->isConstant && symbol2->isConstant) ) // can't do cmp 1,2 // BUG
			{ 
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else {
				if(symbol1->isPointer && symbol2->isPointer){
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol1->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,1,1);
					symbol1->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					symbol1->isPointer = 0;
					this->PC++;
				}
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_JLE);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		_symbol *result = this->globalSymbolTable->getTemp(symbol1->type);
		printf("%s <= %s ? -> %s\n",symbol2->name,symbol1->name,result->name);
		this->CODE[this->PC] = findOperation(OP_MOV); //FALSE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 2);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_MOV); //TRUE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@Greater"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		checkType(symbol1,symbol2);
		if((symbol1->type == TYPE_FLOAT && symbol1->type == TYPE_INT) ||
			(symbol2->type == TYPE_FLOAT && symbol1->type == TYPE_INT))
			throw CompilerException("Can't compare an int and a float",0,this->scanner->lineNo);
		else if(symbol1->type == TYPE_FLOAT){
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FCOMP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FSTSW);
			this->CODE[this->PC].arg0Name = "ax";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_TEST);
			this->CODE[this->PC].arg0Name = "ah";
			this->CODE[this->PC].arg1Name = "41h";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_JZ);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		} else{
			if((!symbol2->isRegister && !symbol2->isConstant && !symbol1->isRegister && !symbol1->isConstant)// can't do cmp [xx],[xx]
				)//|| (symbol1->isConstant && symbol2->isConstant) ) // can't do cmp 1,2 // BUG
			{ 
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else {
				if(symbol1->isPointer && symbol2->isPointer){
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol1->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,1,1);
					symbol1->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					symbol1->isPointer = 0;
					this->PC++;
				}
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_JA);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		_symbol *result = this->globalSymbolTable->getTemp(symbol1->type);
		printf("%s > %s ? -> %s\n",symbol2->name,symbol1->name,result->name);
		this->CODE[this->PC] = findOperation(OP_MOV); //FALSE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 2);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_MOV); //TRUE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@Greater_EQ"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		checkType(symbol1,symbol2);
		if((symbol1->type == TYPE_FLOAT && symbol1->type == TYPE_INT) ||
			(symbol2->type == TYPE_FLOAT && symbol1->type == TYPE_INT))
			throw CompilerException("Can't compare an int and a float",0,this->scanner->lineNo);
		else if(symbol1->type == TYPE_FLOAT){
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FCOMP);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FSTSW);
			this->CODE[this->PC].arg0Name = "ax";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_TEST);
			this->CODE[this->PC].arg0Name = "ah";
			this->CODE[this->PC].arg1Name = "1h";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_JZ);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		} else{
			if((!symbol2->isRegister && !symbol2->isConstant && !symbol1->isRegister && !symbol1->isConstant)// can't do cmp [xx],[xx]
				)//|| (symbol1->isConstant && symbol2->isConstant) ) // can't do cmp 1,2 // BUG
			{ 
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else {
				if(symbol1->isPointer && symbol2->isPointer){
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol1->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,1,1);
					symbol1->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					symbol1->isPointer = 0;
					this->PC++;
				}
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_JAE);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		_symbol *result = this->globalSymbolTable->getTemp(symbol1->type);
		printf("%s >= %s ? -> %s\n",symbol2->name,symbol1->name,result->name);
		this->CODE[this->PC] = findOperation(OP_MOV); //FALSE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 2);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_MOV); //TRUE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@EQ"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		checkType(symbol1,symbol2);
		if((symbol1->type == TYPE_FLOAT && symbol1->type == TYPE_INT) ||
			(symbol2->type == TYPE_FLOAT && symbol1->type == TYPE_INT))
			throw CompilerException("Can't compare an int and a float",0,this->scanner->lineNo);
		else if(symbol1->type == TYPE_FLOAT){
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FUCOMPP);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FSTSW);
			this->CODE[this->PC].arg0Name = "ax";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_TEST);
			this->CODE[this->PC].arg0Name = "ah";
			this->CODE[this->PC].arg1Name = "44h";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_JPO);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		} else{
			if((!symbol2->isRegister && !symbol2->isConstant && !symbol1->isRegister && !symbol1->isConstant)// can't do cmp [xx],[xx]
				|| (symbol1->isPointer && symbol2->isPointer)
				 )//|| (symbol1->isConstant && symbol2->isConstant) ) // can't do cmp 1,2 // BUG
			{ 
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else {
				if(symbol1->isPointer && symbol2->isPointer){
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol1->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,1,1);
					symbol1->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					symbol1->isPointer = 0;
					this->PC++;
				}
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_JZ);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		_symbol *result = this->globalSymbolTable->getTemp(symbol1->type);
		printf("%s == %s ? -> %s\n",symbol2->name,symbol1->name,result->name);
		this->CODE[this->PC] = findOperation(OP_MOV); //FALSE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 2);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_MOV); //TRUE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@NotEQ"){
		_symbol *symbol1 = this->parseStack.topSymbol(1);
		_symbol *symbol2 = this->parseStack.topSymbol(1);
		checkType(symbol1,symbol2);
		if((symbol1->type == TYPE_FLOAT && symbol1->type == TYPE_INT) ||
			(symbol2->type == TYPE_FLOAT && symbol1->type == TYPE_INT))
			throw CompilerException("Can't compare an int and a float",0,this->scanner->lineNo);
		else if(symbol1->type == TYPE_FLOAT){
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FLD);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FUCOMPP);
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_FSTSW);
			this->CODE[this->PC].arg0Name = "ax";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_TEST);
			this->CODE[this->PC].arg0Name = "ah";
			this->CODE[this->PC].arg1Name = "44h";
			this->PC++;
			this->CODE[this->PC] = findOperation(OP_JPE);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		} else{
			if((!symbol2->isRegister && !symbol2->isConstant && !symbol1->isRegister && !symbol1->isConstant)// can't do cmp [xx],[xx]
				)//|| (symbol1->isConstant && symbol2->isConstant) ) // can't do cmp 1,2 // BUG
			{ 
				this->CODE[this->PC] = findOperation(OP_MOV);
				this->CODE[this->PC].arg0Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = (char *)getRegByte("eax",symbol1->type);//this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			} else {
				if(symbol1->isPointer && symbol2->isPointer){
					this->CODE[this->PC] = findOperation(OP_MOV);
					symbol1->isPointer = 0;	
					this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol1,1,1);
					symbol1->isPointer = 1;
					this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
					symbol1->isPointer = 0;
					this->PC++;
				}
				this->CODE[this->PC] = findOperation(OP_CMP);
				this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol2,0,1);
				this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(symbol1,0,1);
				this->PC++;
			}
			this->CODE[this->PC] = findOperation(OP_JNZ);
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 3);
			this->PC++;
		}
		this->globalSymbolTable->releaseTemp(symbol1);
		this->globalSymbolTable->releaseTemp(symbol2);
		_symbol *result = this->globalSymbolTable->getTemp(symbol1->type);
		printf("%s == %s ? -> %s\n",symbol2->name,symbol1->name,result->name);
		this->CODE[this->PC] = findOperation(OP_MOV); //FALSE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(this->PC + 2);
		this->PC++;
		this->CODE[this->PC] = findOperation(OP_MOV); //TRUE
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(result,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(1);
		this->PC++;
		this->parseStack.push(result);
	} else if(action == "@WhileInit"){
		sprintf(tmpBuffer,"WhileStart%d",this->whileLbl); // while start label
		this->parseStack.push(this->PC,tmpBuffer);
		printf(" %s:\n",tmpBuffer);

		sprintf(tmpBuffer,"WhileEnd%d",this->whileLbl++);
		this->parseStack.push(0,tmpBuffer); // set the address later
	} else if(action == "@WhileStart"){
		this->globalSymbolTable->enterBlock();
		symbol = this->parseStack.topSymbol(1);
		_label *lbl = this->parseStack.topLabel(0);
		printf("CMP %s,0\n",symbol->name);
		this->CODE[this->PC] = findOperation(OP_CMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		printf("JZ %s\n",lbl->name); // JZ whileEnd
		lbl->address = this->PC;
		this->CODE[this->PC] = findOperation(OP_JZ);
		//this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(lbl->address);
		this->PC++;
		this->globalSymbolTable->releaseTemp(symbol);
	} else if(action == "@WhileEnd"){
		_label *endLbl = this->parseStack.topLabel(1);
		_label *startLbl = this->parseStack.topLabel(1);
		printf("JMP %s\n",startLbl->name);
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(startLbl->address);
		this->PC++;
		printf(" %s:\n",endLbl->name);
		this->CODE[endLbl->address].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);
		if(this->globalSymbolTable->lastSymbolTable->noOfVars > this->currentFunction->noOfVars)
			this->currentFunction->noOfVars = this->globalSymbolTable->lastSymbolTable->noOfVars;
		this->globalSymbolTable->exitBlock();
	} else if(action == "@DoStart"){
		this->globalSymbolTable->enterBlock();
		sprintf(tmpBuffer,"DoStart%d",this->doLbl++);
		this->parseStack.push(this->PC,tmpBuffer);
		printf(" %s:\n",tmpBuffer);
	} else if(action == "@DoEnd"){
		symbol = this->parseStack.topSymbol(1);
		printf("CMP %s,0\n",symbol->name);
		this->CODE[this->PC] = findOperation(OP_CMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;
		_label *startLbl = this->parseStack.topLabel(1);

		printf("JNZ %s\n",startLbl->name);
		this->CODE[this->PC] = findOperation(OP_JNZ);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(startLbl->address);
		this->PC++;
		if(this->globalSymbolTable->lastSymbolTable->noOfVars > this->currentFunction->noOfVars)
			this->currentFunction->noOfVars = this->globalSymbolTable->lastSymbolTable->noOfVars;
		this->globalSymbolTable->exitBlock();
		this->globalSymbolTable->releaseTemp(symbol);
	} else if(action == "@EnterFor"){
		this->globalSymbolTable->enterBlock();
	} else if(action == "@ForCondStart"){
		sprintf(tmpBuffer,"ForCond-%d",this->forLbl);
		this->parseStack.push(this->PC,tmpBuffer);
		printf(" %s:\n",tmpBuffer);
	} else if(action == "@ForCondEnd"){
		symbol = this->parseStack.topSymbol(1);
		printf("CMP %s,0\n",symbol->name);
		this->CODE[this->PC] = findOperation(OP_CMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
		this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(0);
		this->PC++;

		sprintf(tmpBuffer,"ForEnd-%d",this->forLbl);
		this->parseStack.push(this->PC,tmpBuffer);
//		_label *forEnd = this->parseStack.topLabel(0);
//		forEnd->address1 = 
		printf("JZ %s\n",tmpBuffer);
		this->CODE[this->PC] = findOperation(OP_JZ);
		//this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(symbol,0,1);
		this->PC++;

		sprintf(tmpBuffer,"ForBody-%d",this->forLbl);
		this->parseStack.push(this->PC,tmpBuffer);
		printf("JMP %s\n",tmpBuffer);
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->PC++;

		sprintf(tmpBuffer,"ForJob-%d",this->forLbl++);
		this->parseStack.push(this->PC,tmpBuffer);
		printf(" %s:\n",tmpBuffer);
		this->globalSymbolTable->releaseTemp(symbol);
	} else if(action == "@ForJobEnd"){
		_label *forJob = this->parseStack.topLabel(1);
		_label *forBody = this->parseStack.topLabel(1);
		_label *forEnd = this->parseStack.topLabel(1);
		_label *forCond = this->parseStack.topLabel(1);
		printf("JMP %s\n",forCond->name);
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(forCond->address);
		this->PC++;

		this->CODE[forBody->address].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);

		this->parseStack.push(forEnd->address,forEnd->name);
//		this->parseStack.push(0,forBody->name);
		printf(" %s:\n",forBody->name);
		this->parseStack.push(forJob->address,forJob->name);
	} else if(action == "@ForEnd"){
		_label *forJob = this->parseStack.topLabel(1);
		_label *forEnd = this->parseStack.topLabel(1);
		printf("JMP %s\n",forJob->name);
		this->CODE[this->PC] = findOperation(OP_JMP);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(forJob->address);
		this->PC++;
		printf(" %s:\n",forEnd->name);
		this->CODE[forEnd->address].arg0Name = this->globalSymbolTable->getVarAddress(this->PC);
		this->globalSymbolTable->exitBlock();
	} else if(action == "@SwitchStart"){
		_symbol *switchVar = this->parseStack.topSymbol(1);
		if(switchVar->isPointer){ // code from evaluate
			this->CODE[this->PC] = findOperation(OP_MOV);
			switchVar->isPointer = 0;	
			this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(switchVar,1,1);
			switchVar->isPointer = 1;
			this->CODE[this->PC].arg1Name = this->globalSymbolTable->getVarAddress(switchVar,0,1);
			switchVar->isPointer = 0;
			this->PC++;
		}
		if(switchVar->type != TYPE_INT)
			throw CompilerException("switch var should be an int",0,this->scanner->lineNo);
		this->switchDefaultHasBeenSeen = 0;
		this->CODE[this->PC] = findOperation(OP_SWITCH);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(switchVar,0,1);
		this->PC++;
		this->globalSymbolTable->enterBlock();
	} else if(action == "@SwitchCase"){
		if(this->switchDefaultHasBeenSeen)
			throw CompilerException("Can't add case after default",0,this->scanner->lineNo);
		_symbol *caseConst = this->parseStack.topSymbol(1);
		this->CODE[this->PC] = findOperation(OP_CASE);
		this->CODE[this->PC].arg0Name = this->globalSymbolTable->getVarAddress(caseConst,0,1);
		this->PC++;
	} else if(action == "@SwitchDefault"){
		if(this->switchDefaultHasBeenSeen)
			throw CompilerException("2 defaults!",0,this->scanner->lineNo);
		this->CODE[this->PC] = findOperation(OP_DEFAULT);
		this->PC++;
	} else if(action == "@SwitchEnd"){
		this->CODE[this->PC] = findOperation(OP_SWEND);
		this->PC++;
		this->globalSymbolTable->exitBlock();
	}
}
void CodeGenerator::checkType(_symbol *s1,_symbol *s2){
	return this->checkType(s1,s2,0);
}

void CodeGenerator::checkType(_symbol *s1,_symbol *s2,int forceType){
	char tmpBuffer[200];
	if(forceType && s1->type != s2->type){
		sprintf(tmpBuffer,"Incomatible types: %s,%s.",s1->name,s2->name);
		throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
	}
	if(s1->type == TYPE_INT && s2->type == TYPE_FLOAT)
		return;
	if(s2->type == TYPE_INT && s1->type == TYPE_FLOAT)
		return;
	if(s1->type != s2->type){
		sprintf(tmpBuffer,"Incomatible types: %s,%s.",s1->name,s2->name);
		throw CompilerException(tmpBuffer,0,this->scanner->lineNo);
	}
}

_operation CodeGenerator::findOperation(_operand operand){
	this->operationIterator = this->operationsMap.find(operand);
	if(this->operationIterator == this->operationsMap.end())
		throw CompilerException("Unknown operation !",0,this->scanner->lineNo);

	return this->operationIterator->second;
}

void CodeGenerator::generateAsm(const char *path){
	FILE *o = fopen(path,"wt");
	int mainFuncFound = 0;

	// header
	fprintf(o,"%s",".386\n.model flat,stdcall\noption casemap:none\n\ninclude .\\include\\kernel32.inc\nincludelib .\\lib\\kernel32.lib\n");
	fprintf(o,"%s","include .\\switch.inc\n\n\n");

	// protos
	for(int i = 0; this->globalSymbolTable->symbols[i].isValid; i++){
		if(!this->globalSymbolTable->symbols[i].isFunction)
			continue;
		fprintf(o,"%s\t\tPROTO\n",this->globalSymbolTable->symbols[i].name);
	}
	fprintf(o,"\n");


	// data
	fprintf(o,".data\n");
	for(int i = 0; this->globalSymbolTable->symbols[i].isValid; i++){
		if(this->globalSymbolTable->symbols[i].isTemp 
			|| this->globalSymbolTable->symbols[i].isConstant
			|| this->globalSymbolTable->symbols[i].isFunction
			)
			continue;
		if(this->globalSymbolTable->symbols[i].arraySize == 0){
			if(this->globalSymbolTable->symbols[i].type != TYPE_CHAR)
				fprintf(o,"%s\t\tDWORD 0%Xh\n",this->globalSymbolTable->symbols[i].name,this->globalSymbolTable->symbols[i].initialValue.intVal);
			else
				fprintf(o,"%s\t\tBYTE 0%Xh\n",this->globalSymbolTable->symbols[i].name,this->globalSymbolTable->symbols[i].initialValue.intVal);
		} else {
			int noOfElements = 1;
			for(int j = 0; j < this->globalSymbolTable->lastSymbolTable->symbols[i].noOfDims; j++)
				noOfElements *= this->globalSymbolTable->lastSymbolTable->symbols[i].dims[j];
			switch(this->globalSymbolTable->symbols[i].type){
				case TYPE_INT:
				case TYPE_FLOAT:
					fprintf(o,"%s\t\tDWORD %d dup(0)\n",this->globalSymbolTable->symbols[i].name,noOfElements);
					break;
				case TYPE_CHAR:
					fprintf(o,"%s\t\tBYTE %d dup(0)\n",this->globalSymbolTable->symbols[i].name,noOfElements);
					break;
			}
			
		}
	}
	fprintf(o,"\n");

	// code initalization
	for(int i = 0 ; i < this->PC; i++){
		int jmpAddress;
		char jmpLabel[20];
		char *jmpType = NULL;
		int isJmp = 0;
		switch(this->CODE[i].operand){
			case OP_JMP: isJmp = 1; jmpType = "JMP"; break;
			case OP_JZ: isJmp = 1; jmpType = "JZ"; break;
			case OP_JNZ: isJmp = 1; jmpType = "JNZ"; break;
			case OP_JA: isJmp = 1; jmpType = "JA"; break;
			case OP_JAE: isJmp = 1; jmpType = "JAE"; break;
			case OP_JL: isJmp = 1; jmpType = "JL"; break;
			case OP_JLE: isJmp = 1; jmpType = "JLE"; break;
			case OP_JPE: isJmp = 1; jmpType = "JPE"; break;
			case OP_JPO: isJmp = 1; jmpType = "JPO"; break;
			default:
				continue;
		}
		if(isJmp){
				sscanf(this->CODE[i].arg0Name,"%d",&jmpAddress);
				//jmpLabel = new char[20];
				sprintf(jmpLabel,"%s_%d",jmpType,i);
				this->CODE[jmpAddress].hasLabel = 1;
				strcpy(this->CODE[jmpAddress].labelName,jmpLabel);
				this->CODE[i].arg0Name = &(this->CODE[jmpAddress].labelName[0]);
		}
	}

	// code
	fprintf(o,"%s",".code\nstart:\n\tcall main\n\tinvoke ExitProcess,eax\n\n");
	for(int i = 0 ; i < this->PC; i++){
		if(this->CODE[i].isFunctionStart){
			if(!strcmp(this->CODE[i].functionName,"main"))
				mainFuncFound = 1;
			fprintf(o,"%s\tPROC\n",this->CODE[i].functionName);
		}
		if(this->CODE[i].hasLabel)
			fprintf(o,"%s:\n",this->CODE[i].labelName);
		switch(this->CODE[i].noOfArgs){
			case 0:
				fprintf(o,"\t%s\n",this->CODE[i].symbol); break;
			case 1:
				if(this->CODE[i].operand == OP_CALL){
					fprintf(o,"\tCALL %s\n",this->CODE[i].arg1Name); break;
				} else {
					fprintf(o,"\t%s %s\n",this->CODE[i].symbol,this->CODE[i].arg0Name); break;
				}
			case 2:
				fprintf(o,"\t%s %s,%s\n",this->CODE[i].symbol,this->CODE[i].arg0Name,this->CODE[i].arg1Name); break;
			case 3:
				fprintf(o,"\t%s %s,%s\n",this->CODE[i].symbol,
					this->CODE[i].arg1Name,this->CODE[i].arg2Name); break;
		}
		if(this->CODE[i].isFunctionEnd)
			fprintf(o,"%s\tENDP\n\n",this->CODE[i].functionName);
	}

	fprintf(o,"%s","end start\n");

	fclose(o);

	if(!mainFuncFound){
		fprintf(stderr,"** Warning! \"main\" function not found.\n");
	}
}
