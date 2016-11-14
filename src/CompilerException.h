/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"

class CompilerException{
public:
	CompilerException(const char *reason,const char *info,DWORD lineNo);
	const char *getReason();
	const char *getInfo();
	DWORD getLineNo();

private:
	char reason[256];
	char info[512];
	DWORD lineNo;
};
