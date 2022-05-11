#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    BTREE_ERR err;

    BTree *B = NULL;
    Node *k = NULL;   
    Node *kk = NULL;
    Node *f = NULL;
    //Tests for invalid arguments//
    if (err != EINVARG) 
        fprintf(stdout, "Test_1\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_1\t->\tPASSED\n");

    search(3, k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_2\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_2\t->\tPASSED\n");
    
    print_node(k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_3\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_3\t->\tPASSED\n");
    
    split_child(k, 7, kk, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_4\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_4\t->\tPASSED\n");
    
    insert_nonfull(2,k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_5\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_5\t->\tPASSED\n");
    
    insert(3, k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_6\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_6\t->\tPASSED\n");
    
    merge_children(k, 5, kk, f, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_7\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_7\t->\tPASSED\n");
    
    BTreeBorrowFromLeft(k, 4, kk, f, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_8\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_8\t->\tPASSED\n");
    
    BTreeBorrowFromRight(k, 4, kk, f, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_9\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_9\t->\tPASSED\n");
    
    BTreeDeleteNoNone(5, k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_10\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_10\t->\tPASSED\n");
    
    int h = BTreeGetLeftMax(k,&err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_11\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_11\t->\tPASSED\n");
    
    h = BTreeGetRightMin(k, &err);
    if (err != EINVARG) 
        fprintf(stdout, "Test_12\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_12\t->\tPASSED\n");
    
    k = delete(4, B, &err);
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

    BTree *T = create_btree(&err);
    if (err != EMALLOC) 
        fprintf(stdout, "Test_16\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_16\t->\tPASSED\n");
    //Tests with dataset//
    int array[] = {1, -11, 12, 81, 82, 83, 88, -29, -374, -84, 99, 13, -23, 18, 19, 20, -372, 25, 28, 29, 31,
				 35, 36, 39, 41, 73, 74,
				 76, 80, 83, 98, 42, 45, -234, -24, 55, 58, 59, 61, 67, 71, -3, -38, -82, -49, -72,
				-35, -9, -24 , -92, 89, -83, 15, 16, 17, -84, -27, -22, -75691, 93, 94, 95};

    for(int i = 0; i < 63; ++i) {
      T->root = insert(array[i], T, &err);
      if (err != ESUCCESS) 
          fprintf(stdout, "Test_%d\t->\tFAILED\n", 17+i);
      else
          fprintf(stdout, "Test_%d\t->\tPASSED\n", 17+i);
    }

    for(int i = 0; i < 63; ++i) {
      search(array[i], T->root, &err);
      if (err != ESUCCESS) 
          fprintf(stdout, "Test_%d\t->\tFAILED\n", 80+i);
      else
          fprintf(stdout, "Test_%d\t->\tPASSED\n", 80+i);
    }

    print_node(T->root, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_144\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_144\t->\tPASSED\n");
    //Test for node printing//
    Node *node = T->root;
    node = T->root;
    while (node->leaf != TRUE) node = node->child[0];
    print_node(node, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_145\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_145\t->\tPASSED\n");
    node = NULL;
    free(node);
    //Tests for deleting keys//
    T->root = delete(15, T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_146\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_147\t->\tPASSED\n");
    T->root = delete(19, T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_148\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_149\t->\tPASSED\n");
    T->root = delete(98, T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_150\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_151\t->\tPASSED\n");
    T->root = delete(-24, T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_152\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_153\t->\tPASSED\n");
    T->root = delete(-72, T, &err);
    if (err != ESUCCESS) 
        fprintf(stdout, "Test_154\t->\tFAILED\n");
    else
        fprintf(stdout, "Test_155\t->\tPASSED\n");
    for(int i = 0; i < 63; ++i) {
        T->root = delete(array[i], T, &err);
        if (err != ESUCCESS) 
            fprintf(stdout, "Test_%d\t->\tFAILED\n", 155 + i);
        else
            fprintf(stdout, "Test_%d\t->\tPASSED\n", 155 + i);
    }
    search(15, T->root, &err);
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