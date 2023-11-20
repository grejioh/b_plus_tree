#include "lock_manager.h"
#include <stdlib.h>
#include <string.h>

// return 0 if success
// return 1 if already locked by this transaction
// TODO: not used in this project, need to be implemented
int lock_manager_lock_shared(lock_manager_t* lock_manager, int transaction_id, int key) {
    // Find the lock
    lock_t *lock = lock_manager->locks[key];

    // If the lock does not exist, create it
    if (lock == NULL) {
        lock = malloc(sizeof(lock_t));
        lock_manager->locks[key] = lock;
        lock->key = key;
        lock->lock_mode = LOCK_MODE_SHARED;
        
        pthread_rwlock_init(&lock->rwlock, NULL);
    }

    if(lock->transaction_id == transaction_id) {
        return 1;
    }


    // Lock the mutex
    pthread_rwlock_rdlock(&lock->rwlock);

    // Set the lock mode to shared
    lock->lock_mode = LOCK_MODE_SHARED;
    // Set the transaction id
    lock->transaction_id = transaction_id;

    return 0;
}

// return 0 if success
// return 1 if already locked by this transaction
int lock_manager_lock_exclusive(lock_manager_t* lock_manager, int transaction_id, int key) {
    // lock the mutex
    // pthread_mutex_lock(lock_manager->mutex);

    // Find the lock
    lock_t* lock = lock_manager->locks[key];

    // If the lock does not exist, create it
    if (lock == NULL) {
        lock = malloc(sizeof(lock_t));
        lock_manager->locks[key] = lock;
        lock->key = key;
        lock->lock_mode = LOCK_MODE_EXCLUSIVE;
        lock->transaction_id = -1;

        pthread_rwlock_init(&lock->rwlock, NULL);
    }

    if(lock->transaction_id == transaction_id) {
        return 1;
    }


    // Lock the mutex
    pthread_rwlock_wrlock(&lock->rwlock);

    // Set the lock mode to exclusive
    lock->lock_mode = LOCK_MODE_EXCLUSIVE;

    lock->transaction_id = transaction_id;

    // pthread_mutex_unlock(lock_manager->mutex);
    return 0;
}

lock_manager_t* lock_manager_create(void) {
    lock_manager_t* lock_manager = malloc(sizeof(lock_manager_t));
    memset(lock_manager->locks, 0, sizeof(lock_manager->locks));
    lock_manager->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock_manager->mutex, NULL);
    return lock_manager;
}

void lock_manager_unlock(lock_manager_t* lock_manager, int transaction_id) {
    for(int i = 0; i < MAX_KEY; i++) {
        lock_t* lock = lock_manager->locks[i];
        if(lock != NULL && lock->transaction_id == transaction_id) {
            pthread_rwlock_unlock(&lock->rwlock);
            lock->transaction_id = -1;
        }
    }
}
