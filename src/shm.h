#ifndef __DC_SHM_H_
#define __DC_SHM_H_


#define SHM_MAX_SEGS     100
#define SHM_CREATE       0x1

#define DC_FLOOR_ALIGN(p, K)    ((long)(p) - (((long)(p)%(K) == 0) ? 0 : (long)(p)%(K)))
#define DC_CEIL_ALIGN(p, K)     ((long)(p) + (((long)(p)%(K) == 0) ? 0 : (K-(long)(p)%(K))))

#define DC_SHM_REGION_DEFAULT   0
#define DC_SHM_REGION_A         1
#define DC_SHM_REGION_B         2

#define DC_SHM_IS_READY(v)      ((v) == 1)
#define DC_SHM_IS_DISCARD(v)    ((v) == 2)

/*
 *  Function:  dc_shm_init
 *      To create a shared memory,
 *          or attach a shm
 *      Warn: a lock must be hold if to create!
 *
 *  Parameters:
 *      shm_key   - ipc key
 *      shm_size  - the size of full shared memory, without shm_header_t
 *      shm_addr  - the address of shared memory,
 *                  both input and output
 *      flag      - CREATE or 0
 *
 *  Return Value:
 *      SUCCESS - shmid
 *      FAILURE - failure
 */
int dc_shm_init(long shm_key, long shm_size, void **shm_addr, long flag, int *errcode);


/*
 *  Function:  
 *      To detach and delete a shared memory,
 *      the shared-memory is already created!
 *
 *  Parameters:
 *      addr    - address of shared memory
 *      idx     - the index of segments
 *
 *  Return Value:
 *      SUCCESS - the index of segments
 *      FAILURE - failure
 */
int dc_shm_term(int shm_id, void *shm_addr);



/*
 *  Function:  
 *      get the address of block-A
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
void *dc_shm_get_block_A_addr(void *_shm_addr);


/*
 *  Function:  
 *      get the address of block-B
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
void *dc_shm_get_block_B_addr(void *_shm_addr);


/*
 *  Function:  
 *      get block size
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - N
 *      FAILURE - -1
 */
long dc_shm_get_block_size(void *_shm_addr);

long dc_shm_get_gate(void *_shm_addr);
void dc_shm_switch_gate(void *_shm_addr);

void *dc_shm_get_addr();
void dc_shm_set_addr();

long dc_shm_calc_size(int _block_size);

int  dc_shm_set_ready(void *_shm_addr);
int  dc_shm_get_ready(void *_shm_addr);
int  dc_shm_set_discard(void *_shm_addr);

int  dc_shm_set_semid(void *_shm_addr, int _sem_id);
int  dc_shm_get_semid(void *_shm_addr);

void dc_shm_debug(void *_shm_addr);

#endif
