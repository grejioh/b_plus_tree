#ifndef EXECUTION_H
#define EXECUTION_H

#include "B_plus_tree.h"
#include "lock_manager.h"

typedef struct Database {
    B_plus_tree *tree;
    lock_manager_t *lock_manager;
    // key type int
    // value type int
} Database;

Database *db_create(void);

void db_destroy(Database *db);

void db_insert(Database *db, int key, int value);

void db_find(Database *db, int key);

void db_find_range(Database *db, int lb, int ub);

void db_delete(Database *db, int key);

#endif

