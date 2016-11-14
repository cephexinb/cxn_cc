/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include <stdio.h>

#include "Common.h"
#include "Scanner.h"
#include "Token.h"


class Compiler{
public:
	Compiler(const char *inputFileName);
	char compile();

private:
	Scanner *scanner;
	
	void initialize(const char *inputFileName);
};
