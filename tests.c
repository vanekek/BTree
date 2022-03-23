#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    btree_t* T = create_btree();
    int array[] = {1, -11, 12, 81, 82, 83, 88, -29, -374, -84, 99, 13, -23, 18, 19, 20, -372, 25, 28, 29, 31,
				 35, 36, 39, 41, 73, 74,
				 76, 80, 83, 98, 42, 45, -234, -24, 55, 58, 59, 61, 67, 71, -3, -38, -82, -49, -72,
				-35, -9, -24 , -92, 89, -83, 15, 16, 17, -84, -27, -22, -75691, 93, 94, 95};
    for(int i = 0; i < 63; ++i) T->root = insert(array[i], T);
    for(int i = 0; i < 63; ++i) search(array[i], T->root);
    print_node(T->root);
    node_t* node = T->root;
    node = T->root;
    while (node->leaf != TRUE) node = node->child_array[0];
    print_node(node);
    node = NULL;
    free(node);
    T->root = delete(15, T);
    T->root = delete(19, T);
    T->root = delete(98, T);
    T->root = delete(-24, T);
    T->root = delete(-72, T);
    for(int i = 0; i < 63; ++i) T->root = delete(array[i], T);
    search(15, T->root);
    btree_delete(T);
    return 0;
}