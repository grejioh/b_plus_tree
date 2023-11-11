#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

// macro to config B plus tree
// an internal node can have at most N children
#include <stdbool.h>
#define NUMBER_OF_CHILD 4 


// B plus tree node
typedef struct B_plus_tree_node {
    // search-keys
    int *key; // pointer to dynamically allocated array
    // capacity of key array
    int key_capacity; 
    // keys number
    int key_num;

    // pointers, maybe point to data or other nodes
    void **pointer; // pointer to dynamically allocated array
    // pointer capacity
    int pointer_capacity;
    // pointer number
    int pointer_num;

    // bool value indicate whether this node is a leaf node
    bool is_leaf;

    // parent pointer, lazily update (only update when find)
    struct B_plus_tree_node *parent;
    
} B_plus_tree_node;

// B plus tree
typedef struct B_plus_tree {
    // root node
    B_plus_tree_node *root;
} B_plus_tree;

// data structure represent a database entry
typedef struct Record {
    // key
    int key;
    // value
    int value;
} Record;

// function to insert a record into B plus tree
void B_plus_tree_insert(B_plus_tree *tree, int Key, Record *Ptr_to_data);

// function to find a record in B plus tree
Record *B_plus_tree_find(B_plus_tree *tree, int v);

// print B plus tree
void B_plus_tree_print(B_plus_tree *tree);

// deconstruct B plus tree
void B_plus_tree_deconstruct(B_plus_tree *tree);

// 其他函数声明...

#endif // B_PLUS_TREE_H