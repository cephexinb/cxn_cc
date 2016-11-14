/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "CompilerException.h"	


Parser::Parser(const char *PTFile,const char *inputFileName){
	this->initialize(PTFile,inputFileName);
}

void Parser::initialize(const char *PTFile,const char *inputFileName){

	for(int i = 0 ; i < 512 ; i++)
		this->keywordsIDs[i].isValid = 0;

	this->scanner = new Scanner(inputFileName);

	FILE *pt = fopen(PTFile,"rt");
	if(pt == NULL)
		fatalError("Parser::initialize\tCan't open PTFile \"%s\"\n",PTFile);

	int rows,cols;
	rows = cols = 0;
	fscanf(pt,"%d %d",&rows,&cols);
	char tmpBuffer[100];
	for(int i = 0 ; i < cols ; i++){
		fscanf(pt,"%s",tmpBuffer);
		this->setKeywordID(tmpBuffer,i);
	}
	this->PT = new PTBlock *[rows];
	for(int i = 0 ; i < rows ; i++){
		this->PT[i] = new PTBlock[cols];
		for(int j = 0 ; j < cols ; j++)
			fscanf(pt,"%d %d %s",&(this->PT[i][j].action),
				&(this->PT[i][j].index),this->PT[i][j].sem);
//			this->PT[i][j].index = 0;
	}
	fclose(pt);

	this->CG = new CodeGenerator(this->scanner);
}

int Parser::findKeywordID(const char *id){
	for(int i = 0 ; i < 512 && this->keywordsIDs[i].isValid ; i++){
		if(!strcmp(id,this->keywordsIDs[i].symbol))
			return i;
	}

	fatalError("Parser::findKeyword\tCan't find keyword id,%s\n",id);
	return 0;
}

void Parser::setKeywordID(const char *idStr,int id){
	int i = 0 ;
	while(this->keywordsIDs[i].isValid) i++;
	strcpy(this->keywordsIDs[i].symbol,idStr);
	this->keywordsIDs[i].isValid = 1;
}

char Parser::parse(){
	int currentNode,tokenID;

	currentNode = 0;
	_tokens currentToken = this->scanner->scan();
	this->scanner->tokenIterator = this->scanner->tokensMap.find(currentToken);
	if(this->scanner->tokenIterator == this->scanner->tokensMap.end())
		throw CompilerException("Unknown Token !","??",this->scanner->lineNo);

	_tokenMap tokenMap = this->scanner->tokenIterator->second;
	while(true){
		//printf("CurrentNode: %d, token:%s\n",currentNode+1,tokenMap.symbol);
		tokenID = findKeywordID(tokenMap.symbol);
		PTBlock *ptb = &(this->PT[currentNode][tokenID]);
		switch(ptb->action){
			case ERR:
				printf("Parse error lineno %d\n",scanner->lineNo);
				return 3;
			case SHIFT:
				//printf("SHIFT\n");
				currentToken = this->scanner->scan();
				this->scanner->tokenIterator = this->scanner->tokensMap.find(currentToken);
				if(this->scanner->tokenIterator == this->scanner->tokensMap.end())
					throw CompilerException("Unknown Token !","??",this->scanner->lineNo);

				tokenMap = this->scanner->tokenIterator->second;
				tokenID = findKeywordID(tokenMap.symbol);
				currentNode = ptb->index;
				//printf("codegen: %s\n",ptb->sem);
				this->CG->generateCode(ptb->sem);
				break;
			case PUSHGOTO:
				//printf("push goto\n");
				m_ParseStack.push(currentNode);
				currentNode = ptb->index;
				break;
			case RET:
				currentNode = m_ParseStack.top();
				m_ParseStack.pop();
				ptb = &(this->PT[currentNode][ptb->index]);
				//printf("%s\n",ptb->sem);
				this->CG->generateCode(ptb->sem);
				//printf("ret and codegen: %s\n",ptb->sem);
				currentNode = ptb->index;
				break;
			case ACC:
				printf("Accept !\n");
				printf("Parse Stack Count : %d\n\n",this->CG->parseStack.getCount());
				for(int i = 0 ; i < this->CG->PC; i++){
					switch(this->CG->CODE[i].noOfArgs){
						case 0:
							printf("%-8d\t%s\n",i,this->CG->CODE[i].symbol); break;
						case 1:
							printf("%-8d\t%s %s\n",i,this->CG->CODE[i].symbol,this->CG->CODE[i].arg0Name); break;
						case 2:
							printf("%-8d\t%s %s,%s\n",i,this->CG->CODE[i].symbol,this->CG->CODE[i].arg0Name,this->CG->CODE[i].arg1Name); break;
						case 3:
//							printf("%-8d\t%s %s,%s,%s\n",i,this->CG->CODE[i].symbol,this->CG->CODE[i].arg0Name,
//								this->CG->CODE[i].arg1Name,this->CG->CODE[i].arg2Name); break;
							printf("%-8d\t%s %s,%s\n",i,this->CG->CODE[i].symbol,
								this->CG->CODE[i].arg1Name,this->CG->CODE[i].arg2Name); break;
					}
				}
				printf("%-8d\n",this->CG->PC);
				this->CG->generateAsm("tests\\comp.asm");
				return 0;
			default:
				printf("??\n");
				break;
		}
	}
	printf("%s\n",tokenMap.symbol);
	return 0;
}
