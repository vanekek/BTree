#include "btree.h"
#include <stdio.h>

int main() {
    btree_t* T = create_btree();
    int array[] = {1, -11, 12, 13, 15, 16, 17, 18, 19, 20, 25, 28, 29, 31,
				 35, 36, 39, 41, 42, 45, 55, 58, 59, 61, 67, 71, 73, 74,
				 76, 80, 81, 82, 83, 88, 89, 99, 83, 91, 93, 94, 95, 98, -23, -234, -24, -3, -38, -82, -49, -72, -84, -27, -22,
				   -35, -9, -29, -374, -84, -24 , -92, -83, -372, -756};
    for(int i = 0; i < 63; ++i) T->root = insert(array[i], T);
    for(int i = 0; i < 63; ++i) search(array[i], T->root);
    print_node(T->root);
    node_t* node = create_node();
    node = T->root;
    while (node->leaf != TRUE) node = node->child_array[0];
    print_node(node);
    T->root = delete(15, T);
    T->root = delete(19, T);
    T->root = delete(98, T);
    T->root = delete(-24, T);
    T->root = delete(-72, T);
    search(15, T->root);
    btree_delete(T);
    return 0;
}