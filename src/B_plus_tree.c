#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "B_plus_tree.h"

void B_plus_tree_free_node_recursively(B_plus_tree_node *node);
void B_plus_tree_free_tree(B_plus_tree *tree);
void B_plus_tree_free_node_nonrecursively(B_plus_tree_node *node);
void B_plus_tree_print_node(B_plus_tree_node *node);

void B_plus_tree_insert_in_parent(B_plus_tree *tree, B_plus_tree_node *Node,int Key, B_plus_tree_node *Node_prime);
void B_plus_tree_insert_in_internal_node(B_plus_tree_node *N, int K, B_plus_tree_node *P);
void B_plus_tree_insert_in_leaf(B_plus_tree_node *L, int K, Record *P);
B_plus_tree_node* B_plus_tree_find_leaf(B_plus_tree *tree, int v);
B_plus_tree_node *B_plus_tree_node_create_node(int key_capacity, int pointer_capacity);
void B_plus_tree_copy_node(B_plus_tree_node *src, B_plus_tree_node *dst, int key_start, int key_stop_index, int pointer_start, int pointer_stop_index);
Record *B_plus_tree_find(B_plus_tree *tree, int v);

bool B_plus_tree_can_merge(B_plus_tree_node *node1, B_plus_tree_node *node2);
bool B_plus_tree_node_too_few_pointers(B_plus_tree_node *node);
void B_plus_tree_delete_entry(B_plus_tree *tree, B_plus_tree_node *Node, int Key, void* Pointer);
void B_plus_tree_delete(B_plus_tree *tree, int v);

// check node can merge
bool B_plus_tree_can_merge(B_plus_tree_node *node1, B_plus_tree_node *node2) {
    if(node1->is_leaf != node2->is_leaf) {
        return false;
    }
    if(node1->is_leaf) {
        return node1->key_num + node2->key_num <= NUMBER_OF_CHILD-1;
    } else {
        return node1->key_num + node2->key_num <= NUMBER_OF_CHILD-2;
    }
}

// check node has too few pointers
// For nonleaf nodes, this criterion means less than ⌈n∕2⌉ pointers; 
// for leaf nodes, it means less than ⌈(n − 1)∕2⌉ values. 
bool B_plus_tree_node_too_few_pointers(B_plus_tree_node *node) {
    if(node->is_leaf) {
        return node->pointer_num < (NUMBER_OF_CHILD)/2;
    } else {
        return node->pointer_num < (NUMBER_OF_CHILD+1)/2;
    }
}

