/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Scanner.h"

#include <string.h>
#include <ctype.h>
#include <math.h>
#include "CompilerException.h"
#include "Token.h"


Scanner::Scanner(const char *inputFileName){
	this->initialize(inputFileName);
}

void Scanner::initialize(const char *inputFileName){
	strcpy(this->inputFileName,inputFileName);
	this->currentTokenValue.charVal = 0;
	this->currentTokenValue.intVal = 0;
	strcpy(this->currentTokenValue.strVal,"");
	lastTokenValue = currentTokenValue;
	this->currentCharacter = 0;
	this->lineNo = 1;

	this->inputFile = fopen(inputFileName,"rt");
	if(this->inputFile == NULL)
		throw CompilerException("Input file not found !",inputFileName,-1);

	for(int i = 0 ; i < tokenMapSize ; i++)
		tokensMap.insert(tokenPair(tokenMapInit[i].token,tokenMapInit[i]));

//	for(int i = 0 ; i < tokenMapSize ; i++)
//		keywordsMap.insert(keywordPair(tokenMapInit[i].keyword,tokenMapInit[i]));
	for(int i = 0 ; i < tokenMapSize ; i++)
		keywordsTree.insertWord(tokenMapInit[i].keyword,tokenMapInit[i]);

	readChar();
}


inline void Scanner::readChar(){
	this->currentCharacter = fgetc(this->inputFile);
}

_tokens Scanner::scan(){
	this->lastTokenValue = this->currentTokenValue;
	this->lastToken = currentToken;
	currentToken = this->internalScan();
	return this->currentToken;
}
_tokens Scanner::internalScan(){
	int index = 0;
	while(1){
		while(isWhiteSpace(this->currentCharacter) && !feof(this->inputFile)){
			if(this->currentCharacter == '\n')
				this->lineNo++;
			readChar();
		}

		if(feof(this->inputFile)) // end of file reached
			return _EOF;
		if(this->currentCharacter == '\n'){
			this->lineNo++;
			readChar();
		}
		else break;
	}
	switch(this->currentCharacter){
		case '!':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return NOT_EQ;
			} else
				throw CompilerException("= expected",0,this->lineNo);
		case '<':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return SMALLER_EQ;
			} else
				return SMALLER;
		case '>':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return BIGGER_EQ;
			} else
				return BIGGER;
		case '.':
			readChar();
			return DOT;
		case ',':
			readChar();
			return COMMA;
		case ';':
			readChar();
			return SEMI_COLON;
		case ':':
			readChar();
			return COLON;
		case '(':
			readChar();
			return PAREN_OPEN;
		case ')':
			readChar();
			return PAREN_CLOSE;
		case '[':
			readChar();
			return BRACK_OPEN;
		case ']':
			readChar();
			if(this->currentCharacter == '['){
				readChar();
				return BRACK_CLOSE_OPEN;
			}
			return BRACK_CLOSE;
		case '{':
			readChar();
			return AK_OPEN;
		case '}':
			readChar();
			return AK_CLOSE;
		case '=':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return EQ_EQ;
			} else
				return EQ;
		case '+':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return PLUS_EQ;
			} else
				return PLUS;
		case '-':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return MIN_EQ;
			} else
				return MIN;
		case '*':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return ASTERISK_EQ;
			} else
				return ASTERISK;
		case '/':
			readChar();
			if(this->currentCharacter == '='){
				readChar();
				return DIV_EQ;
			} else if(this->currentCharacter == '/'){
				do { readChar(); } while(this->currentCharacter != '\n' && !feof(this->inputFile));
				if(this->currentCharacter != '\n' && feof(this->inputFile))
					return _EOF;
				readChar();
				this->lineNo++;
				return this->internalScan();
			} else if(this->currentCharacter == '*'){
				do {
					do {
						readChar();
						if(this->currentCharacter == '\n')
							this->lineNo++;
					} while(this->currentCharacter != '*' && !feof(this->inputFile));
					if(feof(this->inputFile) && this->currentCharacter != '*')
						return _EOF;
					readChar();
					if(this->currentCharacter == '/'){
						readChar();
						return this->internalScan();
					}
					if(feof(this->inputFile))
						return _EOF;
				} while(1);
			} else
				return DIV;
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			return readNumber();
		case '"':
			return readString();
		case '\'':
			//this->currentTokenValue.tokenType = CHARACTER;
			readChar();
			this->currentTokenValue.charVal = this->currentCharacter;
			readChar();
			if(this->currentCharacter != '\'')
				throw CompilerException("' Expected",NULL,this->lineNo);
			readChar();
			return CHARACTER;
		case 'A': case 'D': case 'G': case 'J': case 'M': case 'P': case 'S': case 'U': case 'W': case 'Y':
		case 'B': case 'E': case 'H': case 'K': case 'N': case 'Q': case 'T': case 'V': case 'X': case 'Z':
		case 'C': case 'F': case 'I': case 'L': case 'O': case 'R':
		case 'a': case 'd': case 'g': case 'j': case 'm': case 'p': case 's': case 'u': case 'w': case 'y':
		case 'b': case 'e': case 'h': case 'k': case 'n': case 'q': case 't': case 'v': case 'x': case 'z':
		case 'c': case 'f': case 'i': case 'l': case 'o': case 'r':
			index = 0;
			while((	isalnum(this->currentCharacter) ||
					this->currentCharacter == '_'		)
				&& !feof(this->inputFile)){
				this->currentTokenValue.strVal[index++] = this->currentCharacter;
				readChar();
				if(index == MAX_STRING_LEN - 1){
					this->currentTokenValue.strVal[index] = 0;
					throw CompilerException("Buffer overflow,string too long !",this->currentTokenValue.strVal,this->lineNo);
				}
			}
			this->currentTokenValue.strVal[index] = 0;
			return this->findKeyword(this->currentTokenValue.strVal);
		default:
			char info[2];
			info[0] = this->currentCharacter;
			info[1] = 0;
			throw CompilerException("Unexpected Token",info,this->lineNo);
	}
	return INVALID;
}

