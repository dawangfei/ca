#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>

#include "type.h"
#include "util.h"
#include "log.h"

#include "lock.h"

#define     DC_MAX_SEMS        100
#define     DC_MAX_RESOURCE    2000

/* single shared lock */
static struct sembuf Ps;
static struct sembuf Vs;

/* single exclusive lock */
static struct sembuf Px;
static struct sembuf Vx;

/* mutext lock */
static struct sembuf Pm;
static struct sembuf Vm;

static ushort sem_array[DC_MAX_SEMS];

static int g_sem_num = 0;

/*
 *  Function:
 *  Parameters:
 *      
 *  Return Value:
 *      SUCCESS - semid
 *      FAILURE - failure
 */
int dc_lock_init()
{
    /* SEM_UNDO: for unspecified quit! */

    /* shared */
    Ps.sem_num = 0;
    Ps.sem_op  = -1;
    Ps.sem_flg = SEM_UNDO;

    Vs.sem_num = 0;
    Vs.sem_op  = 1;
    Vs.sem_flg = SEM_UNDO;

    /* exclusive */
    Px.sem_num = 0;
    Px.sem_op  = 0 - DC_MAX_RESOURCE;
    Px.sem_flg = SEM_UNDO;

    Vx.sem_num = 0;
    Vx.sem_op  = DC_MAX_RESOURCE;
    Vx.sem_flg = SEM_UNDO;


    /* mutex */
    Pm.sem_num = 0;
    Pm.sem_op  = -1;
    Pm.sem_flg = SEM_UNDO;

    Vm.sem_num = 0;
    Vm.sem_op  = 1;
    Vm.sem_flg = SEM_UNDO;

    return 0;
}


/*
 *  Function:
 *  Parameters:
 *      n  : (int)
 *  Return Value:
 *      SUCCESS - semid
 *      FAILURE - failure
 */
int dc_lock_create(int _sem_cnt)
{
    int   i;
    int   semid;

    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;

    /* olog("sem_count is <%d>", _sem_cnt); */

    /* semaphore not exist, create it */
    if ((semid = semget(IPC_PRIVATE, _sem_cnt, IPC_CREAT | 0644)) == -1) 
    {
        elog("error: semget2 <%d><%s>", errno, strerror(errno));
        return FAILURE;
    }

    /* set value for semaphore */
    assert(_sem_cnt <= DC_MAX_SEMS);
    memset(sem_array, 0, sizeof(sem_array));
    for (i = 0; i < _sem_cnt; i++)
    {
        sem_array[i] = DC_MAX_RESOURCE;
    }
    arg.array = &sem_array[0];

    if (semctl(semid, 0, SETALL, arg) == -1) 
    {
        elog("error: semctl SETALL <%d><%s>", errno, strerror(errno));
        return FAILURE;
    }

    g_sem_num = _sem_cnt;

    return semid;
}


int dc_lock_destroy(int _semid)
{
    if (semctl(_semid, IPC_RMID, 0) < 0)
    {
        elog("error: semctl IPC_RMID");
        return -1;
    }
    return 0;
}


int dc_lock_clear(int _sem_id)
{
    if (semctl(_sem_id, IPC_RMID, 0) < 0)
    {
        elog("error: semctl RPC_RMID: %d", _sem_id);
        return -1;
    }

    return 0;
}


/*
 *  Function:
 *  Parameters:
 * 
 *  Return Value:
 *      SUCCESS - semid
 *      FAILURE - failure
 */
int dc_lock_create_mutext_lock()
{
    int   semid;
    ushort sem_a[1];

    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;

    /* semaphore not exist, create it */
    if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644)) == -1) 
    {
        elog("error: semget2 <%d><%s>", errno, strerror(errno));
        return FAILURE;
    }

    /* set value for semaphore */
    memset(sem_a, 0, sizeof(sem_a));
    sem_a[0] = 1;
    arg.array = &sem_a[0];

    if (semctl(semid, 0, SETALL, arg) == -1) 
    {
        elog("error: semctl SETALL <%d><%s>", errno, strerror(errno));
        return FAILURE;
    }

    return semid;
}


