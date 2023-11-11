#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "B_plus_tree.h"



int main() {
    
    printf("Hello, World!\n");

    // test insert to build a B plus tree
    B_plus_tree *tree = malloc(sizeof(B_plus_tree));
    tree->root = NULL;
    // insert some records
    for(int i = 0; i < 10; i++) {
        Record *record = malloc(sizeof(Record));
        record->key = i;
        record->value = i;
        B_plus_tree_insert(tree, i, record);
        printf("====================================\n");
        printf("insert key: %d, value: %d\n", record->key, record->value);
        B_plus_tree_print(tree);
    }
    // print the tree
    B_plus_tree_print(tree);

    // test find, use the key to find the record
    for(int i = 0; i < 10; i++) {
        Record *record = B_plus_tree_find(tree, i);
        assert(record->key == i);
        assert(record->value == i);
        printf("find key: %d, value: %d\n", record->key, record->value);
    }

    // free the tree and all node



    return 0;
}