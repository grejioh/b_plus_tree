#ifndef TRANSACTION_H
#define TRANSACTION_H
#include "execution.h"

typedef struct transaction_c {
    int id;
    int priority;
    // int *keys_to_unlock;
    // int keys_to_unlock_num;
} transaction_t;

transaction_t *transaction_create(int priority);

int transaction_handle(Database *db, transaction_t *transaction, char *buffer);


#endif