/*
 *  Function:
 *      To lock a specified resource
 *  Parameters:
 *      n - the Nth semaphore
 *  Return Value:
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_shared_lock(int semid, int n)
{
    int  rv = 0;

    Ps.sem_num = n;

    do {
        rv = semop(semid, &Ps, 1);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}


/*
 *  Function:  dc_shared_unlock
 *      To unlock the specified resource
 *  Parameters:
 *      n - the Nth semaphore
 *  Return Value:
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_shared_unlock(int semid, int n)
{
    int  rv = 0;

    Vs.sem_num = n;

    do {
        rv = semop(semid, &Vs, 1);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}



/*
 *  Function: 
 *      To lock a specified resource
 *  Parameters:
 *      n - the Nth semaphore
 *  Return Value:
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_exclusive_lock(int semid, int n)
{
    int  rv = 0;

    Px.sem_num = n;

    do {
        rv = semop(semid, &Px, 1);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}


/*
 *  Function:
 *      To unlock the specified resource
 *  Parameters:
 *      n - the Nth semaphore
 *  Return Value:
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_exclusive_unlock(int semid, int n)
{
    int  rv = 0;

    Vx.sem_num = n;

    do {
        rv = semop(semid, &Vx, 1);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}



int dc_lock_info(int _sem_id)
{
    int  rv = 0;
    struct semid_ds tat;

    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;

    memset(&tat, 0, sizeof(tat));

    arg.buf = &tat;
    rv = semctl(_sem_id, 0, IPC_STAT, arg);
    if (rv < 0)
    {
        elog("error: semctl IPC_STAT failure: %d:%d: [%d, %s]!",
                _sem_id, rv, errno, strerror(errno));
        return -1;
    }

    g_sem_num = tat.sem_nsems;
    olog("[%d] sub in sem[%d]", g_sem_num, _sem_id);

    return 0;
}


int dc_lock_full(int _sem_id)
{
    int  i  = 0;
    int  rv = 0;

    struct sembuf sops[DC_MAX_SEMS];

    if (g_sem_num <= 0 || g_sem_num > DC_MAX_SEMS)
    {
        elog("error: invalid number: %d", g_sem_num);
        return -1;
    }

    memset(&sops, 0, sizeof(sops));

    for (i = 0; i < g_sem_num; i++)
    {
        sops[i].sem_num = i;
        sops[i].sem_op  = 0-DC_MAX_RESOURCE;
        sops[i].sem_flg = SEM_UNDO;
    }

    do {
        rv = semop(_sem_id, sops, g_sem_num);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return 0;
}


int dc_unlock_full(int _sem_id)
{
    int  i  = 0;
    int  rv = 0;

    struct sembuf sops[DC_MAX_SEMS];

    if (g_sem_num <= 0 || g_sem_num > DC_MAX_SEMS)
    {
        elog("error: invalid number: %d", g_sem_num);
        return -1;
    }

    memset(&sops, 0, sizeof(sops));

    for (i = 0; i < g_sem_num; i++)
    {
        sops[i].sem_num = i;
        sops[i].sem_op  = DC_MAX_RESOURCE;
        sops[i].sem_flg = SEM_UNDO;
    }

    do {
        rv = semop(_sem_id, sops, g_sem_num);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return 0;
}


/*
 *  Function:
 *      mutex lock
 *  Parameters:
 *      n - the Nth semaphore
 *  Return Value:
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_mutex_lock(int semid)
{
    int  rv = 0;

    Pm.sem_num = 0;

    do {
        rv = semop(semid, &Pm, 1);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}


/*
 *  Function:
 *      mutex unlock
 *  Parameters:
 *      n - the Nth semaphore
 *  Return Value:
 *      SUCCESS - success
 *      FAILURE - failure
 */
int dc_mutex_unlock(int semid)
{
    int  rv = 0;

    Vm.sem_num = 0;

    do {
        rv = semop(semid, &Vm, 1);
    } while (rv < 0 && errno == EINTR);

    if (rv < 0)
    {
        elog("error: semop: <%d>,<%s>", errno, strerror(errno));
        return FAILURE;
    }

    return SUCCESS;
}

