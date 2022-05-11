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
node* create_node(BTREE_ERR *err);
BTree* create_btree(BTREE_ERR *err);
//search functions//
void search_in_node(int key, Node *node, BTREE_ERR *err);
void search(int key, BTree *tree, BTREE_ERR *err)
//print of node function//
void print_node(Node *n, BTREE_ERR *err);
//insert functions//
void split_child(Node *parent_node, int i, Node *child_array, BTREE_ERR *err);
void insert_node(Node *node, int key, BTREE_ERR *err);
void insert(int key, BTree *tree, BTREE_ERR *err);
//functions for deleting keys//
void merge_children(node_t* root, int index, node_t* child1, node_t* child2, BTREE_ERR *err);
void BTreeBorrowFromLeft(node_t* root, int index, node_t* leftPtr, node_t* curPtr, BTREE_ERR *err);
void BTreeBorrowFromRight(node_t* root, int index, node_t* rightPtr, node_t* curPtr, BTREE_ERR *err);
void BTreeDeleteNoNone(int X, node_t* root, BTREE_ERR *err);
node_t* delete(int key, btree_t* tree, BTREE_ERR *err);

int BTreeGetLeftMax(node_t *T, BTREE_ERR *err);
int BTreeGetRightMin(node_t *T, BTREE_ERR *err);
//functions for memory free//
void node_delete(node_t* node, BTREE_ERR *err);
void btree_delete(btree_t* T, BTREE_ERR *err);