// delete an entry from node
void B_plus_tree_delete_entry(B_plus_tree *tree, B_plus_tree_node *Node, int Key, void* Pointer) {
    // delete K,P from N
    int i = 0;
    while(i < Node->key_num && Node->key[i] != Key) {
        i++;
    }
    // assert(i < Node->key_num);
    // move the key and pointer after i
    int j = i;
    while(j < Node->key_num - 1) {
        Node->key[j] = Node->key[j+1];
        if(Node->is_leaf) { 
            Node->pointer[j] = Node->pointer[j+1];
        } else {
            assert(j+2<=Node->pointer_num);
            Node->pointer[j+1] = Node->pointer[j+2];
        }
        j++;
    }
    Node->key_num--;
    Node->pointer_num--;
    if(Node->is_leaf) {
        free((Record*)Pointer);
    }else {
        B_plus_tree_free_node_nonrecursively((B_plus_tree_node*)Pointer);
    }

    if(Node == tree->root && Node->is_leaf) {
        return;
    }
    if(Node == tree->root && Node->pointer_num == 1) {
        // the root node is empty, delete it
        tree->root = Node->pointer[0];
        B_plus_tree_free_node_nonrecursively(Node);
        return;
    }else if(B_plus_tree_node_too_few_pointers(Node)) {
        // get prev and next sibling node
        B_plus_tree_node *Parent = Node->parent;
        int i = 0;
        while(i < Parent->pointer_num && Parent->pointer[i] != Node) {
            i++;
        }
        int pointer_node_index = i;
        assert(pointer_node_index < Parent->pointer_num);
        B_plus_tree_node *Prev = NULL;
        B_plus_tree_node *Next = NULL;
        if (pointer_node_index > 0) {
            Prev = Parent->pointer[pointer_node_index - 1];
        }
        if (pointer_node_index < Parent->pointer_num - 1) {
            Next = Parent->pointer[pointer_node_index + 1];
        }
        enum {
            MERGE_PREV,
            MERGE_NEXT,
            REDISTRIBUTE_PREV,
            REDISTRIBUTE_NEXT,
        } action;


        B_plus_tree_node *Node_prime;
        int K_prime;
        // caculate the action
        if(Prev != NULL && B_plus_tree_can_merge(Prev, Node)) {
            action = MERGE_PREV;
            Node_prime = Prev;
            K_prime = Parent->key[pointer_node_index-1];
        } else if(Next != NULL && B_plus_tree_can_merge(Node, Next)) {
            action = MERGE_NEXT;
            Node_prime = Next;
            K_prime = Parent->key[pointer_node_index];
        } else if(Prev != NULL) {
            action = REDISTRIBUTE_PREV;
            Node_prime = Prev;
            K_prime = Parent->key[pointer_node_index-1];
        } else if(Next != NULL) {
            action = REDISTRIBUTE_NEXT;
            Node_prime = Next;
            K_prime = Parent->key[pointer_node_index];
        } else {
            assert(0);
        }

        // do the action
        if(action == MERGE_PREV || action == MERGE_NEXT) {
            if(action == MERGE_NEXT) {
                // swap node and next
                B_plus_tree_node *temp = Node;
                Node = Node_prime;
                Node_prime = temp;
            }

            // merge node_prime and node
            if(Node->is_leaf) {
                int i = 0;
                for(i = 0; i < Node->key_num; i++) {
                    Node_prime->key[Node_prime->key_num++] = Node->key[i];
                    Node_prime->pointer[Node_prime->pointer_num++] = Node->pointer[i];
                    assert(Node_prime->pointer_num <= NUMBER_OF_CHILD-1);
                }
                Node_prime->pointer[NUMBER_OF_CHILD-1] = Node->pointer[NUMBER_OF_CHILD-1];
            }else {
                int i = 0;
                // append K' to N' and append all pointers and keys in N to N'
                Node_prime->key[Node_prime->key_num++] = K_prime;
                for(i = 0; i < Node->key_num; i++) {
                    Node_prime->key[Node_prime->key_num++] = Node->key[i];
                    Node_prime->pointer[Node_prime->pointer_num++] = Node->pointer[i];
                    assert(Node_prime->pointer_num <= NUMBER_OF_CHILD-1);
                }
                Node_prime->pointer[Node_prime->pointer_num++] = Node->pointer[Node->pointer_num-1];
                assert(Node_prime->pointer_num <= NUMBER_OF_CHILD);
            }
            B_plus_tree_delete_entry(tree, Parent, K_prime, Node);
        }else if(action == REDISTRIBUTE_PREV){
            if(Node->is_leaf) {
                // let m be such that (N'.Pm, N'.Km) is the last pointer-key pair in N'
                int m = Node_prime->pointer_num - 1;
                int N_prime_Key_m = Node_prime->key[m];
                Record *N_prime_Pointer_m = Node_prime->pointer[m];

                // remove (N'.Pm, N'.Km) from N'
                Node_prime->pointer_num--;
                Node_prime->key_num--;

                // insert (N'.Pm, N'.Km) at the first pointer-key pair in N
                // move the key and pointer after i = 0
                int j = Node->key_num;
                while(j > 0) {
                    Node->key[j] = Node->key[j-1];
                    Node->pointer[j] = Node->pointer[j-1];
                    j--;
                }
                Node->key[0] = N_prime_Key_m;
                Node->pointer[0] = N_prime_Pointer_m;
                Node->pointer_num++;
                Node->key_num++;

                // replace K' in the parent of N' with N'.Km
                Parent->key[pointer_node_index-1] = N_prime_Key_m;

            }else { // Node is a nonleaf node
                // let m be such that N'.Pm is the last pointer in N'
                int m = Node_prime->pointer_num - 1;
                int N_prime_Key_m_1 = Node_prime->key[m-1];
                B_plus_tree_node *N_prime_Pointer_m = Node_prime->pointer[m];

                //remove (N'.Km−1, N'.Pm) from N′
                Node_prime->pointer_num--;
                Node_prime->key_num--;

                // insert (N'.Pm, K') as the first pointer and value in N
                int j = Node->key_num;
                while(j > 0) {
                    Node->key[j] = Node->key[j-1];
                    Node->pointer[j+1] = Node->pointer[j];
                    j--;
                }
                Node->pointer[1] = Node->pointer[0];

                Node->key[0] = K_prime;
                Node->pointer[0] = N_prime_Pointer_m;
                Node->pointer_num++;
                Node->key_num++;

                // replace K' in the parent of N' with N'.Km-1
                Parent->key[pointer_node_index-1] = N_prime_Key_m_1;
            }

        } else if(action == REDISTRIBUTE_NEXT) {
            if(Node->is_leaf) {
                // let m be such that (N'.Pm, N'.Km) is the first pointer-key pair in N'
                int m = 0;
                int N_prime_Key_m = Node_prime->key[m];
                Record *N_prime_Pointer_m = Node_prime->pointer[m];
                // remove (N'.Pm, N'.Km) from N'
                int j = 0;
                while(j < Node_prime->key_num - 1) {
                    Node_prime->key[j] = Node_prime->key[j+1];
                    Node_prime->pointer[j] = Node_prime->pointer[j+1];
                    j++;
                }
                Node_prime->pointer_num--;
                Node_prime->key_num--;
                // insert (N'.Pm, N'.Km) at the last pointer-key pair in N
                Node->key[Node->key_num++] = N_prime_Key_m;
                Node->pointer[Node->pointer_num++] = N_prime_Pointer_m;

                // replace K' in the parent of N' with N'.K0
                Parent->key[pointer_node_index] = Node_prime->key[0];
            } else { // Node is nonleaf node
                // let m be such that N'.Pm is the first pointer in N'
                int m = 0;
                int N_prime_Key_m = Node_prime->key[m];
                B_plus_tree_node *N_prime_Pointer_m = Node_prime->pointer[m];
                // remove (N'.Pm, N'.Km) from N'
                int j = 0;
                while(j < Node_prime->key_num - 1) {
                    Node_prime->key[j] = Node_prime->key[j+1];
                    Node_prime->pointer[j] = Node_prime->pointer[j+1];
                    j++;
                }
                Node_prime->pointer[j] = Node_prime->pointer[j+1];
                Node_prime->pointer_num--;
                Node_prime->key_num--;

                // insert (K', N'.Km) as the last pointer and value in N
                Node->key[Node->key_num++] = K_prime;
                Node->pointer[Node->pointer_num++] = N_prime_Pointer_m;

                // replace K' in the parent of N' with N'.Km
                Parent->key[pointer_node_index] = N_prime_Key_m;
            }
        }
    }
}

