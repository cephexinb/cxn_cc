/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include <stdio.h>

#include "Common.h"
#include "Compiler.h"
#include "CompilerException.h"
#include "Parser.h"

int main(){
	try{
//		Compiler c("test.txt");
//		c.compile();
		Parser p("pt.npt","tests\\test.cpp");
		p.parse();
	} catch (CompilerException exp){
		int thereIsInfo = 0;
		printf("*** Compiler Exception\n");
		if(exp.getReason() != NULL){
			printf("\tReason: %s\n",exp.getReason());
			thereIsInfo = 1;
		}
		if(exp.getInfo() != NULL){
			printf("\tInfo: %s\n",exp.getInfo());
			thereIsInfo = 1;
		}
		printf("\tLineNo: %d\n",exp.getLineNo());
		if(!thereIsInfo)
			printf("\tNo more info available !\n");
	}
	int y=0;
	return 0;
}