#if RUN_LOCK
static int hash(int _input)
{
    int i = _input;

    i += ~(i << 9); 
    i ^=  ((i >> 14) | (i << 18)); /* >>> */
    i +=  (i << 4); 
    i ^=  ((i >> 10) | (i << 22)); /* >>> */

    if (i < 0)
    {
        i = 0 - i;
    }

    return i;
}


int main(int argc, char *argv[])
{
    int rv = 0;
    int  id  = 0;
    int  i   = 0;
    int  idx = 0;
    int  loop = 0;
    int  max = 100;
    int  seq = 0;
    long begin = 0;


    if (argc == 2)
    {
        if (memcmp(argv[1], "M", 1) == 0)
        {
            id = dc_lock_create_mutext_lock();
            if (id < 0)
            {
                elog("error: dc_lock_create_mutext_lock: %d", id);
                goto _end;
            }
        }
        else
        {
            id = dc_lock_create(max);
            if (id < 0)
            {
                elog("error: dc_lock_create: %d", id);
                goto _end;
            }
        }

        olog("semid: %d", id);

        dc_lock_init();
    }
    else if (argc == 5)
    {
        id = atoi(argv[3]);
        olog("semid: %d", id);

        seq = atoi(argv[4]);
        olog("seq: %d", seq);

        dc_lock_init();

        if (dc_lock_info(id))
        {
            elog("error: dc_lock_info %d", id);
            return -1;
        }

        if (memcmp(argv[1], "S", 1) == 0)
        {
            olog("shared");
            if (memcmp(argv[2], "L", 1) == 0)
            {
                olog("try locking: %d", seq);
                begin = get_time();
                rv = dc_shared_lock(id, seq);
                if (rv)
                {
                    elog("error: dc_shared_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);
                pause();
            }
            else if (memcmp(argv[2], "U", 1) == 0)
            {
                olog("try locking: %d", seq);
                begin = get_time();
                rv = dc_shared_lock(id, seq);
                if (rv)
                {
                    elog("error: dc_shared_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);

                sleep(10);

                olog("try unlocking: %d", seq);
                begin = get_time();
                rv = dc_shared_unlock(id, seq);
                if (rv)
                {
                    elog("error: dc_shared_unlock failure: %d", rv);
                    goto _end;
                }
                olog("unlock succeeds: %ld", get_time()-begin);
            }
            else if (memcmp(argv[2], "B", 1) == 0)
            {
                idx = hash(getpid()) % max;
                loop = 1000000;
                olog("benchmark mode: [%d], vector: [%d]", loop, idx);
                begin = get_time();
                for (i = 0; i < loop; i++)
                {
                    rv = dc_shared_lock(id, idx);
                    if (rv)
                    {
                        elog("error: dc_shared_lock failure: %d", rv);
                        goto _end;
                    }

                    rv = dc_shared_unlock(id, idx);
                    if (rv)
                    {
                        elog("error: dc_shared_unlock failure: %d", rv);
                        goto _end;
                    }
                }
                olog("has loop [%d] times, cost(%ld)us", loop, get_time()-begin);
            }
            else
            {
                elog("error: S invalid command");
            }
        }
        else if (memcmp(argv[1], "X", 1) == 0)
        {
            olog("exclusive");
            if (memcmp(argv[2], "L", 1) == 0)
            {
                olog("try locking: %d", seq);
                begin = get_time();
                rv = dc_exclusive_lock(id, seq);
                if (rv)
                {
                    elog("error: dc_exclusive_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);
                pause();
            }
            else if (memcmp(argv[2], "U", 1) == 0)
            {
                olog("try locking: %d", seq);
                begin = get_time();
                rv = dc_exclusive_lock(id, seq);
                if (rv)
                {
                    elog("error: dc_exclusive_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);

                sleep(10);

                olog("try unlocking: %d", seq);
                begin = get_time();
                rv = dc_exclusive_unlock(id, seq);
                if (rv)
                {
                    elog("error: dc_exclusive_unlock failure: %d", rv);
                    goto _end;
                }
                olog("unlock succeeds: %ld", get_time()-begin);
            }
            else
            {
                elog("error: X invalid command");
            }
        }
        else if (memcmp(argv[1], "F", 1) == 0)
        {
            olog("shared");
            if (memcmp(argv[2], "L", 1) == 0)
            {
                olog("try locking: %d", seq);
                begin = get_time();
                rv = dc_lock_full(id);
                if (rv)
                {
                    elog("error: dc_shared_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);
                pause();
            }
            else if (memcmp(argv[2], "U", 1) == 0)
            {
                olog("try locking: %d", seq);
                begin = get_time();
                rv = dc_lock_full(id);
                if (rv)
                {
                    elog("error: dc_shared_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);

                sleep(10);

                olog("try unlocking: %d", seq);
                begin = get_time();
                rv = dc_unlock_full(id);
                if (rv)
                {
                    elog("error: dc_shared_unlock failure: %d", rv);
                    goto _end;
                }
                olog("unlock succeeds: %ld", get_time()-begin);
            }
            else if (memcmp(argv[2], "B", 1) == 0)
            {
                idx = hash(getpid()) % max;
                loop = 1000000;
                olog("benchmark mode: [%d], vector: [%d]", loop, idx);
                begin = get_time();
                for (i = 0; i < loop; i++)
                {
                    rv = dc_shared_lock(id, idx);
                    if (rv)
                    {
                        elog("error: dc_shared_lock failure: %d", rv);
                        goto _end;
                    }

                    rv = dc_shared_unlock(id, idx);
                    if (rv)
                    {
                        elog("error: dc_shared_unlock failure: %d", rv);
                        goto _end;
                    }
                }
                olog("has loop [%d] times, cost(%ld)us", loop, get_time()-begin);
            }
            else
            {
                elog("error: S invalid command");
            }
        }
        else if (memcmp(argv[1], "M", 1) == 0)
        {
            olog("mutex");
            if (memcmp(argv[2], "L", 1) == 0)
            {
                olog("try mutex locking: %d", seq);
                begin = get_time();
                rv = dc_mutex_lock(id);
                if (rv)
                {
                    elog("error: dc_mutex_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);
                pause();
            }
            else if (memcmp(argv[2], "U", 1) == 0)
            {
                olog("try mutex locking: %d", seq);
                begin = get_time();
                rv = dc_mutex_lock(id);
                if (rv)
                {
                    elog("error: dc_mutex_lock failure: %d", rv);
                    goto _end;
                }
                olog("lock succeeds: %ld", get_time()-begin);

                sleep(10);

                olog("try mutex unlocking: %d", seq);
                begin = get_time();
                rv = dc_mutex_unlock(id);
                if (rv)
                {
                    elog("error: dc_mutex_unlock failure: %d", rv);
                    goto _end;
                }
                olog("unlock succeeds: %ld", get_time()-begin);
            }
            else if (memcmp(argv[2], "B", 1) == 0)
            {
                idx = hash(getpid()) % max;
                loop = 1000000;
                olog("benchmark mode: [%d], vector: [%d]", loop, idx);
                begin = get_time();
                for (i = 0; i < loop; i++)
                {
                    rv = dc_shared_lock(id, idx);
                    if (rv)
                    {
                        elog("error: dc_shared_lock failure: %d", rv);
                        goto _end;
                    }

                    rv = dc_shared_unlock(id, idx);
                    if (rv)
                    {
                        elog("error: dc_shared_unlock failure: %d", rv);
                        goto _end;
                    }
                }
                olog("has loop [%d] times, cost(%ld)us", loop, get_time()-begin);
            }
            else
            {
                elog("error: S invalid command");
            }
        }
        else
        {
            elog("error: invalid lock: [%s]", argv[1]);
        }

    }
    else
    {
        elog("usage:");
        elog("%s N -- create a new semphore.", argv[0]);
        elog("%s S L semid seq", argv[0]);
        elog("%s X L semid seq", argv[0]);
        elog("%s S U semid seq", argv[0]);
        elog("%s X U semid seq", argv[0]);
        elog("----------------------------");
        elog("%s S L 28147762 0", argv[0]);
        goto _end;
    }


_end:

    return rv;
}
#endif

/* lock.c */
