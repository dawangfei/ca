#ifndef __DC_LOCK_H_
#define __DC_LOCK_H_


int dc_lock_init();

int dc_lock_create(int _sem_cnt);

int dc_lock_destroy(int _semid);

int dc_lock_clear(int _sem_id);

int dc_shared_lock(int semid, int n);

int dc_shared_unlock(int semid, int n);

int dc_exclusive_lock(int semid, int n);

int dc_exclusive_unlock(int semid, int n);

int dc_lock_full(int _sem_id);

int dc_unlock_full(int _sem_id);

int dc_lock_create_mutext_lock();
int dc_mutex_lock(int semid);
int dc_mutex_unlock(int semid);

#endif
