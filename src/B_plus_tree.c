#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "B_plus_tree.h"


// find a record by key
Record *B_plus_tree_find(B_plus_tree *tree, int v) {
    B_plus_tree_node *C = tree->root;

    while(C->is_leaf == false) {
        // i is the smallest number such that v <= C->key[i]
        int i = 0;
        while(i < C->key_num && v > C->key[i]) {
            i++;
        }
        if (i == C->key_num) {
            // C = last non-null pointer in C
            C = C->pointer[C->pointer_num - 1];
        } else if (v == C->key[i]) {
            C = C->pointer[i+1];
        } else {
            // v < C->key[i]
            C = C->pointer[i];
        }
    }

    int i = 0;
    for(i = 0; i < C->key_num; i++) {
        if (C->key[i] == v) {
            return C->pointer[i];
        }
    }
    return NULL;
}

// create a new B plus tree node
B_plus_tree_node *B_plus_tree_node_create_node(int key_capacity, int pointer_capacity) {
    B_plus_tree_node *node = malloc(sizeof(B_plus_tree_node));
    // does this need calloc?
    node->key = malloc(sizeof(int) * key_capacity);
    node->pointer = malloc(sizeof(void *) * pointer_capacity);

    node->key_capacity = key_capacity;
    node->pointer_capacity = pointer_capacity;

    node->key_num = 0;
    node->pointer_num = 0;
    node->is_leaf = false;
    node->parent = NULL;

    return node;
}

// find_leaf to insert a record
B_plus_tree_node* B_plus_tree_find_leaf(B_plus_tree *tree, int v) {
    B_plus_tree_node *C = tree->root;

    while(C->is_leaf == false) {
        // i is the smallest number such that v <= C->key[i]
        int i = 0;
        while(i < C->key_num && v > C->key[i]) {
            i++;
        }
        if (i == C->key_num) {
            // C = last non-null pointer in C
            C = C->pointer[C->pointer_num - 1];
        } else if (v == C->key[i]) {
            C = C->pointer[i+1];
        } else {
            // v < C->key[i]
            C = C->pointer[i];
        }
    }
    return C;
}

// insert in leaf node
void B_plus_tree_insert_in_leaf(B_plus_tree_node *L, int K, Record *P) {
    // insert the record into L
    int i = 0;
    while(i < L->key_num && K > L->key[i]) {
        i++;
    }
    // move the key and pointer after i
    int j = L->key_num;
    while(j > i) {
        L->key[j] = L->key[j-1];
        L->pointer[j] = L->pointer[j-1];
        j--;
    }
    L->key[i] = K;
    L->pointer[i] = P;
    L->key_num++;
    L->pointer_num++;
}

// B plus tree insert in internal node 
// N is the internal node, P is the pointer to be inserted, K is the key to be inserted
void B_plus_tree_insert_in_internal_node(B_plus_tree_node *N, int K, B_plus_tree_node *P) {
    // insert the record into L
    int i = 0;
    while(i < N->key_num && K > N->key[i]) {
        i++;
    }
    // ?? i should not be 0
    assert(i != 0);

    // move the key and pointer after i
    int j = N->key_num;
    while(j > i) {
        N->key[j] = N->key[j-1];
        N->pointer[j+1] = N->pointer[j];
        j--;
    }
    N->key[i] = K;
    N->pointer[i+1] = P;  // P is inserted after K, that's the difference between insert in leaf node
    N->key_num++;
    N->pointer_num++;
}

// copy function
void B_plus_tree_copy_node(B_plus_tree_node *src, B_plus_tree_node *dst, int key_start, int key_stop_index, int pointer_start, int pointer_stop_index) {
    // copy key
    for(int i = key_start, j = 0; i < key_stop_index; i++, j++) {
        dst->key[j] = src->key[i];
        dst->key_num++;
    }
    // copy pointer
    for(int i = pointer_start, j = 0; i < pointer_stop_index; i++, j++) {
        dst->pointer[j] = src->pointer[i];
        dst->pointer_num++;
    }

}

