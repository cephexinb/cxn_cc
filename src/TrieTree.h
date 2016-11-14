/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "TrieNode.h"
#include <stdio.h>

class TrieTree{
private:
	TrieNode *root;
	void dumpTree(TrieNode *r,FILE *out,char *prefix);
	bool checkWord(char *word);
public:
	TrieTree();
	bool insertWord(const char *word,_tokenMap tokenMap);
	bool deleteWord(char *word);
	TrieNode *searchWord(const char *word);
	void dumpTree(FILE *out);
};
