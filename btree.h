#define NODE_ORDER		3  //degree of tree//
#define NODE_POINTERS	(NODE_ORDER*2)
#define NODE_KEYS		NODE_POINTERS - 1 

typedef enum{
	ESUCCESS = 0,
	EINVARG,
	EFULL,
	EEMPTY
} BTREE_ERR;

typedef enum {
    FALSE = 0,
    TRUE = 1
} boolean;

typedef struct node {
	int key_array[NODE_KEYS];
	struct node *child_array[NODE_POINTERS];
	unsigned int key_index;
	boolean leaf;
} node_t;

typedef struct {
	node_t *root;
	unsigned int order;
} btree_t;

typedef struct {
	node_t *node_pointer;
	int key;
	boolean found;
	unsigned int depth;
} result_t;

node_t* create_node();
btree_t* create_btree();

result_t* get_resultset();
result_t* search(int key, node_t *node);

void print_node(node_t* n);

void split_child(node_t* parent_node, int i, node_t* child_array);
void insert_nonfull(int key, node_t* n);
node_t* insert(int key, btree_t* node);

node_t* node_delete(int key, btree_t* tree);






