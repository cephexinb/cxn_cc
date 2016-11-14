/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Compiler.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "CompilerException.h"
#include "Token.h"


Compiler::Compiler(const char *inputFileName){
	this->initialize(inputFileName);
}

void Compiler::initialize(const char *inputFileName){
	this->scanner = new Scanner(inputFileName);
}


char Compiler::compile(){
	_tokens currentToken;
	do {
		currentToken = 	this->scanner->scan();
		
		scanner->tokenIterator = scanner->tokensMap.find(currentToken);
		if(scanner->tokenIterator == scanner->tokensMap.end())
			throw CompilerException("Unknown Token !","??",scanner->lineNo);
		printf("%s\n",(char *)(scanner->tokenIterator->second));
		if(currentToken == ALPHA || currentToken == STRING)
			printf("\t*%s\n",scanner->currentTokenValue.strVal);
		else if(currentToken == NUMBER_INT)
			printf("\t%d\n",scanner->currentTokenValue.intVal);
		else if(currentToken == NUMBER_FLOAT)
			printf("\t%06f\n",scanner->currentTokenValue.floatVal);

	} while(currentToken != INVALID && currentToken != _EOF);
	return 1;
}
