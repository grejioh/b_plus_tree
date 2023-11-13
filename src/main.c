#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "B_plus_tree.h"

// test sequential insert
void test_sequential_insert(void) {
    
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

// test range query
void test_range_query(void) {
    printf("Test range query\n");
    // test insert to build a B plus tree
    B_plus_tree *tree = malloc(sizeof(B_plus_tree));
    tree->root = NULL;

    int times = 1000;

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

    // shuffle the array
    for(int i = 0; i < times; i++) {
        int j = rand() % times;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }

    // test range query
    int i = 0;
    for(i = 1; i < times; i++) {
        int v1 = a[i-1];
        int v2 = a[i];
        Record** result_set;
        int result_count = B_plus_tree_find_range(tree, v1, v2, &result_set);

        printf("range query: %d, %d\n", v1, v2);
        if(result_count > 0) {
            assert(result_count == v2 - v1 + 1);
            printf("result_set[0] %d\n", result_set[0]->key);
            assert(result_set[0]->key == v1);
            // free should be done by the caller
            free(result_set);
        }

        printf("result count: %d\n", result_count);
        printf("\n");
        printf("\n");
    }
    
}
// check remain
void check_remain(B_plus_tree *tree, int i, int times) {
        // check for total number of records
        int total_keys = 0;
        for(int j = 0; j < times; j++) {
            Record *record = B_plus_tree_find(tree, j);
            if(record != NULL && record->value == j) {
                total_keys++;
            }
        }
        assert(total_keys == times - i);
}

// test delete 
void test_delete(int times) {
    printf("Test delete\n");
    // test insert to build a B plus tree
    B_plus_tree *tree = malloc(sizeof(B_plus_tree));
    tree->root = NULL;

    int* a = malloc(times * sizeof(int));
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
    // test find and delete use the key to find the record
    printf("==start delete==\n");
    for(int i = 0; i < times; i++) {

        //check_remain(tree, i, times);
        int v = a[i];
        Record *record = B_plus_tree_find(tree, v);
        // printf("find key: %d, value: %d\n", v, record->value);
        assert(record->key == v);
        assert(record->value == v);
        B_plus_tree_delete(tree, v);
        record = B_plus_tree_find(tree, v);
        assert(record == NULL);
        //B_plus_tree_print(tree);
    }
    free(a);

    // free the tree and all node
    B_plus_tree_deconstruct(tree);
}


int main(void) {
    // test_sequential_insert();
    // test_random_unique_insert(1000000);
    // test_range_query();
    // record run time
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    test_delete(100000000);
    end = clock();
    // print the time in seconds
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("time used: %f\n", cpu_time_used);

    return 0;
}
