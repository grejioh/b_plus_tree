#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "B_plus_tree.h"


// test sequential insert
void test_sequential_insert() {
    
    printf("Test sequential insert\n");

    // test insert to build a B plus tree
    B_plus_tree *tree = malloc(sizeof(B_plus_tree));
    tree->root = NULL;
    // insert some records
    for(int i = 0; i < 10; i++) {
        Record *record = malloc(sizeof(Record));
        record->key = i;
        record->value = i;
        // printf("====================================\n");
        // printf("insert key: %d, value: %d\n", record->key, record->value);
        B_plus_tree_insert(tree, i, record);
        // B_plus_tree_print(tree);
    }
    // print the tree
    // B_plus_tree_print(tree);

    // test find, use the key to find the record
    for(int i = 0; i < 10; i++) {
        Record *record = B_plus_tree_find(tree, i);
        assert(record->key == i);
        assert(record->value == i);
        printf("find key: %d, value: %d\n", record->key, record->value);
    }

    // free the tree and all node
    B_plus_tree_deconstruct(tree);
}

// test random insert
void test_random_unique_insert(int times) {
    printf("Test random insert\n");
    // test insert to build a B plus tree
    B_plus_tree *tree = malloc(sizeof(B_plus_tree));
    tree->root = NULL;

    int a[times];
    for(int i = 0; i < times; i++ ) {
        a[i] = i;
    }
    
    // shuffle the array
    for(int i = 0; i < times; i++) {
        int j = rand() % times;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }


    // insert some records
    for(int i = 0; i < times; i++) {
        Record *record = malloc(sizeof(Record));
        record->key = a[i];
        record->value = a[i];
        // printf("====================================\n");
        // printf("insert key: %d, value: %d\n", record->key, record->value);
        B_plus_tree_insert(tree, record->key, record);
        // B_plus_tree_print(tree);
    }
    // print the tree
    // B_plus_tree_print(tree);

    // shuffle the array
    for(int i = 0; i < times; i++) {
        int j = rand() % times;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
    // test find, use the key to find the record
    for(int i = 0; i < times; i++) {
        int v = a[i];
        Record *record = B_plus_tree_find(tree, v);
        assert(record->key == v);
        assert(record->value == v);
        // printf("find key: %d, value: %d\n", record->key, record->value);
    }

    // free the tree and all node
    B_plus_tree_deconstruct(tree);
}

int main() {
    test_sequential_insert();
    test_random_unique_insert(1000000);
    return 0;
}