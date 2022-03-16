#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "btree.h"

node_t* create_node() {
    
	node_t* new_node = (node_t *) malloc(sizeof(node_t));

	if(new_node == NULL){
		printf("Out of memory");
	}

	for(int i = 0;i < NODE_KEYS; i++){
		new_node->key_array[i] = 0;
	}

	for(int i = 0;i < NODE_POINTERS; i++){
		new_node->child_array[i] = NULL;
	}

	new_node->key_index = 0;
	new_node->leaf = TRUE;

	return new_node;
}

btree_t* create_btree() {

    btree_t* new_root = (btree_t *) malloc(sizeof(btree_t));

	if(new_root == NULL){
		return NULL;
	}

	node_t* head = create_node();

	if(head == NULL){
		return NULL;
	}

	new_root->order = NODE_ORDER;
	new_root->root = head;

	return new_root;
}

result_t* get_resultset() {

    result_t *ret = (result_t*) malloc(sizeof(result_t));

	if(ret == NULL){
		printf("ERROR! Out of memory.");
	}

	ret->node_pointer = NULL;
	ret->key = 0;
	ret->found = FALSE;
	ret->depth = 0;

	return ret;
}

void print_node(node_t* n) {
    int i, q;

	printf("  Index: %d\n", n->key_index);

	printf("   Leaf: ");
	if(n->leaf){
		printf("TRUE\n");
	}else{
		printf("FALSE\n");
	}

	printf("  Array:");
	for(i = 0; i < n->key_index; i++){
		printf(" [%d : %d]", i, n->key_array[i]);
	}

	printf("\n  Childs:");
	if(n->leaf){
		printf(" NONE");
	}else{
		for(q = 0; q < NODE_POINTERS; q++){
			printf(" [%d : %x]", q, n->child_array[q]);
		}
	}

	printf("\n\n");
}

result_t* search(int key, node_t *node) {

	int i = 0;

	while((i < node->key_index) && (key > node->key_array[i])){
		i++;
	}

	if((i <= node->key_index) && (key == node->key_array[i])){
		result_t *result = get_resultset();
		result->node_pointer = node;
		result->key = i;
		result->found = TRUE;
		return result;
	}

	if(node->leaf == TRUE){
		result_t *result = get_resultset();
		result->node_pointer = node;
		result->found = FALSE;
		return result;
	} else {
		result_t *result = get_resultset();
		return search(key, node->child_array[i]);
	}
}

void split_child(node_t* parent_node, int i, node_t* child_array) { //It means i-th child in child_array of papent_node//
	
	node_t* new_node = create_node();
	new_node->leaf = child_array->leaf;
	new_node->key_index = NODE_ORDER-1;

	for(int j = 0;j < NODE_ORDER-1;j++){
		new_node->key_array[j] = child_array->key_array[NODE_ORDER+j];
	}

	if(child_array->leaf == 0){
		for(int j = 0;j < NODE_ORDER;j++){
			new_node->child_array[j] = child_array->child_array[NODE_ORDER+j];
		}
	}
	child_array->key_index = NODE_ORDER-1;

	for(int j = parent_node->key_index;j >= i;j--){
		parent_node->child_array[j+1] = parent_node->child_array[j];
	}

	parent_node->child_array[i] = new_node;

	for(int j = parent_node->key_index;j>=i;j--){
		parent_node->key_array[j] = parent_node->key_array[j-1];
	}

	parent_node->key_array[i-1] = child_array->key_array[NODE_ORDER-1];

	parent_node->key_index++;
}

