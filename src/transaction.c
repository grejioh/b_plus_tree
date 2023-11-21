#include "execution.h"
#include "transaction.h"
#include "lock_manager.h"
#include <stdlib.h>
#include <string.h>


int transaction_id_counter;

transaction_t *transaction_create(int priority) {
    transaction_t *transaction = malloc(sizeof(transaction_t));
    transaction->priority = priority;
    transaction->id = transaction_id_counter++;
    // transaction->keys_to_unlock = malloc(sizeof(int) * 100);
    // transaction->keys_to_unlock_num = 0;
    return transaction;
}

int transaction_handle(Database *db, transaction_t *transaction, char *buffer, char *response) {
    if(strcmp(buffer, "commit;") == 0) {
        // commit transaction
        lock_manager_unlock(db->lock_manager, transaction->id);
        return 0;
    } else if(strcmp(buffer, "abort;") == 0) {
        // abort transaction
        lock_manager_unlock(db->lock_manager, transaction->id);
        return -1;
    }
    // parse buffer
    char *command = strtok(buffer, "(");
    if (strcmp(command, "insert") == 0) {
        int key = atoi(strtok(NULL, ","));
        int value = atoi(strtok(NULL, ")"));

        lock_manager_lock_exclusive(db->lock_manager, transaction->id, key);
        db_insert(db, key, value);
    } else if (strcmp(command, "find") == 0) {
        int key = atoi(strtok(NULL, ")"));
        lock_manager_lock_exclusive(db->lock_manager, transaction->id, key);
        db_find(db, key, response);
    } else if (strcmp(command, "delete") == 0) {
        int key = atoi(strtok(NULL, ")"));
        lock_manager_lock_exclusive(db->lock_manager, transaction->id, key);
        db_delete(db, key);
    } else if (strcmp(command, "findrange") == 0) {
        int lb = atoi(strtok(NULL, ","));
        int ub = atoi(strtok(NULL, ")"));
        // TODO: need lock
        db_find_range(db, lb, ub, response);
    } else {
        // Unknown command
    }

    return 1;
}



