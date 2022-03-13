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

