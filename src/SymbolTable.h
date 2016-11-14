/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"
#include "Scanner.h"

struct _symbol {
	DWORD address;
	DWORD defLineNo;
	int isValid;
	int isTemp;
	int isConstant;

	int level;
	char name[64];
	TYPES type; //ret value for functions
	initialValueType initialValue;

	int isFunction;
	int functionBodyFound;
	TYPES functionParams[10];
	char functionParamsName[10][30];
	int noOfFunctionParams;
	int noOfVars;
	int functionEntranceSUBESPPC;

	int isRegister;

	int isPointer;
	int arraySize;
	int noOfDims;
	int dims[5];
};

class SymbolTable {
public:
	DWORD nextAddress;
	_symbol functionResultSymbol;
	SymbolTable *lastSymbolTable;
	int noOfVars;
	_symbol symbols[512];

	SymbolTable(SymbolTable *st);
	SymbolTable(Scanner *scanner);
	_symbol *declareVar(const char *name,TYPES type,initialValueType initialValue);
	_symbol *declareVar(const char *name,TYPES type,initialValueType initialValue,int declareInGlobal);
	void declareArray(_symbol *symbol,int arraySize);
	void declareFunction(_symbol *symbol);
	int isVarDeclared(const char *name);
	int isVarDeclaredInThisBlock(const char *name);
	//_symbol getVar(const char *name);
	_symbol *getVarDesc(const char *name);
	_symbol *getTemp(TYPES type);
	_symbol *getTemp(TYPES type,int doNotUseReg);
	void releaseTemp(_symbol *name);
	void enterBlock();
	void exitBlock();
	char *getVarAddress(_symbol *symbol,int effectiveAddress,int effectiveAddressOnReg);
	char *getVarAddress(int constant);
	char *getVarAddress(char *symName);
	int getNoOfTemps();
	int getMaxNoOfVars();
	int getRegStatus(const char *regName);

private:
	_symbol temps[512];
	int regsStatus[5];
	int symbolsPointer;
	int tempsPointer;
	int noUsedRegs;
	int noOfLocalStackTemps;
	int maxNoOfLocalStackTemps;
	SymbolTable *parentSymbolTable; 
	static Scanner *scanner;
	DWORD noTemps;
	int maxNoOfVars;
	int level;

	void initialize(SymbolTable *st,Scanner *scanner);
	_symbol *getVarDescInternal(const char *name);
	const char *getFreeReg();
	void freeReg(const char *regName);
	void updateMaxNoOfVars(int noOfVars);
};