// delete a record
void B_plus_tree_delete(B_plus_tree *tree, int v) {
    // find the leaf node to delete
    B_plus_tree_node *leaf = B_plus_tree_find_leaf(tree, v);
    int i = 0;
    for(i = 0; i < leaf->key_num; i++) {
        if (leaf->key[i] == v) {
            break;
        }
    }

    if(i == leaf->key_num) {
        // the record is not found
        return;
    }
    B_plus_tree_delete_entry(tree, leaf, v, leaf->pointer[i]);
}


/*
find several records by key range [lb, ub]
return number of records found

result_set is a pointer to an array of Record pointers
result_set is allocated by this function, you should free it after use
TODO: use a better way to return result_set, such as a struct support dynamic array
*/
int B_plus_tree_find_range(B_plus_tree *tree, int lb, int ub, Record*** result_set) {
    if(lb > ub) {
        return 0;
    }

    // 初始化结果集的大小
    int result_capacity = 10;
    *result_set = malloc(sizeof(Record*) * result_capacity);
    int result_count = 0;

    B_plus_tree_node *C = B_plus_tree_find_leaf(tree, lb);

    if(C == NULL) {
        return 0;
    }

    int i = 0;
    for (i = 0; i < C->key_num && C->key[i] < lb; i++) {
        // do nothing
    }

    while(C != NULL) {
        for(; i < C->key_num && C->key[i] <= ub; i++) {

            if (result_count == result_capacity) {
                // 如果结果集已满，那么将其大小加倍
                result_capacity *= 2;
                *result_set = realloc(*result_set, sizeof(Record*) * result_capacity);
            }

            (*result_set)[result_count++] = C->pointer[i];
        }
        C = C->pointer[NUMBER_OF_CHILD-1];
        i = 0;
    }

    // 在查找结束后，你可以将结果集的大小调整为实际的记录数，以节省内存
    *result_set = realloc(*result_set, sizeof(Record*) * result_count);
    for(int i = 0; i < result_count; i++) {
        printf("result: %d %d\n", (*result_set)[i]->key, (*result_set)[i]->value);
    }

    return result_count;
}

