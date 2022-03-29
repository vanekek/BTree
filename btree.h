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
//enum for define bool//
typedef enum {
    FALSE = 0,
    TRUE = 1
} boolean;
// node of btree//
typedef struct node {
	int key_array[NODE_KEYS];
	struct node *child_array[NODE_POINTERS];
	unsigned int key_index;
	boolean leaf;
} node_t;
//btree//
typedef struct {
	node_t *root;
} btree_t;
//struct for the result of search function and effective printing//
typedef struct {
	node_t *node_pointer;
	int key;
	boolean found;
	unsigned int depth;
} result_t;
//creating functions//
node_t* create_node(BTREE_ERR *err);
btree_t* create_btree(BTREE_ERR *err);
//search functions//
result_t* get_resultset(BTREE_ERR *err);
void print_resultset(result_t* res, BTREE_ERR *err);
void search(int key, node_t *node, BTREE_ERR *err);
//print of node function//
void print_node(node_t* n, BTREE_ERR *err);
//insert functions//
void split_child(node_t* parent_node, int i, node_t* child_array, BTREE_ERR *err);
void insert_nonfull(int key, node_t* n, BTREE_ERR *err);
node_t* insert(int key, btree_t* node, BTREE_ERR *err);
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






