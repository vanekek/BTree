#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "btree.h"
//Function allocates node, sets default values for properties//
Node *create_node(BTREE_ERR *err) {
    
	Node *new_node = (Node *) malloc(sizeof(Node));

	if (new_node == NULL) {
		fprintf(stderr, "Out of memory\n");
		if (err != NULL)
			*err = EMALLOC;
		return;
	}

	for (int i = 0; i < NODE_KEYS; i++) {
		new_node->keys[i] = 0;
	}

	for (int i = 0;i < NODE_POINTERS; i++) {
		new_node->child[i] = NULL;
	}

	new_node->current_size = 0;
	new_node->leaf = TRUE;

	*err = ESUCCESS;
	return new_node;
}
//Allocating a btree//
BTree *create_btree(BTREE_ERR *err) {

    BTree *new_root = (BTree *) malloc(sizeof(BTree));

	if (new_root == NULL) {
		fprintf(stderr, "Out of memory\n");
		if (err != NULL)
			*err = EMALLOC;
		return;
	}

	Node *head = create_node(err);

	new_root->root = head;

	*err = ESUCCESS;
	return new_root;
}
//printing of node function//
void print_node(Node *n, BTREE_ERR *err) {
    int i, q;

	if (n == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}	

	printf("  Index: %d\n", n->current_size);

	printf("   Leaf: ");
	if (n->leaf) {
		printf("TRUE\n");
	} else {
		printf("FALSE\n");
	}

	printf("  Array:");
	for (i = 0; i < n->current_size; i++) {
		printf(" [%d : %d]", i, n->keys[i]);
	}

	printf("\n  Childs:");
	if (n->leaf) {
		printf(" NONE");
	} else {
		for (q = 0; q < NODE_POINTERS; q++) {
			printf(" [%d : %lx]", q, (unsigned long int)n->child[q]);
		}
	}
	*err = ESUCCESS;
	printf("\n\n");
}
/* Search operation searches key X in btree T. Recursively traverse the tree
** from top to bottom. At each level Search choose the maximum key whose
** value is greater than or equal to the X. If equal to X, found.
** Otherwise continue to traverse. */
void search_in_node(int key, Node *node, BTREE_ERR *err) {

	if (node == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}

	int i = 0;

	while ((i < node->current_size) && (key > node->keys[i])) {
		i++;
	}

	if ((i <= node->current_size) && (key == node->keys[i])) {
		printf("Key %d is found in Node:\n", key);
		printf("[ ");
		for (int k = 0; k < node->current_size; ++k) {
			printf("%d ", node->keys[k]);
		}
		printf("]");
	} else if (node->leaf == TRUE) {
		printf("Key %d wasn't found");
	} else {
		search_in_node(key, node->child[i], err);
	}
	*err = ESUCCESS;
}
//Search the key function for tree now//
void search(int key, BTree *tree, BTREE_ERR *err) {
	search_in_node(key, tree->root, err);
}
/* Inserts key into node.*/
void insert_node(Node *node, int key, BTREE_ERR *err) {
	int index;

	for (index = node->current_size; (index > 0) && (key < node->keys[index - 1]); --index) {
		node->keys[index] = node->keys[index - 1];
		node->child[index + 1] = node->child[index];
	}

	node->keys[index] = key;
	node->child[index + 1] = node->child[index];
	node->current_size += 1;
}
/* split_child function is splitting node, that is full (too many children to work further with it)*/
void split_child(Node *parent, int i, BTREE_ERR *err) {

	if ((i > 5) || (i < 0)) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return NULL;
	}

	Node *splitted_node = parent->child[i];
	Node *new_node = create_node(err);
	new_node->leaf = splitted_node->leaf;
	new_node->current_size = NODE_ORDER - 1;

	for (int k = 0; k < NODE_ORDER - 1; ++k) {
		new_node->keys[k] = splitted_node->keys[k + NODE_ORDER];
	}
	if (splitted_node->leaf == FALSE) {
		for (int j = 0; j < NODE_ORDER; ++j) {
			new_node->child[j] = splitted_node->child[j+NODE_ORDER];
		}
	}
	splitted_node->current_size = NODE_ORDER - 1;

	insert_node(parent, splitted_node->keys[NODE_ORDER - 1], err); //we are lifting median key to parent//
	parent->child[i + 1] = new_node;
}
/* Insert function inserts key into tree*/
void insert(int key, BTree *tree, BTREE_ERR *err) {
	
	if (tree == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	if (tree->root->current_size == NODE_KEYS) {
		Node *new_root = create_node(err);
		new_root->leaf = FALSE;
		new_root->child[0] = tree->root;
		tree->root = new_root;
		split_child(new_root, 0, err);
	}

	Node *current_node = tree->root;
	while(current_node->leaf == FALSE) {

		int index = current_node->current_size - 1;
		while ((index >= 0) && (key <= current_node->keys[index])) {
			index -= 1;
		}
		index += 1;

		if (current_node->child[index]->current_size == NODE_KEYS) {
			split_child(current_node, index, err);
			if (current_node->keys[index] < key) {
				index += 1;
			}
		}

		current_node = current_node->child[index];
	}

	insert_node(current_node, key, err);
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