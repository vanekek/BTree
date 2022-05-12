#include "btree.h"
int main() {
    BTREE_ERR err;

    BTree *B = NULL;
    Node *k = NULL;   
    Node *kk = NULL;
    Node *f = NULL;
    //Tests for invalid arguments//
    search(3, B, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_2\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_2\t->\tPASSED\n");
    
    print_node(k, 2, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_3\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_3\t->\tPASSED\n");
    insert(3, B, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_6\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_6\t->\tPASSED\n");
    
    merge_children(B, k, 0, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_7\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_7\t->\tPASSED\n");
    
    delete(B, 4, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_13\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_13\t->\tPASSED\n");
    
    node_delete(f, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_14\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_14\t->\tPASSED\n");
    
    btree_delete(B, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_15\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_15\t->\tPASSED\n");
    search_in_node(1, k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_15\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_15\t->\tPASSED\n");
    split_child(k, 9, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_15\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_15\t->\tPASSED\n");
    //Tests with data//
    BTree *T = create_btree(&err);
    if (err != EMALLOC) 
        fprintf(stdout, "Test_16\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_16\t->\tPASSED\n");

    BTree *tree = create_btree(&err);
    Node *left = create_node(&err);
    Node *right = create_node(&err);

    
    //Tests with dataset//
    int array[] = {1, -11, 12, 13, 15, 16, 17, 18, 19, 20, 25, 28, 29, 31,
				35, 36, 39, 41, 42, 45, 55, 58, 59, 61, 67, 71, 73, 74,
				76, 80, 81, 82, 83, 88, 89, 99, 83, 91, 93, 94, 95, 98,
                -23, -234, -24, -3, -38, -82, -49, -72, -84, -27, -22,
				35, -9, -29, -374, -84, -24 , -92, -83, -372, -756, 10987};
    for(int i = 0; i < 63; ++i) {
      insert(array[i], T, &err);
      if (err != ESUCCESS) 
          fprintf(stdout, "Test_%d\t->\tFAILED\n", 17+i);
      else
          fprintf(stdout, "Test_%d\t->\tPASSED\n", 17+i);
    }

    for(int i = 0; i < 63; ++i) {
      search(array[i], T, &err);
      if (err != ESUCCESS) 
          fprintf(stdout, "Test_%d\t->\tFAILED\n", 80+i);
      else
          fprintf(stdout, "Test_%d\t->\tPASSED\n", 80+i);
    }

    print_node(T->root, 2, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_144\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_144\t->\tPASSED\n");
    //Test for node printing//
    Node *node = T->root;
    node = T->root;
    while (node->leaf != TRUE) node = node->child[0];
    print_node(node, 2, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_145\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_145\t->\tPASSED\n");
    node = NULL;
    free(node);
    //Tests for deleting keys//
    delete(T, 15, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_146\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_147\t->\tPASSED\n");
    delete(T, 19, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_148\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_149\t->\tPASSED\n");
    delete(T, 73, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_150\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_151\t->\tPASSED\n");
    delete(T, -24, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_152\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_153\t->\tPASSED\n");
    delete(T, -72, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_154\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_155\t->\tPASSED\n");
    for (int i = -100; i < 0; ++i) {
        delete(T, i, &err);
    }
    for (int i = 100; i > 0; --i) {
        delete(T, i, &err);
    }
    search(15, T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_209\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_210\t->\tPASSED\n");
    btree_delete(T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_211\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_212\t->\tPASSED\n");
    return 0;
}