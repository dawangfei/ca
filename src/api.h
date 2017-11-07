#ifndef __DC_API_H_
#define __DC_API_H_

/****************************/

int dc_cache_api_admin_init(long _shm_key);
int dc_cache_api_admin_close();

/****************************/

int dc_cache_api_open(long _shm_key);
int dc_cache_api_close();
int dc_cache_api_is_already_open();
int dc_cache_api_is_aggregation_count(int *_num);

int dc_cache_api_select(char *_sql);
int dc_cache_api_get_data(int _idx, void *_buffer, int _buffer_size);

int dc_cache_api_cursor(char *_sql);
void *dc_cache_api_fetch();

int dc_cache_api_reload(char *_table_name);

int dc_cache_api_reload_cluster(char *_table_name);
int dc_cache_api_enable_table_cluster(char *_table_name);
int dc_cache_api_disable_table_cluster(char *_table_name);

int dc_cache_api_cluster_set_life(int _life);
int dc_cache_api_cluster_get_life();
/****************************/
int dc_cache_api_dump_table(char *_table_name);
int dc_cache_api_dump_block();
int dc_cache_api_dump_curr_row();
int dc_cache_api_list_tables();
int dc_cache_api_desc_table(char *_table_name);
int dc_cache_api_shm_info();
int dc_cache_api_set_mode(int _mode);
/****************************/



#define DC_CODE_RELOAD      "1001"
#define DC_CODE_ENABLE      "2001"
#define DC_CODE_DISABLE     "2002"

typedef struct _dc_cache_api_svc_t 
{
    char code[4+1];
    char table[32+1];
    char resp[2+1];
    char timestamp0[64+1];  /* local-time */
    char timestamp1[64+1];  /* remote-begin */
    char timestamp2[64+1];  /* remote-end */
    char message[128+1];
} dc_cache_api_svc_t;




/*
 *  Function: int CAOpen(long shm_key);
 *      attach to the shared memory.
 *      should be called one and only one time.
 *
 *  Parameters:
 *      shm_key - long, shared memory key
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAOpen          dc_cache_api_open


/*
 *  Function: int CAClose()
 *      detach from the shared memory.
 *  
 *  Parameters:
 *      No      - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAClose         dc_cache_api_close


/*
 *  Function: int CASelect(char *sql)
 *      Query by execute the sql, better used for 1 row result set case.
 *      the query proceeding use shared lock.
 *
 *  Parameters:
 *      sql     -  "select .. from ... where ..."
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
#define CASelect        dc_cache_api_select


/*
 *  Function: CAGetData(int idx, void *buffer, int buffer_size)
 *      get column data by select columns index
 *
 *  Parameters:
 *      idx         - index of columns from select region
 *      buffer      - output buffer
 *      buffer_size - output buffer size
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAGetData       dc_cache_api_get_data


/*
 *  Function: int CACursor(char *sql)
 *      Query by execute the SQL, for more than 1 row result set case
 *      the query proceeding use shared lock.
 *
 *      should with: dc_cache_api_fetch()
 *
 *  Parameters:
 *      sql     -  "select .. from ... where ..."
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
#define CACursor        dc_cache_api_cursor


/*
 *  Function: void *CAFetch()
 *      fetch next row, for multiple rows result set
 *
 *  Parameters:
 *      No      - 
 *
 *  Return Value:
 *      SUCCESS - not NULL
 *      FAILURE - NULL, reach the end
 */
#define CAFetch         dc_cache_api_fetch


/*
 *  Function: int CAReload(char *table_name)
 *      Reload a table from DB to SHM
 *      Use mutex lock and shared lock
 *      XXX: MUST ensure has connected to DB.
 *
 *  Parameters:
 *      table_name - "t_credit_bin"
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAReload        dc_cache_api_reload


/*
 *  Function: int CAReloadCluster(char *table_name)
 *      Reload table in cluster mode
 *
 *  Parameters:
 *      table_name - "t_credit_bin"
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAReloadCluster dc_cache_api_reload_cluster


/*
 *  Function: int CAEnableTableCluster(char *table_name)
 *      enable table in cluster mode
 *
 *  Parameters:
 *      table_name - "t_credit_bin"
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAEnableTableCluster dc_cache_api_enable_table_cluster


/*
 *  Function: int CADisableTableCluster(char *table_name)
 *      disable table in cluster mode
 *
 *  Parameters:
 *      table_name - "t_credit_bin"
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CADisableTableCluster dc_cache_api_disable_table_cluster


/************************************************
 ***** Supervisor *******************************
 ************************************************/

/*
 *  Function: int CAAdminOpen(long shm_key)
 *      1. create IPC: shared memory and semaphore.
 *      2. load all tables from DB to SHM.
 *
 *      Use mutex lock and shared lock
 *      WARN: invoke only one time!
 *      WARN: this is supervisor user, who will create the IPC.
 *
 *  Parameters:
 *      shm_key - the KEY of shared memory to create
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAAdminOpen     dc_cache_api_admin_init


/*
 *  Function: int CAAdminClose()
 *      detach from IPC and destroy it
 *
 *  Parameters:
 *      No      - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAAdminClose    dc_cache_api_admin_close



/*
 *  Function:  int dc_aux_init();
 *      only called one time!
 *
 *  Parameters:
 *      No      - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAAuxINIT       dc_aux_init


/*
 *  Function: int CAAuxAddTable(char *table_name, int max_rows)
 *      mark the table to be cached
 *
 *  Parameters:
 *      table_name - table name
 *      max_rows   - the estimated max row number of table
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
#define CAAuxAddTable   dc_aux_seg_add


/*****************************************************************************/

/* int CAMallocArea(void); */
int dc_malloc_area(void);
#define CAMallocArea    dc_malloc_area

/* int CADBSelect(int _area , char *_statement, char _op_flag, long _offset); */
int dc_db_select(int _area , char *_statement, char _op_flag, long _offset);
#define CADBSelect      dc_db_select


/* int CADBGetStru(int _data_id , void *_data , int _len); */
int dc_db_get_stru(int _data_id , void *_data , int _len);
#define CADBGetStru     dc_db_get_stru


/* int CADBGetData(int _data_id , void *_data , int _len); */
int dc_db_get_data(int _data_id , void *_data , int _len);
#define CADBGetData     dc_db_get_data


/* int CAFreeArea(void); */
int dc_free_area(void);
#define CAFreeArea      dc_free_area


/* int CADBPutData(int _data_id , int _data_type, void *_data); */
int dc_db_put_data(int _data_id , int _data_type, void *_data);
#define CADBPutData     dc_db_put_data

#endif
