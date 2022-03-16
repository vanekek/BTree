#include "btree.h"
#include <stdio.h>

int main() {
    btree_t* T = create_btree();
    for (int i = 1; i < 11; ++i) {
        T->root = insert(i, T);
    }
    result_t* res = search(5, T->root);
    print_resultset(res);
    T->root = delete(5, T);
    result_t* ress = search(5, T->root);
    print_resultset(ress);
    print_node(T->root);
    return 0;
}