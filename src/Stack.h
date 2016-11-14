/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"
#include "SymbolTable.h"

enum StackElementType { SET_SYMBOL, SET_LABEL, SET_TYPE, SET_MISC };

struct _label {
	DWORD address;
//	DWORD address1; // :D
	char name[40];
};

struct _stackElement {
	StackElementType type;
	union {
		_symbol *symbol;
		_label label;
		TYPES type;
		DWORD miscData;
	} data;
};

class Stack {
public:
	Stack();
	DWORD getCount();
	_symbol *topSymbol(int doPop);
	_label *topLabel(int doPop);
	TYPES topType(int doPop);
	DWORD topMisc(int doPop);
	StackElementType getTopType();
	void pop();
	void push(_symbol *symbol);
	void push(TYPES type);
	void push(DWORD address,char *name);
	void push(DWORD misc);

private:
	_stackElement data[2048];
	DWORD dataPointer;

	void initialize();
};
