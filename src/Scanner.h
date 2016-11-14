/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Common.h"
#include "Token.h"
#include <stdio.h>
#include <hash_map>

#include "TrieTree.h"

using namespace std;
using namespace stdext;

class Scanner {
public:
	token currentTokenValue;
	token lastTokenValue;
	_tokens currentToken;
	_tokens lastToken;
	DWORD lineNo;

	typedef pair <_tokens,_tokenMap> tokenPair;
	hash_map <_tokens,_tokenMap> tokensMap;
	hash_map <_tokens,_tokenMap> :: iterator tokenIterator;

	typedef pair <const char *,_tokenMap> keywordPair;
	hash_map <const char *,_tokenMap> keywordsMap;
	hash_map <const char *,_tokenMap> :: iterator keywordIterator;
	TrieTree keywordsTree;

	Scanner(const char *inputFileName);
	FILE *getInputFile();
	_tokens scan();

private:
	FILE *inputFile;
	char currentCharacter;
	char inputFileName[1024];


	void initialize(const char *inputFileName);
	inline void readChar();
	_tokens readNumber();
	_tokens readString();
	_tokens internalScan();

	_tokens findKeyword(const char *buffer);
};
