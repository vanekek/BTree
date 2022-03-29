#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "btree.h"
//Function allocates node, sets default values for properties//
node_t* create_node(BTREE_ERR *err) {
    
	node_t* new_node = (node_t *) malloc(sizeof(node_t));

	if (new_node == NULL) {
		fprintf(stderr, "Out of memory\n");
		if (err != NULL)
			*err = EMALLOC;
		return;
	}

	for (int i = 0;i < NODE_KEYS; i++) {
		new_node->key_array[i] = 0;
	}

	for (int i = 0;i < NODE_POINTERS; i++) {
		new_node->child_array[i] = NULL;
	}

	new_node->key_index = 0;
	new_node->leaf = TRUE;

	*err = ESUCCESS;
	return new_node;
}
//Allocating a btree//
btree_t* create_btree(BTREE_ERR *err) {

    btree_t* new_root = (btree_t *) malloc(sizeof(btree_t));

	if (new_root == NULL) {
		fprintf(stderr, "Out of memory\n");
		if (err != NULL)
			*err = EMALLOC;
		return;
	}

	node_t* head = create_node(err);

	new_root->root = head;

	*err = ESUCCESS;
	return new_root;
}
//Allocating a result-set//
result_t* get_resultset(BTREE_ERR *err) {

    result_t *ret = (result_t*) malloc(sizeof(result_t));

	if (ret == NULL) {
		fprintf(stderr, "Out of memory\n");
		if (err != NULL)
			*err = EMALLOC;
		return;
	}

	ret->node_pointer = NULL;
	ret->key = 0;
	ret->found = FALSE;
	ret->depth = 0;

	*err = ESUCCESS;
	return ret;
}
//printing result-set//
void print_resultset(result_t* res, BTREE_ERR *err) {

	if (res == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}

	if (res->found == FALSE) {
		printf("Key is not found in btree.\n\n");
	} else {
		printf("Key %d has succesfully found in btree !\n", res->key);
		printf("Depth: %d\n", res->depth);
		printf("Key is in node: %lx", (unsigned long int)res->node_pointer);
		printf("\n\n");
	}
	*err = ESUCCESS;
}
//printing of node function//
void print_node(node_t* n, BTREE_ERR *err) {
    int i, q;

	if (n == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}	

	printf("  Index: %d\n", n->key_index);

	printf("   Leaf: ");
	if (n->leaf) {
		printf("TRUE\n");
	} else {
		printf("FALSE\n");
	}

	printf("  Array:");
	for (i = 0; i < n->key_index; i++) {
		printf(" [%d : %d]", i, n->key_array[i]);
	}

	printf("\n  Childs:");
	if (n->leaf) {
		printf(" NONE");
	} else {
		for (q = 0; q < NODE_POINTERS; q++) {
			printf(" [%d : %lx]", q, (unsigned long int)n->child_array[q]);
		}
	}
	*err = ESUCCESS;
	printf("\n\n");
}
/* Search operation searches key X in btree T. Recursively traverse the tree
** from top to bottom. At each level Search choose the maximum key whose
** value is greater than or equal to the X. If equal to X, found.
** Otherwise continue to traverse. */
void search(int key, node_t *node, BTREE_ERR *err) {

	if (node == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}

	int i = 0;

	while ((i < node->key_index) && (key > node->key_array[i])) {
		i++;
	}

	if ((i <= node->key_index) && (key == node->key_array[i])) {
		result_t *result = get_resultset(err);
		result->node_pointer = node;
		result->key = key;
		result->found = TRUE;
		print_resultset(result, err);
		free(result);
	} else if (node->leaf == TRUE) {
		result_t *result = get_resultset(err);
		result->node_pointer = node;
		result->found = FALSE;
		print_resultset(result, err);
		free(result);
	} else {
		search(key, node->child_array[i], err);
	}
	*err = ESUCCESS;
}
/* split_child function moves the median key of node child_array into
** its parent ptrParent, where child_array is the ith child of ptrParent.*/
void split_child(node_t* parent_node, int i, node_t* child_array, BTREE_ERR *err) { //It means i-th child in child_array of papent_node//

	if ((i > 5) || (i < 0)) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return NULL;
	}

	node_t* new_node = create_node(err);
	new_node->leaf = child_array->leaf;
	new_node->key_index = NODE_ORDER-1;

	for (int j = 0;j < NODE_ORDER-1;j++) {
		new_node->key_array[j] = child_array->key_array[NODE_ORDER+j];
	}

	if (child_array->leaf == 0) {
		for (int j = 0;j < NODE_ORDER;j++) {
			new_node->child_array[j] = child_array->child_array[NODE_ORDER+j];
		}
	}
	child_array->key_index = NODE_ORDER-1;

	for (int j = parent_node->key_index;j >= i;j--) {
		parent_node->child_array[j+1] = parent_node->child_array[j];
	}

	parent_node->child_array[i] = new_node;

	for (int j = parent_node->key_index; j >= i; j--) {
		parent_node->key_array[j] = parent_node->key_array[j-1];
	}

	parent_node->key_array[i-1] = child_array->key_array[NODE_ORDER-1];

	parent_node->key_index++;
	*err = ESUCCESS;
}
/* insert_nonfull function inserts X into a non-full node T. It means that
** it is guaranteed that T is not a full node.*/
void insert_nonfull(int key, node_t* n, BTREE_ERR *err) {
	   	
		if (n == NULL) {
			fprintf(stderr, "Invalid argument\n");
			if (err != NULL)
				*err = EINVARG;
			return;
		}
		   
		int i = n->key_index;

		if (n->leaf == TRUE) {
			while ((i >= 1) && (key < n->key_array[i-1])) {
				n->key_array[i] = n->key_array[i-1];
				i--;
			}
			n->key_array[i] = key;
			n->key_index++;
		} else {
			while ((i >= 1) && (key < n->key_array[i-1])) {
				i--;
			}
			if (n->child_array[i]->key_index == NODE_KEYS) {
				split_child(n, i+1, n->child_array[i], err);
				if (key > n->key_array[i]) {
					i++;
				}
			}
			insert_nonfull(key, n->child_array[i], err);
	}
	*err = ESUCCESS;
}
/* The BTreeInsert operation insert key into T.Before insert ,this operation
** check whether T's root node is full(root->key_index == 2*d -1) or not.If full,
** execute split_child to guarantee the parent never become full.And then
** execute BTreeInsertNonFull to insert X into a non-full node.*/
node_t* insert(int key, btree_t* b, BTREE_ERR *err) {
	
	if (b == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	node_t* root = b->root;
	if (root->key_index == NODE_KEYS) {
		node_t* newNode = create_node(err);
		b->root = newNode;
		newNode->leaf = FALSE;
		newNode->key_index = 0;
		newNode->child_array[0] = root;
		split_child(newNode, 1, root, err);
		insert_nonfull(key, newNode, err);
	} else {
		insert_nonfull(key, b->root, err);
	}
	*err = ESUCCESS;
	return b->root;
}
/* merge_children function merges the root->K[index] and it's two children
** and then set child1 to the new root.*/
void merge_children(node_t* root, int index, node_t* child1, node_t* child2, BTREE_ERR *err) {
	
	if ((root == NULL) || (index < 0) || (child1 == NULL) || (child2 == NULL)) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return NULL;
	}
	
	child1->key_index = NODE_KEYS;
	int i;

	for (i = NODE_ORDER;i < NODE_KEYS;i++)
		child1->key_array[i] = child2->key_array[i-NODE_ORDER];
	child1->key_array[NODE_ORDER-1] = root->key_array[index]; 
	
	if (child2->leaf == FALSE) {
		for (i = NODE_ORDER;i < NODE_POINTERS;i++)
			child1->child_array[i] = child2->child_array[i-NODE_ORDER];
	}
	
	for (i = index+1;i < root->key_index;i++) {
		root->key_array[i-1] = root->key_array[i];
		root->child_array[i] = root->child_array[i+1];
	}
	root->key_index--;
	free(child2);
	*err = ESUCCESS;
}
/* BTreeBorrowFromLeft function borrows a key from leftPtr, curPtr borrows
** a node from leftPtr, root->K[index] shifts down to curPtr, leftPtr's
** is shifting right-max key up to root->K[index].*/
void BTreeBorrowFromLeft(node_t* root, int index, node_t* leftPtr, node_t* curPtr, BTREE_ERR *err) {
	
	if ((root == NULL) || (leftPtr == NULL) || (index < 0) || (curPtr == NULL)) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	curPtr->key_index++;
	int i;

	for (i=curPtr->key_index-1;i>0;i--) {
		curPtr->key_array[i] = curPtr->key_array[i-1];
	}
	curPtr->key_array[0] = root->key_array[index];

	root->key_array[index] = leftPtr->key_array[leftPtr->key_index-1];

	if (leftPtr->leaf == FALSE)
		for (i=curPtr->key_index;i>0;i--)
			curPtr->child_array[i] = curPtr->child_array[i-1];
	curPtr->child_array[0] = leftPtr->child_array[leftPtr->key_index];

	leftPtr->key_index--;
	*err = ESUCCESS;
}
/* BTreeBorrowFromLeft function borrows a key from rightPtr, curPtr borrows
** a node from rightPtr, root->K[index] shifts down to curPtr, shifts RightPtr's
** left-min key up to root->K[index].*/
void BTreeBorrowFromRight(node_t* root, int index, node_t* rightPtr, node_t* curPtr, BTREE_ERR *err) {
	
	if ((root == NULL) || (index < 0) || (rightPtr == NULL) || (curPtr == NULL)) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	curPtr->key_index++;
	curPtr->key_array[curPtr->key_index-1] = root->key_array[index];
	root->key_array[index] = rightPtr->key_array[0];

	int i;

	for (i=0;i<rightPtr->key_index-1;i++)
		rightPtr->key_array[i] = rightPtr->key_array[i+1];

	if (0 == rightPtr->leaf) {
		curPtr->child_array[curPtr->key_index] = rightPtr->child_array[0];
		for (i=0;i<rightPtr->key_index;i++)
			rightPtr->child_array[i] = rightPtr->child_array[i+1];
	}

	rightPtr->key_index--;
	*err = ESUCCESS;
}
//Get T's left-max key//
int BTreeGetLeftMax(node_t* T, BTREE_ERR *err) {

	if (T == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}

	if (T->leaf == FALSE) {
		return BTreeGetLeftMax(T->child_array[T->key_index], err);
	} else {
		return T->key_array[T->key_index-1];
	}
	*err = ESUCCESS;
}
//Get T's right-min key//
int BTreeGetRightMin(node_t* T, BTREE_ERR *err){
	
	if (T == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	if (T->leaf == FALSE) {
		return BTreeGetRightMin(T->child_array[0], err);
	} else {
		return T->key_array[0];
	}
	*err = ESUCCESS;
}
/* BTreeDeleteNoNone function recursively deletes X in root, handles both leaf
** and internal node:
**   1) If X is in leaf node, delete it.
**   2) If X is in internal node P:
**      1): If P's left neighbor -- prePtr -- has at least d keys, then replace X with
**      	prePtr's right-max key and then recursively delete it.
**      2): If P's right neighbor -- nexPtr -- has at least d keys, replace X with
**      	nexPtr's left-min key and then recursively delete it.
**      3): If both of prePtr and nexPtr have d-1 keys, merge X and nexPtr into
**      	prePtr, prePtr have 2*d-1 keys, and then recursively delete X in
**      	prePtr.
**   3) If X is not in internal node P, X must in P->child_array[i] keys. If child_array[i]
**      only has d-1 keys:
**      1): If child_array[i]'s neighbor have at least d keys, borrow a key from
**      	child_array[i]'s neighbor.
**      2): If both of child_array[i]'s left and right neighbor have d-1 keys, merge
**      	child_array[i] with one of its neighbor.
**   	In both situations recursively delete X.*/
void BTreeDeleteNoNone(int X, node_t* root, BTREE_ERR *err) {
	
	if (root == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}

	int i;
	//If root is a leaf node//
	if (root->leaf == TRUE) {
		i=0;

		while ((i < root->key_index) && (X > root->key_array[i])) {
			i++;
		}
		
		if (X == root->key_array[i]) {
			for (;i<root->key_index-1;i++)
				root->key_array[i] = root->key_array[i+1];
			root->key_index--;
		} else {
			fprintf(stderr, "Key is not in BTree\n\n");
			if (err != NULL)
				*err = EEMPTY;
			return;
		}
	} else {  //X is in internal node//
		i = 0;
		node_t* prePtr = NULL;
		node_t* nexPtr = NULL;
		while ( (i < root->key_index) && (X > root->key_array[i]) ) i++;
		
		if ( (i < root->key_index) && (X == root->key_array[i]) ) {
			prePtr = root->child_array[i];
			nexPtr = root->child_array[i+1];
			
			if (prePtr->key_index > NODE_ORDER-1) {
				int aPrecursor = BTreeGetLeftMax(prePtr, err);
				root->key_array[i] = aPrecursor;
				BTreeDeleteNoNone(aPrecursor,prePtr, err);
			} else if (nexPtr->key_index > NODE_ORDER-1) {
				int aSuccessor = BTreeGetRightMin(nexPtr, err);
				root->key_array[i] = aSuccessor;
				BTreeDeleteNoNone(aSuccessor,nexPtr, err);
			} else {
				merge_children(root,i,prePtr,nexPtr, err);
				BTreeDeleteNoNone(X,prePtr, err);
			}
		} else {
			prePtr = root->child_array[i];
			node_t *preprePtr = NULL;
			if (i < root->key_index)
				nexPtr = root->child_array[i+1];
			if (i > 0)
				preprePtr = root->child_array[i-1];
			if (NODE_ORDER-1 == prePtr->key_index) {
				if ((preprePtr != NULL) && (preprePtr->key_index > NODE_ORDER-1))
					BTreeBorrowFromLeft(root,i-1,preprePtr,prePtr, err);
				else if ((nexPtr != NULL) && (nexPtr->key_index > NODE_ORDER-1))
					BTreeBorrowFromRight(root,i,nexPtr,prePtr, err);
				else if (preprePtr != NULL) {
					merge_children(root,i-1,preprePtr,prePtr, err);
					prePtr = preprePtr;
				} else
					merge_children(root,i,prePtr,nexPtr, err);
			}
			BTreeDeleteNoNone(X, prePtr, err);
		}
	}
	*err = ESUCCESS;
}
/* delete operation deletes X from btree b up-to-down and no-backtrack.
** Before deleting, checks if it's necessary to merge the root and its children
** to reduce the tree's height. Executes BTreeDeleteNoNone to recursively delete*/
node_t* delete(int key, btree_t* b, BTREE_ERR *err) {
	
	if (b == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	//if the root of T only have 1 key and both of T's two child have d-1//
	//keys, then merge the children and the root. Guarantees not need to backtrack.//
	if (b->root->key_index == 1) {
		node_t* child1 = b->root->child_array[0];
		node_t* child2 = b->root->child_array[1];
		if ((child1 != NULL) && (child2 != NULL)) {
			if ((NODE_ORDER-1 == child1->key_index) && (NODE_ORDER-1 == child2->key_index)) {
				merge_children(b->root, 0, child1, child2, err);
				free(b->root);
				BTreeDeleteNoNone(key, child1, err);
				return child1;
			}
		}
	}
	BTreeDeleteNoNone(key, b->root, err);
	*err = ESUCCESS;
	return b->root;
}
//free memory//
void node_delete(node_t* node, BTREE_ERR *err) {

	if (node == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	if (node->leaf == TRUE) {
		free(node);
	}
	*err = ESUCCESS;
}
//free memory//
void btree_delete(btree_t* T, BTREE_ERR *err) {

	if (T == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}

	node_delete(T->root, err);
	free(T);
	*err = ESUCCESS;
}