// find a record by key
Record *B_plus_tree_find(B_plus_tree *tree, int v) {
    B_plus_tree_node *C = B_plus_tree_find_leaf(tree, v);

    if (C == NULL) {
        return NULL;
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
    B_plus_tree_node *P = NULL;

    if(C == NULL) {
        return NULL;
    }

    while(C->is_leaf == false) {
        // i is the smallest number such that v <= C->key[i]
        int i = 0;
        while(i < C->key_num && v > C->key[i]) {
            i++;
        }
        P = C;
        if (i == C->key_num) {
            // C = last non-null pointer in C
            C = C->pointer[C->pointer_num - 1];
        } else if (v == C->key[i]) {
            C = C->pointer[i+1];
        } else {
            // v < C->key[i]
            C = C->pointer[i];
        }
        C->parent = P;
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

        B_plus_tree_free_node_nonrecursively(Tamp);

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

        B_plus_tree_copy_node(Leaf, Tamp, 0, NUMBER_OF_CHILD-1, 0, NUMBER_OF_CHILD-1);

        // insert the record into T
        B_plus_tree_insert_in_leaf(Tamp, Key, Ptr_to_data);

        // handle the special ptr (next pointer) of L and L_prime
        Leaf_prime->pointer[NUMBER_OF_CHILD-1] = Leaf->pointer[NUMBER_OF_CHILD-1];
        Leaf->pointer[NUMBER_OF_CHILD-1] = Leaf_prime;

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

        B_plus_tree_free_node_nonrecursively(Tamp);



        B_plus_tree_insert_in_parent(tree, Leaf, Leaf_prime->key[0], Leaf_prime);

    }
}
// free a Temp node
void B_plus_tree_free_node_nonrecursively(B_plus_tree_node *node) {
    // free all pointer
    free(node->key);
    free(node->pointer);
    free(node);
}

// free a B plus tree node
void B_plus_tree_free_node_recursively(B_plus_tree_node *node) {
    // free all pointer
    if(node->is_leaf == false) {
        for(int i = 0; i < node->pointer_num; i++) {
            B_plus_tree_free_node_recursively(node->pointer[i]);
        }
    }else {
        for(int i = 0; i < node->pointer_num; i++) {
            // free the record 
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
    B_plus_tree_free_node_recursively(tree->root);
    free(tree);
}

// print a B plus tree node
void B_plus_tree_print_node(B_plus_tree_node *node) {
    printf("node: %p\n", (void*)node);
    printf("key_num: %d\n", node->key_num);
    printf("pointer_num: %d\n", node->pointer_num);
    printf("is_leaf: %d\n", node->is_leaf);
    printf("parent: %p\n", (void*)node->parent);
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
// works only for small tree
void B_plus_tree_print(B_plus_tree *tree) {
    // print a line
    printf("====================================\n");
    // print all node
    B_plus_tree_node *node = tree->root;
    // make a stack of nodes
    B_plus_tree_node *stack[100];
    int stack_top = 0;
    stack[stack_top++] = node;

    while(stack_top > 0) {
        assert(stack_top < 100);
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

// deconstruct a B plus tree
void B_plus_tree_destroy(B_plus_tree *tree) {
    B_plus_tree_free_tree(tree);
    pthread_rwlock_destroy(&tree->lock);
}

B_plus_tree* B_plus_tree_create(void) {
    B_plus_tree *tree = malloc(sizeof(B_plus_tree));
    tree->root = NULL;
    pthread_rwlock_init(&tree->lock, NULL);
    return tree;
}