_tokens Scanner::readNumber(){
	char number[1024];
	int numberPointer = 0;
	int dotFound = 0;
	int hex = 0;
	_tokens retVal;
	do {
		number[numberPointer++] = this->currentCharacter;
		readChar();
/*		if(numberPointer == 1 && !isDigit(ch))
			return DOT; */
		if(this->currentCharacter == '.' && dotFound == 1)
			throw CompilerException("Invalid number.",NULL,this->lineNo);

		if(this->currentCharacter == '.')
			dotFound = 1;
		if((this->currentCharacter | 32 ) == 'x' && number[0] == '0' && hex == 0){ // 0xNUMBER = HEX
			hex = 1;
			numberPointer = 0;
			readChar();
			continue;
		}
	} while(isdigit(this->currentCharacter) || (this->currentCharacter== '.' && hex == 0)
		|| isHexDigit(hex,this->currentCharacter)
		);
	if(hex && numberPointer == 0)
		throw CompilerException("Hex constant must have at least one hex digit.",NULL,this->lineNo);
	number[numberPointer] = 0;
	if(numberPointer == 0)
		strcpy(number,"0");
	if(hex){
		int num;
		sscanf(number,"%x",&num);
		retVal = NUMBER_INT;
		this->currentTokenValue.intVal = num;
	} else {
		float num;
		sscanf(number,"%f",&num);
		if(floor(num) == num){
			retVal = NUMBER_INT;
			this->currentTokenValue.intVal = (int)num;
		} else {
			retVal = NUMBER_FLOAT;
			this->currentTokenValue.floatVal = num;
		}

	}
//	readChar(); // se above , it is already done
	return retVal;
}
_tokens Scanner::readString(){
	int index;
	index = 0;
	readChar();
	while(this->currentCharacter != '"' && !feof(this->inputFile)){
		if(this->currentCharacter == '\n')
			this->lineNo++;
		this->currentTokenValue.strVal[index++] = this->currentCharacter;
		readChar();
		if(index == MAX_STRING_LEN - 1){
			this->currentTokenValue.strVal[index] = 0;
			throw CompilerException("Buffer overflow,string too long !",this->currentTokenValue.strVal,this->lineNo);
		}
	}
	if(this->currentCharacter != '"') { // end of file reached
		this->currentTokenValue.strVal[index] = 0;
		throw CompilerException("End of file reached while reading string, \" expected.",this->currentTokenValue.strVal,this->lineNo);
	}
	
	this->currentTokenValue.strVal[index] = 0;
	readChar();
	return STRING;
}

_tokens Scanner::findKeyword(const char *buffer){
	//char types[][20] = {"int","char","float"};
	//keywordIterator = keywordsMap.find(buffer);
	TrieNode *tn = this->keywordsTree.searchWord(buffer);
	if(tn == NULL){
		if(!strcmp(buffer,"int"))
			return TYPE;
		else if(!strcmp(buffer,"char"))
			return TYPE;
		else if(!strcmp(buffer,"float"))
			return TYPE;
		else if(!strcmp(buffer,"void"))
			return TYPE;
		return ALPHA;
	}
	else
		return tn->tokenMap.token;
}
