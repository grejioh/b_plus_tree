#include "B_plus_tree.h"
#include "lock_manager.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "execution.h"

// create a b plus tree database
Database *db_create(void) {
    Database *db = malloc(sizeof(Database));
    db->tree = B_plus_tree_create();
    db->lock_manager = lock_manager_create();
    return db;
}

// Destroy the database
void db_destroy(Database *db) {
    B_plus_tree_destroy(db->tree);
    free(db);
}

// insert key value pair into database
void db_insert(Database *db, int key, int value) {
    pthread_rwlock_wrlock(&db->tree->lock);
    Record *record = B_plus_tree_find(db->tree, key);
    if(record != NULL) {
        printf("key already exists\n");
        record->value = value;
        pthread_rwlock_unlock(&db->tree->lock);
        return;
    }

    record = malloc(sizeof(Record));

    record->key = key;
    record->value = value;

    B_plus_tree_insert(db->tree, key, record);
    pthread_rwlock_unlock(&db->tree->lock);
}

// find
void db_find(Database *db, int key) {
    pthread_rwlock_rdlock(&db->tree->lock);
    Record *record = B_plus_tree_find(db->tree, key);
    if(record == NULL) {
        printf("key not found\n");
        return;
    }
    printf("key: %d, value: %d\n", record->key, record->value);
    pthread_rwlock_unlock(&db->tree->lock);
}

// find range
void db_find_range(Database *db, int lb, int ub) {
    pthread_rwlock_rdlock(&db->tree->lock);
    Record** result_set;
    int result_number = B_plus_tree_find_range(db->tree, lb, ub, &result_set);
    for(int i = 0; i < result_number; i++) {
        printf("key: %d, value: %d\n", result_set[i]->key, result_set[i]->value);
    }
    pthread_rwlock_unlock(&db->tree->lock);
    return;
}

// delete
void db_delete(Database *db, int key) {
    pthread_rwlock_wrlock(&db->tree->lock);
    Record *record = B_plus_tree_find(db->tree, key);
    if(record == NULL) {
        printf("key not found\n");
        return;
    }
    B_plus_tree_delete(db->tree, key);
    pthread_rwlock_unlock(&db->tree->lock);
}

