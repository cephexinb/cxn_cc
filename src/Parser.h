/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"
#include "Scanner.h"
#include <stack>
#include "CodeGenerator.h"


enum ACTIONTYPE {ERR, SHIFT, GOTO, PUSHGOTO, RET, ACC};

struct PTBlock {
	int index;
	ACTIONTYPE  action;
	char sem[30];
};

struct _keywordID{
	char symbol[30];
	char isValid;
};


class Parser {
public:
	Parser(const char *PTFile,const char *inputFileName);
	char parse();

private:
	PTBlock **PT;
	Scanner *scanner;
	_keywordID keywordsIDs[512];
	CodeGenerator *CG;

	typedef stack<int> INTSTACK;
	INTSTACK m_ParseStack;

	void initialize(const char *PTFile,const char *inputFileName);
	int findKeywordID(const char *id);
	void setKeywordID(const char *idStr,int id);
};
