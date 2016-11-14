/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "Token.h"
#include "Common.h"

class TrieNode{
private:
	TrieNode **childs;
	TrieNode *parent;
	bool valid;
	int noOfChilds;
	int indexInParent;
public:
	_tokenMap tokenMap;

	TrieNode(TrieNode *parent,char indexInParent,_tokenMap tokenMap);
	TrieNode *getChild(char ch);
	TrieNode *getChild(int ch);
	void setChild(char ch,TrieNode *child);
	void setParent(TrieNode *p);
	int getIndexInParent();
	TrieNode *getParent();
	bool isValid();
	void setValid(bool valid);
	bool hasChild();
};

