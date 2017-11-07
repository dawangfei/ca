#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"
#include "util.h"

#include "ext.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"



/* ================================================================= */
static int g_ext_connected_db = 0;

/* ================================================================= 
   ================================================================= */

void dc_ext_set_connected();
void dc_ext_set_disconnected();

int dc_ext_connect_db()
{
    char dbname[32] = {0};
    char username[32] = {0};
    char password[32] = {0};

#if ORACLE
    snprintf(username, sizeof(username),"%s", "toplink");
    snprintf(password, sizeof(password),"%s", "toplink");
#elif DB2
#if STARRING_INTEGRATE
    snprintf(dbname, sizeof(dbname),    "%s", "pocdb");
#else
    snprintf(dbname, sizeof(dbname),    "%s", "moon");
#endif
    snprintf(username, sizeof(username),"%s", "db2pe");
    snprintf(password, sizeof(password),"%s", "cmbc631");
#endif

    if (DBOpen(3, dbname, username, password) == -1)
    {
        elog("error: DBOpen [%ld]: [%s/%s]@[%s]", FAP_SQLCODE, username, password, dbname);
        return -1;
    }

    dc_ext_set_connected();

    return 0;
}


int dc_ext_connect_to_db(char *_dbname, char *_username, char *_password)
{
    if (DBOpen(3, _dbname, _username, _password) == -1)
    {
        elog("error: DBOpen [%ld]: [%s/%s]@[%s]", FAP_SQLCODE, _username, _password, _dbname);
        return -1;
    }

    g_ext_connected_db = 1;

    return 0;
}

int dc_ext_disconnect_db()
{
    if (!dc_ext_already_connect_db())
    {
        return 0;
    }

    if (DBClose())
    {
        elog("error: DBClose[%ld]", FAP_SQLCODE);
        return -1;
    }
    else
    {
        dc_ext_set_disconnected();
    }

    return 0;
}


int dc_ext_already_connect_db()
{
    return (g_ext_connected_db == 1);
}


void dc_ext_set_connected()
{
    g_ext_connected_db = 1;

    return;
}

void dc_ext_set_disconnected()
{
    g_ext_connected_db = 0;

    return;
}


#if RUN_EXT
int main(int argc, char *argv[])
{
    int  rv  = 0;

    olog("ext begin");

    rv = dc_ext_connect_db();
    if (rv)
    {
        elog("error: dc_ext_connect_db");
        return -1;
    }
    olog("connect to db succeeds");

    olog("ext end");

    return rv;
}
#endif

/* ext.c */
