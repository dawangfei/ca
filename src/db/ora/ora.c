#include <stdio.h>
#include <string.h>

#include <pladb.h>
#include <sysloc.h>
#include <arcbcl.h>
#include <__arcde.h>

typedef struct {
    char       pan[29];
    char       txn_type[7];
    char       txn_date[9];
    long       fail_continue_num;
    long       fail_total_num;
    long       succ_total_num;
    char       inst_date_time[15];
    char       last_fail_time[15];
    char       last_succ_time[15];
} tbl_log_counter_def;


extern int  db_get_ncols( int _area_id );
extern long db_get_rowsize();
extern long db_get_column(int _idx, char *_name, int _name_max, int *_type, int *_value_max);





void trc_db_data_rec( int _oper_type, int _data_id, int _data_type, char *_data_desc, char *_data, int _len )
{
}

int bclerreg( int appcode, char * file, long line, ... )
{
    return 0;
}

void trc_db_rec( char *_sql_str, int _sql_code )
{
}

void bcl_log( char * err_log_file, char *file_name, int line_no, char * formt, ... )
{
}

char * bcl_ltrim( char *str )
{
    register char *pstart, *ptr;

    if ( str == NULL )
        return NULL;

    pstart = ptr = str;

    while ( *( ( unsigned char * )ptr ) == 0x20 || *ptr == '\t' || \
            *ptr == '\r' || *ptr == '\n' )
        ptr ++;

    while ( *ptr != 0x0 )
        *str ++ = *ptr ++;

    *str = 0x0;

    return pstart;
}

char * bcl_rtrim( char *str )
{
    register char *ptr;

    if ( str == NULL )
        return NULL;

    ptr = str + strlen( str ) - 1;

    while ( ptr >= str && ( *( ( unsigned char * )ptr ) == 0x20 || \
                *ptr == '\t' ||  *ptr == '\r' || *ptr == '\n' ))
        *ptr -- = 0x0;

    return str;
}


int get_index_def(char *_index_name)
{
    int ret  = 0;
    int area = 0;
    char sql_str[256] = {0};
    char val[64] = {0};

    if( DBOpen( 3, "", "toplink", "toplink" ) == E_FAIL ) {
        fprintf( stderr, "[%ld]\n", FAP_SQLCODE );
        exit( -1 );
    }

    /* 分配数据池缓存*/
    area = MallocArea();
    if ( E_FAIL == area )
    {
        printf("MallocArea()返回失败\n");
        return E_FAIL;
    }

    snprintf(sql_str, sizeof(sql_str),
            "SELECT COLUMN_NAME FROM USER_IND_COLUMNS "
            "WHERE INDEX_NAME='%s' ORDER BY COLUMN_POSITION", _index_name);

    printf("sql_str[%s]\n", sql_str);

    while (1)
    {
        /* 读取一条待处理记录 */
        ret = DBSelect( area, sql_str, 0, 0 );
        if( ret == E_NOTFOUND )
        {
            printf("No data found [%s]\n", sql_str );
            break;
        }

        if( ret == E_FAIL )
        {
            printf( "sql excute error [%s]\n", sql_str );
            FreeArea();
            return E_FAIL;
        }
        printf("-------------------------------------------\n");

        DBGetData(0, val, sizeof( val ));
        printf("GetData[0]: [%s]\n", val);
    }

    FreeArea();

    DBClose();

    return 0;
}


int get_table_def()
{
    int  i = 0;
    int ret  = 0;
    int area = 0;
    int col_num = 0;
    long row_size = 0;
    char sql_str[256] = {0};
    char val[64] = {0};
    tbl_log_counter_def tt;
    char col_name[32+1] = {0};
    int  col_type = 0;
    int  col_value_max = 0;
    long col_offset = 0;

    if( DBOpen( 3, "", "toplink", "toplink" ) == E_FAIL ) {
        fprintf( stderr, "[%ld]\n", FAP_SQLCODE );
        exit( -1 );
    }

    /* 分配数据池缓存*/
    area = MallocArea();
    if ( E_FAIL == area )
    {
        printf("MallocArea()返回失败\n");
        return E_FAIL;
    }

    /*
    sprintf (sql_str, "select * from tbl_log_counter where pan='%s' and txn_type='%s'",
            "6216920000008264", "ZHXXYZ");
            */
    sprintf (sql_str, "select pan, txn_type, txn_date from tbl_log_counter");
    printf("sql_str[%s]\n", sql_str);

    while (1)
    {
        /* 读取一条待处理记录 */
        ret = DBSelect( area, sql_str, 0, 0 );
        if( ret == E_NOTFOUND )
        {
            printf("No data found [%s]\n", sql_str );
            break;
        }

        if( ret == E_FAIL )
        {
            printf( "sql excute error [%s]\n", sql_str );
            FreeArea();
            return E_FAIL;
        }
        printf("-------------------------------------------\n");

        col_num  = db_get_ncols(area);
        printf("column number: %d\n", col_num);

        for (i = 0; i < col_num; i++)
        {
            col_offset = db_get_column(i, col_name, sizeof(col_name), &col_type, &col_value_max);
            printf("idx[%s, %d, %d, %ld]\n", col_name, col_type, col_value_max, col_offset);
        } // for

        row_size = db_get_rowsize();
        printf("row_size1: %ld\n", row_size);
        printf("row_size2: %ld\n", sizeof(tbl_log_counter_def));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, pan));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, txn_type));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, txn_date));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, fail_continue_num));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, fail_total_num));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, succ_total_num));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, inst_date_time));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, last_fail_time));
        printf("offset: %ld\n", offsetof(tbl_log_counter_def, last_succ_time));

        memset (&tt, 0, sizeof (tt));
        DBGetStru( 0, &tt, sizeof(tt));
        printf("pan                 [%s]\n", tt.pan);
        printf("txn_type            [%s]\n", tt.txn_type);
        printf("txn_date            [%s]\n", tt.txn_date);
        printf("fail_continue_num   [%ld]\n", tt.fail_continue_num);
        printf("fail_total_num      [%ld]\n", tt.fail_total_num);
        printf("succ_total_num      [%ld]\n", tt.succ_total_num);
        printf("inst_date_time      [%s]\n", tt.inst_date_time);
        printf("last_fail_time      [%s]\n", tt.last_fail_time);
        printf("last_succ_time      [%s]\n", tt.last_succ_time);

        DBGetData(0, val, sizeof( val ));
        printf("GetData[0]: [%s]\n", val);
    }

    FreeArea();

    DBClose();

    return 0;
}

int main()
{
    int rv = 0;

#if 0
    if (get_table_def())
    {
        fprintf( stderr, "[%ld]\n", FAP_SQLCODE );
        exit( -1 );
    }
#endif

    if (get_index_def("PK_TBL_LOG_TXN"))
    {
        fprintf( stderr, "[%ld]\n", FAP_SQLCODE );
        exit( -1 );
    }

    return rv;
}
/* ora.c */