// insert in parent node
void B_plus_tree_insert_in_parent(B_plus_tree *tree, B_plus_tree_node *Node,int Key, B_plus_tree_node *Node_prime) {
    if(Node == tree->root) {
        // create a new root node
        B_plus_tree_node *new_root = B_plus_tree_node_create_node(NUMBER_OF_CHILD-1, NUMBER_OF_CHILD);
        new_root->is_leaf = false;

        new_root->key[0] = Key;
        new_root->pointer[0] = Node;
        new_root->pointer[1] = Node_prime;

        new_root->key_num++;
        new_root->pointer_num += 2;

        Node->parent = new_root;
        Node_prime->parent = new_root;
        tree->root = new_root;

        return;
    }

    // P is the parent node of N
    B_plus_tree_node *Parent = Node->parent;
    if(Parent->pointer_num < NUMBER_OF_CHILD ) {
        // insert the record into L
        B_plus_tree_insert_in_internal_node(Parent, Key, Node_prime);

    }else {

        // T can hold NUMBER_OF_CHILD key, NUMBER_OF_CHILD+1 pointers
        B_plus_tree_node *Tamp = B_plus_tree_node_create_node(NUMBER_OF_CHILD, NUMBER_OF_CHILD+1);

        // copy P to T
        // use copy function
        B_plus_tree_copy_node(Parent, Tamp, 0, NUMBER_OF_CHILD-1, 0, NUMBER_OF_CHILD);

        // insert K, N' into T after N
        B_plus_tree_insert_in_internal_node(Tamp, Key, Node_prime);

        // erase the keys and pointers in P
        Parent->key_num = 0;
        Parent->pointer_num = 0;

        // create a new node P_prime
        B_plus_tree_node *Parent_prime = B_plus_tree_node_create_node(NUMBER_OF_CHILD-1, NUMBER_OF_CHILD);

        // Copy the first ceil(N+1/2) pointers of T to P, copy the first ceil(N/2) keys of T to P
        // So pointers that index in [0, (N+2)/2) is copied, keys that index in [0, (N+1)/2) is copied
        // use copy function
        B_plus_tree_copy_node(Tamp, Parent, 0, (NUMBER_OF_CHILD+1)/2, 0, (NUMBER_OF_CHILD+2)/2);

        // int K_prime = Tamp->key[(NUMBER_OF_CHILD+1)/2];
        int K_prime = Tamp->key[(NUMBER_OF_CHILD+1)/2];

        // copy the rest pointers of T to P_prime, copy the rest keys of T to P_prime
        // use copy function [[WARN]]
        B_plus_tree_copy_node(Tamp, Parent_prime, (NUMBER_OF_CHILD+1)/2 + 1, NUMBER_OF_CHILD, (NUMBER_OF_CHILD+2)/2, NUMBER_OF_CHILD+1);

        // set the parent pointer of P_prime
        Parent_prime->parent = Parent->parent;

        // insert K_prime, P_prime into the parent of P
        B_plus_tree_insert_in_parent(tree, Parent, K_prime, Parent_prime);
    }
}


