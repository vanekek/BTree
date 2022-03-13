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

node_t* insert(int key, btree_t* node) {
	node_t *root = b->root;

	if(root->key_index == NODE_KEYS){
		node_t *newNode = create_node();
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
