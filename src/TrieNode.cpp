/********************************************/
/********  author : cephexin - 2004 **********/
/*********************************************/
#include "TrieNode.h"
#include <stdio.h>
#include <ctype.h>

TrieNode::TrieNode(TrieNode *parent,char indexInParent,_tokenMap tokenMap){
	childs = new TrieNode *[36];
	for(int i = 0 ; i < 36 ; i++)
		childs [i] = NULL;
	noOfChilds = 0;
	this->parent = parent;
	this->indexInParent = isdigit(indexInParent) ? (indexInParent - '0' + 26) : (indexInParent | 32) - 'a';
	this->tokenMap = tokenMap;
	valid = false;
}
void TrieNode::setChild(char ch,TrieNode *child){
	ch = ch | 32;
	int index = ch - 'a';
	if(child == NULL && childs[index] == NULL)
		return;
	if(child == NULL)
		noOfChilds--;
	else // childs[index] == NULL
		noOfChilds++;
	childs[index] = child;
}
void TrieNode::setParent(TrieNode *p){parent = p;};
TrieNode *TrieNode::getChild(char ch){
	ch = ch | 32;
	return childs[isdigit(ch) ? (ch - '0' + 26) : (ch - 'a')];
}
TrieNode *TrieNode::getChild(int ch){
	return childs[ch];
}
int TrieNode::getIndexInParent(){
	return indexInParent;
}
TrieNode *TrieNode::getParent(){return parent;}
void TrieNode::setValid(bool valid){this->valid = valid;}
bool TrieNode::isValid(){return valid;}
bool TrieNode::hasChild(){return (noOfChilds != 0);}

