/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"
#include "SymbolTable.h"
#include <iostream>
#include <hash_map>
#include "Stack.h"

enum _operand {
	OP_MOV,		OP_SUB,		OP_ADD,		OP_LEA,
	OP_MUL,		OP_DIV,		OP_CDQ,
	OP_JMP,		OP_JZ,		OP_JNZ,		OP_JA,		OP_JAE,		OP_JL,		OP_JLE,		OP_JPE,		OP_JPO,
	OP_CMP,
	OP_CALL,	OP_PUSH,	OP_LABEL,	OP_RET,		OP_POP,		OP_PUSHA,	OP_POPA,
	OP_FLD,		OP_FILD,	OP_FSTP,	OP_FISUB,	OP_FSUB,
	OP_FMUL,	OP_FIMUL,	OP_FADD,	OP_FIADD,	OP_FDIV,	OP_FIDIV,
	OP_FUCOMPP,	OP_FCOMP,	OP_FSTSW,
	OP_SWITCH,	OP_CASE,	OP_DEFAULT,	OP_SWEND,
	OP_TEST,
	OP_NEG
};

struct _operation {
	_operand operand;
	char symbol[64];
	DWORD operandSize;
	DWORD noOfArgs;
	DWORD arg0;
	char *arg0Name;
	DWORD arg1;
	char *arg1Name;
	DWORD arg2;
	char *arg2Name;

	int hasLabel;
	char labelName[64];
	int isFunctionStart,isFunctionEnd;
	char functionName[64];
};

extern _operation operations [];
extern const int operationsSize;


using namespace std;

class CodeGenerator {
public:
	DWORD nextAddress;
	Stack parseStack;
	Stack arraysStack;
	Stack addressStack;
	DWORD PC;
	_operation CODE[2048];

	typedef pair <_operand,_operation> operationPair;
	hash_map <_operand,_operation> operationsMap;
	hash_map <_operand,_operation> :: iterator operationIterator;

	CodeGenerator(Scanner *scanner);
	void generateCode(string sem);
	void generateAsm(const char *path);

private:
	SymbolTable *globalSymbolTable;
	int noOfConstFloatVars;
	int isDeclaration;
	DWORD ifLbl;
	DWORD whileLbl;
	DWORD doLbl;
	DWORD forLbl;
	TYPES currentDeclarationType;
	Scanner *scanner;
	_symbol *currentFunction;
	_symbol *currentArray;
	_symbol *currentArrayTempIndex;
	_label lastIfLabel;
	int switchDefaultHasBeenSeen;

	void initialize(Scanner *scanner);
	void checkType(_symbol *s1,_symbol *s2);
	void checkType(_symbol *s1,_symbol *s2,int forceType);
	_operation findOperation(_operand operand);
};
