/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "CompilerException.h"
#include <string.h>

CompilerException::CompilerException(const char *reason,const char *info,DWORD lineNo){
	this->lineNo = lineNo;

	if(reason != NULL)
		strcpy(this->reason,reason);
	else
		strcpy(this->info,"");

	if(info != NULL)
		strcpy(this->info,info);
	else
		strcpy(this->info,"");
}

const char *CompilerException::getInfo(){		return this->info;		}
const char *CompilerException::getReason(){		return this->reason;	}
DWORD CompilerException::getLineNo(){		return this->lineNo;	}

