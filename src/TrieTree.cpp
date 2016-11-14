/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "TrieNode.h"
#include "TrieTree.h"
#include <string.h>
#include <stdio.h>

TrieTree::TrieTree(){
//	root = new TrieNode(NULL,0);
	root = NULL;
}
TrieNode *TrieTree::searchWord(const char *word){
	if(!checkWord((char *)word)){
		printf("Skipping find %s\n",word);
		return NULL;
	}
	int len = (int)strlen(word);
	TrieNode *c = root;
	for(int i = 0 ; (i < len) && (c != NULL); i++)
		c = c->getChild(word[i]);
	if(c == NULL)
		return c;
	if(c->isValid())
		return c;
	else
		return NULL;
}
bool TrieTree::insertWord(const char *word,_tokenMap tokenMap){
	if(!checkWord((char *)word)){
		printf("Skipping %s\n",word);
		return false;
	}
//	if(searchWord(word) != NULL)
//		return ;
	int len = (int)strlen(word);
	if(root == NULL)
		root = new TrieNode(NULL,0,tokenMap);
	TrieNode *c = root;
	for(int i = 0 ; i < len ; i++){
		if(c->getChild(word[i]) == NULL)
			c->setChild(word[i],new TrieNode(c,word[i],tokenMap));
		c = c->getChild(word[i]);
	}
	if(c->isValid())
		return false;
	c->setValid(true);
	return true;
}
bool TrieTree::deleteWord(char *word){
	int len = (int)strlen(word);
	TrieNode *c = root,*parent;
	if(c == NULL)
		return false;
	for(int i = 0 ; i < len ; i++){
		if(c->getChild(word[i]) == NULL)
			return false;
		c = c->getChild(word[i]);
	}
	if(!c->isValid())
		return false;
	c->setValid(false);
	while( (c != NULL) && (!c->isValid()) && (!c->hasChild())){
		parent = c->getParent();
		if(parent != NULL)
			parent->setChild(c->getIndexInParent() + 'a',NULL);
		if(c == root)
			root = NULL;
		delete c;
		c = parent;
	}
	return true;
}
void TrieTree::dumpTree(FILE *out){
	dumpTree(root,out,"");
}
void TrieTree::dumpTree(TrieNode *r,FILE *out,char *prefix){
	if(r == NULL)
		return ;
	if(r->isValid())
		fprintf(out,"%s\n",prefix);
	int len = (int)strlen(prefix)+1;
	for(int i = 0 ; i < 36 ; i++){
		if(r->getChild(i) == NULL)
			continue;
		char* newPrefix = new char[len+1];
		strcpy(newPrefix,prefix);
		newPrefix[len - 1] = i + 'a';
		newPrefix[len] = 0;

		dumpTree(r->getChild(i),out,newPrefix);

		delete [] newPrefix;
	}
}
bool TrieTree::checkWord(char *word){
/*	int len = strlen(word);
	int t ;
	for(int i = 0 ; i < len ; i++){
		t = word[i] | 32;
		if( t < 'a' || t > 'z')
			return false;
	}*/
	return true;
}
