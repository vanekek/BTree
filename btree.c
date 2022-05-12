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

	Node *old = new_root->root;

	new_root->root = head;

	*err = ESUCCESS;
	return new_root;
}
//printing of node function//
void print_node(Node *n, int ots, BTREE_ERR *err) {

	if (n == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}	

	for (unsigned i = 0; i < ots; i++) {
		printf("\t");
	}

	for (unsigned i = 0; i < n->current_size; i++) {
		printf("%d", n->keys[i]);
		printf(" ");
	}
	printf("\n");

	if (n->leaf == FALSE) {
		ots++;
		for (unsigned i = 0; i <= n->current_size; i++) {
			print_node(n->child[i], ots, err);
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
		printf("]\n");
	} else if (node->leaf == TRUE) {
		printf("Key %d wasn't found\n", key);
	} else {
		search_in_node(key, node->child[i], err);
	}
	*err = ESUCCESS;
}
//Search the key function for tree now//
void search(int key, BTree *tree, BTREE_ERR *err) {
	if (tree == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	search_in_node(key, tree->root, err);
}
/* Inserts key into node.*/
int insert_node(Node *node, int key, BTREE_ERR *err) {
	int index;

	for (index = node->current_size; (index > 0) && (key < node->keys[index - 1]); --index) {
		node->keys[index] = node->keys[index - 1];
		node->child[index + 1] = node->child[index];
	}

	node->keys[index] = key;
	node->child[index + 1] = node->child[index];
	node->current_size += 1;
	
	*err = ESUCCESS;
	return index;
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
		while ((index >= 0) && (key < current_node->keys[index])) {
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
//Deletes key from a node on index i//
int delete_from_node(Node *node, int i, BTREE_ERR *err) {
	node->current_size -= 1;
	int res = node->keys[i];
	node->keys[i] = 0;

	for (int j = i; j < node->current_size; ++j) {
		node->keys[j] = node->keys[j + 1];
		node->child[j + 1] = node->child[j + 2];
	}

	return res;
}
/* merge_children function merges the root->K[index] and it's two children
** and then set child1 to the new root.
** 0 - not modified, 1 - modified with new root, 2 - modified without new root*/
int merge_children(BTree *tree, Node *parent, int i, BTREE_ERR *err) {
	
	if ((parent == NULL) || (i < 0)) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return NULL;
	}
	
	Node *left = parent->child[i];
	Node *right = parent->child[i + 1];
	
	left->keys[left->current_size] = delete_from_node(parent, i, err);
	left->current_size += 1;
	int j = left->current_size;

	for (int k = 0; k < right->current_size; ++k) {
		left->keys[j + k] = right->keys[k];
		left->child[j + k] = right->child[k];
	}
	left->current_size	+= right->current_size;
	left->child[left->current_size] = right->child[right->current_size];

	free(right);

	if (parent->current_size == 0) {
		tree->root = left;
		free(parent);
		*err = ESUCCESS;
		return 1;
	}

	*err = ESUCCESS;
	return 2;
}
//Checking that parent->child[i] has at least NODE_ORDER - 1 keys, if not we change it//
int check_size(BTree *tree, Node *parent, int i, BTREE_ERR *err) {
	Node *checked = parent->child[i];

	if (checked->current_size < NODE_ORDER) {
		if ((i != 0) && (parent->child[i - 1]->current_size >= NODE_ORDER)) {
			Node *left = parent->child[i -1];
			int k = insert_node(checked, parent->keys[i - 1], err);
			for (k; k != 0; --k) {
				checked->child[k] = checked->child[k - 1];
			}
			checked->child[0] = left->child[left->current_size];
			parent->keys[i - 1] = delete_from_node(left, left->current_size - 1, err);
		}
		else if ((i != parent->current_size) && (parent->child[i + 1]->current_size >= NODE_ORDER)) {
			Node *right = parent->child[i + 1];
			insert_node(checked, parent->keys[i], err);
			checked->child[checked->current_size] = right->child[0];
			right->child[0] = right->child[1];
			parent->keys[i] = delete_from_node(right, 0, err);
		}
		else if (i != 0) {
			return merge_children(tree, parent, i - 1, err);
		}
		else {
			return merge_children(tree, parent, i, err);
		}

		return 2;
	}

	return 0;
}
//deleting key from btree//
void delete(BTree *tree, int key, BTREE_ERR *err) {
	
	if (tree == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return NULL;
	}
	
	Node *current = tree->root;
	while (1) {
		int i = 0;
		while ((i < current->current_size) && (current->keys[i] < key)) {
			++ i;
		}
		if ((i < current->current_size) && !((current->keys[i] < key) || (key < current->keys[i]))) {
			int res = current->keys[i];

			if (current->leaf == TRUE) {
				delete_from_node(current, i ,err);
			} else {
				Node *left = current->child[i];
				Node *right = current->child[i + 1];

				if (left->current_size >= NODE_ORDER) {
					while (left->leaf != TRUE) {
						check_size(tree, left, left->current_size, err);
						left = left->child[left->current_size];
					}
					current->keys[i] = delete_from_node(left, left->current_size - 1, err);
				} else if (right->current_size >= NODE_ORDER) {
					while (right->leaf != TRUE) {
						check_size(tree, right, 0, err);
						right = right->child[0];
					}
					current->keys[i] = delete_from_node(right, 0, err);
				} else {
					merge_children(tree, current, i, err);
					current = left;
					continue;
				}
			}
			printf("Key %d was successfully deleted !\n", res);
			return;
		} else {
			if (current->leaf == TRUE) {
					fprintf(stderr, "Invalid argument: key %d was not found in btree\n", key);
					if (err != NULL)
						*err = EINVARG;
					return NULL;
			}
			int modification = check_size(tree, current, i, err);
			if (modification == 1) {
				current = tree->root;
			} else {
				int j = 0;
				while ((j < current->current_size) && (current->keys[j] < key)) {
					++ j;
				}
				current = current->child[j];
			}
		}
	}
}

//free memory//
void node_delete(Node *node, BTREE_ERR *err) {

	if (node == NULL) {
		fprintf(stderr, "Invalid argument\n");
		if (err != NULL)
			*err = EINVARG;
		return;
	}
	
	if (node->leaf == FALSE) {
		for (unsigned i = 0; i <= node->current_size; i++) {
			node_delete(node->child[i], err);
		}
	}
	free(node);

	*err = ESUCCESS;
}
//free memory//
void btree_delete(BTree *T, BTREE_ERR *err) {

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