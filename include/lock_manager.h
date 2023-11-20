#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include <pthread.h>


#define MAX_TRANSACTION 100
#define MAX_KEY 1000

enum lock_mode_t {
    LOCK_MODE_SHARED,
    LOCK_MODE_EXCLUSIVE
};

typedef struct lock_s {
    int key;
    int transaction_id;
    enum lock_mode_t lock_mode;
    pthread_rwlock_t rwlock;
} lock_t;

// TODO: use hash table to store locks
typedef struct lock_manager_s {
    lock_t *locks[MAX_KEY];
    pthread_mutex_t *mutex;
} lock_manager_t;

// return 0 if success
// return 1 if already locked by this transaction
// TODO: not used in this project, need to be implemented
int lock_manager_lock_shared(lock_manager_t* lock_manager, int transaction_id, int key);
// return 0 if success
// return 1 if already locked by this transaction
int lock_manager_lock_exclusive(lock_manager_t* lock_manager, int transaction_id, int key);

lock_manager_t* lock_manager_create(void);

void lock_manager_unlock(lock_manager_t* lock_manager, int transaction_id);
#endif