// insert a record
void B_plus_tree_insert(B_plus_tree *tree, int Key, Record *Ptr_to_data) {
    B_plus_tree_node *Leaf;
    if(tree->root == NULL) {
        // create a new root node
        tree->root = B_plus_tree_node_create_node(NUMBER_OF_CHILD-1, NUMBER_OF_CHILD);
        tree->root->is_leaf = true;
        Leaf = tree->root;
    } else {
        // find the leaf node to insert
        Leaf = B_plus_tree_find_leaf(tree, Key);
    }

    if (Leaf->key_num < NUMBER_OF_CHILD-1) {
        // insert the record into L
        B_plus_tree_insert_in_leaf(Leaf, Key, Ptr_to_data);
    } else {
        // split the node
        // L' is the new leaf node
        B_plus_tree_node *Leaf_prime = B_plus_tree_node_create_node(NUMBER_OF_CHILD-1, NUMBER_OF_CHILD);
        Leaf_prime->is_leaf = true;

        // T can hold NUMBER_OF_CHILD key-value pairs
        B_plus_tree_node *Tamp = B_plus_tree_node_create_node(NUMBER_OF_CHILD, NUMBER_OF_CHILD);
        Tamp->is_leaf = true;

        // copy L to T
        // TODO wrap copy function

        B_plus_tree_copy_node(Leaf, Tamp, 0, NUMBER_OF_CHILD-1, 0, NUMBER_OF_CHILD-1);

        // insert the record into T
        B_plus_tree_insert_in_leaf(Tamp, Key, Ptr_to_data);

        // handle the special ptr (next pointer) of L and L_prime
        Leaf->pointer[NUMBER_OF_CHILD-1] = Leaf_prime;
        Leaf_prime->pointer[NUMBER_OF_CHILD-1] = Leaf->pointer[NUMBER_OF_CHILD-1];

        // erase the keys and pointers in L, except the next pointer
        Leaf->key_num = 0;
        Leaf->pointer_num = 0; // L->pointer[N-1] is not erased

        // Now T have N key-value pairs
        // Copy the first ceil(N/2) key-value pairs of T to L
        // So index in [0, (N+1)/2) is copied
        // use copy function
        B_plus_tree_copy_node(Tamp, Leaf, 0, (NUMBER_OF_CHILD+1)/2, 0, (NUMBER_OF_CHILD+1)/2);

        // Copy the rest key-value pairs of T to L_prime
        B_plus_tree_copy_node(Tamp, Leaf_prime, (NUMBER_OF_CHILD+1)/2, NUMBER_OF_CHILD, (NUMBER_OF_CHILD+1)/2, NUMBER_OF_CHILD);

        // set the parent pointer of L_prime
        Leaf_prime->parent = Leaf->parent;

        B_plus_tree_insert_in_parent(tree, Leaf, Leaf_prime->key[0], Leaf_prime);

    }
}

// free a B plus tree node
void B_plus_tree_free_node(B_plus_tree_node *node) {
    // free all pointer
    if(node->is_leaf == false) {
        for(int i = 0; i < node->pointer_num; i++) {
            B_plus_tree_free_node(node->pointer[i]);
        }
    }else {
        for(int i = 0; i < node->pointer_num; i++) {
            free(node->pointer[i]);
        }
    }

    free(node->key);
    free(node->pointer);
    free(node);
}

// free a B plus tree
void B_plus_tree_free_tree(B_plus_tree *tree) {
    // free all node
    B_plus_tree_node *node = tree->root;
    while(node != NULL) {
        B_plus_tree_node *next = node->pointer[NUMBER_OF_CHILD-1];
        B_plus_tree_free_node(node);
        node = next;
    }
}

// print a B plus tree node
void B_plus_tree_print_node(B_plus_tree_node *node) {
    printf("node: %p\n", node);
    printf("key_num: %d\n", node->key_num);
    printf("pointer_num: %d\n", node->pointer_num);
    printf("is_leaf: %d\n", node->is_leaf);
    printf("parent: %p\n", node->parent);
    printf("key: ");
    for(int i = 0; i < node->key_num; i++) {
        printf("%d ", node->key[i]);
    }
    printf("\n");
    printf("pointer: ");
    for(int i = 0; i < node->pointer_num; i++) {
        printf("%p ", node->pointer[i]);
    }

    if (node->is_leaf) {
        printf("\n");
        printf("value: ");
        for(int i = 0; i < node->pointer_num; i++) {
            Record *record = node->pointer[i];
            printf("%d ", record->value);
        }
    }
    printf("\n");
    printf("\n");
}

// print a B plus tree 
void B_plus_tree_print(B_plus_tree *tree) {
    // print all node
    B_plus_tree_node *node = tree->root;
    // make a stack of nodes
    B_plus_tree_node *stack[100];
    int stack_top = 0;
    stack[stack_top++] = node;

    while(stack_top > 0) {
        // pop a node from stack
        node = stack[--stack_top]; 
        B_plus_tree_print_node(node);
        
        // skip leaf node
        if (node->is_leaf == true) {
            continue;
        }

        // push all pointer to stack
        for(int i = 0; i < node->pointer_num; i++) {
            stack[stack_top++] = node->pointer[i];
        }
    }
}