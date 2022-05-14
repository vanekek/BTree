#include <stdio.h>

#define NODE_ORDER		3  //degree of tree//
#define NODE_POINTERS	(NODE_ORDER*2)
#define NODE_KEYS		NODE_POINTERS - 1 
//errors enum//
typedef enum {
	ESUCCESS = 0,
	EINVARG,
	EEMPTY,
	EMALLOC
} BTREE_ERR;
//enum for define bool type//
typedef enum {
    FALSE = 0,
    TRUE = 1
} boolean;
// node of btree//
typedef struct Node {
	int keys[NODE_KEYS];
	struct Node *child[NODE_POINTERS];
	unsigned int current_size;
	boolean leaf;
} Node;
//btree//
typedef struct {
	Node *root;
} BTree;
//creating functions//
Node* create_node(BTREE_ERR *err);
BTree* create_btree(BTREE_ERR *err);
//search functions//
void search_in_node(int key, Node *node, BTREE_ERR *err);
void search(int key, BTree *tree, BTREE_ERR *err);
//print of node function//
void print_node(Node *n, int ots, BTREE_ERR *err);
//insert functions//
void split_child(Node *parent, int i, BTREE_ERR *err);
int insert_node(Node *node, int key, BTREE_ERR *err);
void insert(int key, BTree *tree, BTREE_ERR *err);
//functions for deleting keys//
int merge_children(BTree *tree, Node *parent, int i, BTREE_ERR *err);
int check_size(BTree *tree, Node *parent, int i, BTREE_ERR *err);
int delete_from_node(Node *node, int i, BTREE_ERR *err);	
void delete(BTree *tree, int key, BTREE_ERR *err);
//functions for memory free//
void node_delete(Node *node, BTREE_ERR *err);
void btree_delete(BTree *T, BTREE_ERR *err);