void insert_nonfull(int key, node_t* n) {
    int i = n->key_index;
	
	if(n->leaf){
		while((i >= 1) && (key < n->key_array[i-1])){
			n->key_array[i] = n->key_array[i-1];
			i--;
		}
		n->key_array[i] = key;
		n->key_index++;
	} else {
		while((i >= 1) && (key < n->key_array[i-1])){
			i--;
		}
		if(n->child_array[i]->key_index == NODE_KEYS){
			split_child(n, i+1, n->child_array[i]);
			if(key > n->key_array[i]){
				i++;
			}
		}
		insert_nonfull(key, n->child_array[i]);
}

node_t* insert(int key, btree_t* b) {
	node_t* root = b->root;

	if(root->key_index == NODE_KEYS){
		node_t* newNode = create_node();
		b->root = newNode;
		newNode->leaf = 0;
		newNode->key_index = 0;
		newNode->child_array[0] = root;
		split_child(newNode, 1, root);
		insert_nonfull(newNode, key);
	} else {
		insert_nonfull(b->root, key);
	}

	return b->root;
}

void merge_children(node_t* root, int index, node_t* child1, node_t* child2) {
	child1->key_index = NODE_KEYS;
	int i;

	for(i = NODE_ORDER;i < NODE_KEYS;i++)
		child1->key_array[i] = child2->key_array[i-NODE_ORDER];
	child1->key_array[NODE_ORDER-1] = root->key_array[index]; 
	
	if(child2->leaf == false){
		for(i=NODE_ORDER;i<NODE_POINTERS;i++)
			child1->child_array[i] = child2->child_array[i-NODE_ORDER];
	}
	
	for(i=index+1;i<root->key_index;i++){
		root->key_array[i-1] = root->key_array[i];
		root->child_array[i] = root->child_array[i+1];
	}
	root->key_index--;
	free(child2);
}

void BTreeBorrowFromLeft(node_t* root, int index, node_t* leftPtr, node_t* curPtr) {
	curPtr->key_index++;
	int i;

	for(i=curPtr->key_index-1;i>0;i--) curPtr->key_array[i] = curPtr->key_array[i-1];
	curPtr->key_array[0] = root->key_array[index];

	root->key_array[index] = leftPtr->key_array[leftPtr->key_index-1];

	if(leftPtr->leaf == false)
		for(i=curPtr->key_index;i>0;i--)
			curPtr->child_array[i] = curPtr->child_array[i-1];
	curPtr->child_array[0] = leftPtr->child_array[leftPtr->key_index];

	leftPtr->key_index--;
}

void BTreeBorrowFromRight(node_t* root, int index, node_t* rightPtr, node_t* curPtr) {
	curPtr->key_index++;
	curPtr->key_array[curPtr->key_index-1] = root->key_array[index];
	root->key_array[index] = rightPtr->key_array[0];

	int i;

	for(i=0;i<rightPtr->key_index-1;i++)
		rightPtr->key_array[i] = rightPtr->key_array[i+1];

	if(0 == rightPtr->leaf){
		curPtr->child_array[curPtr->key_index] = rightPtr->child_array[0];
		for(i=0;i<rightPtr->key_index;i++)
			rightPtr->child_array[i] = rightPtr->child_array[i+1];
	}

	rightPtr->key_index--;
}

void BTreeDeleteNoNone(int X, node_t* root) {

	int i;
	
	if(root->leaf == true){
		i=0;
		node_t* prePtr = NULL;
		node_t* nexPtr = NULL;

		while( (i < root->key_index) && (X > root->key_array[i])) {
			i++;
		}
		
		if(X == root->key_array[i]){
			for(;i<root->key_index-1;i++)
				root->key_array[i] = root->key_array[i+1];
			root->key_index--;
		} else {
			printf("Key is not found.\n");
			return;
		}
	} else {  
		i = 0;
		
		while ( (i < root->key_index) && (X > root->key_array[i]) ) i++;
		
		if ( (i < root->key_index) && (X == root->key_array[i]) ) {
			prePtr = root->child_array[i];
			nexPtr = root->child_array[i+1];
			
			if(prePtr->key_index > NODE_ORDER-1){
				int aPrecursor = BTreeGetLeftMax(prePtr);
				root->key_array[i] = aPrecursor;
			
				BTreeDeleteNoNone(aPrecursor,prePtr);
			} else if(nexPtr->key_index > NODE_ORDER-1){
				int aSuccessor = BTreeGetRightMin(nexPtr);
				root->key_array[i] = aSuccessor;
				BTreeDeleteNoNone(aSuccessor,nexPtr);
			} else {
				merge_children(root,i,prePtr,nexPtr);
				BTreeDeleteNoNone(X,prePtr);
			}
		} else {
			prePtr = root->child_array[i];
			node_t *preprePtr = NULL;
			if(i < root->key_index)
				nexPtr = root->child_array[i+1];
			if(i > 0)
				preprePtr = root->child_array[i-1];
			if(NODE_ORDER-1 == prePtr->key_index) {
				if( (preprePtr != NULL) && (preprePtr->key_index > NODE_ORDER-1))
					BTreeBorrowFromLeft(root,i-1,preprePtr,prePtr);
				else if( (nexPtr != NULL) && (nexPtr->key_index > NODE_ORDER-1))
					BTreeBorrowFromRight(root,i,nexPtr,prePtr);
				else if(preprePtr != NULL){
					merge_children(root,i-1,leftBro,prePtr);
					prePtr = preprePtr;
				} else
					merge_children(root,i,prePtr,nexPtr);
			}
			BTreeDeleteNoNone(X,prePtr);
		}
	}
}

int BTreeGetLeftMax(node_t* T){
	if(T->leaf == false){
		return BTreeGetLeftMax(T->child_array[T->key_index]);
	}else{
		return T->key_array[T->key_index-1];
	}
}

int BTreeGetRightMin(node_t* T){
	if(T->leaf == false){
		return BTreeGetRightMin(T->child_array[0]);
	}else{
		return T->key_array[0];
	}
}

node_t* delete(int key, btree_t* b) {
	if(b->root->key_index == 1){
		node_t* child1 = b->root->child_array[0];
		node_t* child2 = b->root->child_array[1];
		if((child1 != NULL) && (child2 != NULL)){
			if((NODE_ORDER-1 == child1->key_index) && (NODE_ORDER-1 == child2->key_index)){
				merge_children(b->root, 0, child1, child2);
				free(b->root);
				BTreeDeleteNoNone(key, child1);
				return child1;
			}
		}
	}
	BTreeDeleteNoNone(key, b->root);
	return b->root;
}
