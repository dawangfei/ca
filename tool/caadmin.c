/* Copyright (c)  2017 Cmbc 
   All rights reserved

   THIS IS UNPUBLISHED PROPRIETARY
   SOURCE CODE OF dc Systems, Inc.
   The copyright notice above does not
   evidence any actual or intended
   publication of such source code.
 */
#include "caadmin.h"

#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

#define ALLOPT      2
#define OFFOPT      4
#define MAXARGS     10      /* Maximum number of command line arguments */
#define ARGLEN      1024    /* Maximum len of each argument */

#define EQUAL(x,y) (!strcmp((x), (y)))
#define isnumeric(x) (strspn((x), "-0123456789") == strlen(x))

static int help();
static int quit();
static int see();
static int down();
static int boot();

static int dump();
static int conn();
static int bye();
static int add();

static int info();
static int table_info();

static int selector();
static int reload();
static int addr();
static int cache();
static int dbname();
static int cluster();

/*
 * The following are flags used in the "flags" field of command
 * structures.
 */

#define BOOT    01              /* valid in "boot" mode */
#define READ    02              /* valid in "read" mode */
#define WIZ     04              /* valid only in "wizard" mode */
#define SM      010             /* valid only in "SM" mode */
#define PRV     020             /* valid in "private" mode */
#define ADM     040             /* valid only if administrator */
#define CONF    0100            /* valid in "conf" mode */
#define MASTER  0200            /* valid in "master" mode */
#define PAGE    0400            /* paginate output of this command */

/*
 *  The following are symbolic names and corresponding message
 *  catalog numbers (DCCMD_CAT).
 */

#define DCCERR      1000    /* Custom Error Message */
#define DCLONGARG   1001    /* Argument too long */
#define DCBADCMD    1002    /* No such command */
#define DCBADOPT    1003    /* Invalid option */
#define DCARGCNT    1004    /* Too many arguments */
#define DCDUPARGS   1005    /* Duplicate arguments */
#define DCQUIT      1006    /* Break out of main loop */
#define DCSYNERR    1007    /* Syntax error on command line */

/*
 * sub command structure
 */
struct cmds_t {
    int desc_msg;
    char *name;
    char *alias;
    int prms_msg;
    short flags;
    int (*proc)();
    char valid_opts[MAXARGS+1];
};

/* global sub cmd table */
static struct cmds_t cmds[] = {
    { 1001, "help",         "h",    1001, (READ), help, "" },

    { 1107, "quit",         "q",    1107, (READ), quit, "" },
    { 1108, "exit",         "Q",    1108, (READ), quit, "" },
    { 1109, "see",          "S",    1109, (READ), see, "" },

    { 1101, "boot",         "B",    1101, (READ), boot, "k" },
    { 1102, "down",         "D",    1102, (READ), down, "" },
    { 1103, "add",          "a",    1103, (READ), add, "tm" },
    { 1104, "addr",         "A",    1104, (READ), addr, "x" },
    { 1105, "cache",        "C",    1105, (READ), cache, "s" },
    { 1106, "dbname",       "E",    1106, (READ), dbname, "sup" },

    { 1201, "conn",         "c",    1201, (READ), conn, "k" },
    { 1202, "connect",      "c",    1202, (READ), conn, "k" },
    { 1203, "bye",          "b",    1203, (READ), bye,  "" },
    { 1205, "dump",         "p",    1205, (READ), dump, "t" },

    { 1206, "info",         "F",    1206, (READ), info, "" },
    { 1207, "table",        "T",    1207, (READ), table_info, "t" },
    { 1208, "reload",       "R",    1208, (READ), reload, "t" },

    { 1301, "cluster",      "U",    1301, (READ), cluster, "st" },
};

const int maxcmds = sizeof(cmds) / sizeof(cmds[0]);


/* switch on/off */
static short d_paginate = FALSE;    /* paginate flag */

/* global memory */
static char cmdline[2 * BUFLEN] = {0}; /* input command line */
static char cmdargs[MAXARGS][ARGLEN + 4]; /* command line arguments */
static char prevcmd[2 * BUFLEN] = {0}; /* previous command line */
static char prevsys[2 * BUFLEN] = {0}; /* previous system command */

static int gotintr = 0; /* flags for interrupt */
static int numargs; /* number of command line args */
static int oldargs = 0; /* number of command line args */
static char g_user[32] = {0};

static FILE *stream; /* input source */

/* current option-value and default option-value storage */
#define  OPTNOARG       0x1
#define  OPTSETDFLT     0x2
#define  OPTNUMVAL      0x4

#define  OPT_NUMS       63      /* a-z ,  A-Z , 0-9 , no opt */

struct opt_val_t {
    short c_opt; /* FALSE | TRUE | OFFOPT | ALLOPT */
    short d_opt; /* FALSE | TRUE | OFFOPT | ALLOPT */
    short optflag; /* OPTNOARG, OPTSETDFLT, OPTNUMVAL */
    char * c_value; /* current value */
    char d_value[ARGLEN + 4]; /* default value */
};
static struct opt_val_t opts[OPT_NUMS];

#define C_OPT(c)    (opts+opt_subs(c))->c_opt
#define D_OPT(c)    (opts+opt_subs(c))->d_opt
#define C_VALUE(c)  (opts+opt_subs(c))->c_value
#define D_VALUE(c)  (opts+opt_subs(c))->d_value
#define OPTFLAG(c)  (opts+opt_subs(c))->optflag

#define ADM_KEY_SELECT          "SELECT"
#define ADM_KEY_SELECT_LEN      6

static char noarg_opts[] = "av"; /* listed options do not accept argument */
static char setdflt_opts[] = ""; /* listed options can set default */
static char numval_opts[] = "n";  /* listed options must have numeric values */

static int maininit(const char *);
static void intr(int);
static void initialize();
static int process();
static int scan(char[]);
static int determ();
static void adm_cleanup();
static void DC_cpnotice(char *str);
static int dc_adm_has_connected();

#if STARRING_INTEGRATE
static int g_pe_connected = 0;
static int get_db_connect(char* db_name);
#endif

static char *proc_name;
static long  g_shm_key = -1;
static int   g_connected = 0;
static char  g_dbname[100] = {0};
static char  g_db_username[100] = {0};
static char  g_db_password[100] = {0};

int dc_ext_connect_db();
int dc_ext_connect_to_db();
int dc_ext_already_connect_db();
void bcl_set_log_file(char *_filename);


/* OPTION subscriptor
 * 0         : no option
 * 1 - 10  : '0' - '9'
 * 11 - 36: 'a' - 'z'
 * 37 - 62: 'A-'Z'
 */
#define  NOOPT          0
static int opt_subs(char c) {
    if (c >= '0' && c <= '9')
        return (1 + c - '0');
    else if (c >= 'a' && c <= 'z')
        return (11 + c - 'a');
    else if (c >= 'A' && c <= 'Z')
        return (37 + c - 'A');
    else
        return -1;
}


/* some rubbish source code for Bug 9277025
 * subscriptor OPTION
 * input 1-OPT_NUMS-1
 * 0         : option no
 * 1 - 10  : '0' - '9'
 * 11 - 36: 'a' - 'z'
 * 37 - 62: 'A-'Z'
 */
static char subs_opt(int x) {

    if (x >= 1 && x <= 10)
        return (char)(-1 + x + '0');
    else if (x >= 11 && x <= 36)
        return (char)(-11 + x + 'a');
    else if (x >= 37 && x <= 62)
        return (char)(-37 + x + 'A');
    else
        return '\0';
}


static void show_usage(char *proc_name)
{
    printf("Usage: %s \n", proc_name);
    exit(1);
}


int main(int argc, char *argv[])
{
    int i;
    char profile_path[BUFLEN] = "";

    /*
     * process "-E appname", if present in argv, and read appname section
     * from environment file
     */

    /* set program name prior to making the first userlog */
    proc_name = "dcadmin"; /* argv[0] is program name */

    /* initialize the locale */

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            if ((i < argc - 1)&&(argv[i + 1][0] != '-'))
            {
                strncpy(profile_path, argv[i + 1], sizeof(profile_path)-1);
                i++;
            }
            else
            {
            }
        }
        else
        {
            show_usage(proc_name);
        }
    }

    /*
     * Very first thing we do (after checking the command line options)
     * is print out the copyright.
     */
    (void) DC_cpnotice(proc_name);

    maininit(profile_path);

    /*CONSTCOND*/
    while (1)
    {
        initialize(); /* initialize variables, allocate memory */

        if (gotintr)
        {
            adm_cleanup(); /* free memory, detach */
            break;
        }

        switch (process())
        {
            case DCQUIT: /* leave dmadmin */
                adm_cleanup();
                return (0);
        }
    } /* while */

    return (0);
}


/*
 * 1 deel with user name and user_password
 * 2 call init
 *
 * user_name: inputed user name, length is 0 if no user
 * name input, cannot be null
 *
 * profile_path: inputed profile path
 */
static int maininit(const char *profile_path)
{
    int rv = 0;

    profile_path = profile_path;

    /* go to routine intr on interrupt */
    if (signal(SIGINT,  SIG_IGN) !=  SIG_IGN)
        (void) signal(SIGINT,  intr);

    if (signal(SIGPIPE,  SIG_IGN) !=  SIG_IGN)
        (void) signal(SIGPIPE,  intr);

    if (getlogin_r(g_user, sizeof(g_user))) 
    {
        fprintf(stderr, "error: getlogin_r failure!\n");
        return -1;
    }

    bcl_set_log_file("caadm.log");

    dc_aux_init();

    return rv;
}


static void intr(int sig)
{
    (void) signal(SIGINT,  intr);
    (void) signal(SIGPIPE,  intr);
    sig = sig;
    gotintr++; /* gotintr > 0 means we got an interrupt */
    return;
}

static void initialize()
{
    int  i = 0;
    char *p = NULL;

    /* no previous command lines either */
    prevcmd[0] = prevsys[0] = cmdline[0] = '\0';
    oldargs = 0;

    /* set option-value */
    for (i = 0; i < OPT_NUMS; i++)
    {
        opts[i].c_opt = opts[i].d_opt = FALSE;
        opts[i].optflag = 0;
        opts[i].c_value = NULL;
        opts[i].d_value[0] = '\0';
    }

    for (p = &noarg_opts[0]; *p != '\0'; p++)
        OPTFLAG(*p) |= OPTNOARG;

    for (p = &setdflt_opts[0]; *p != '\0'; p++)
        OPTFLAG(*p) |= OPTSETDFLT;

    for (p = &numval_opts[0]; *p != '\0'; p++)
        OPTFLAG(*p) |= OPTNUMVAL;

    stream = stdin;
    d_paginate = (isatty(0) && isatty(1));

    return;
}


static int process()
{
    struct cmds_t *ct; /* command table */
    int err, cur;
    int i;

    /* CONSTCOND */
    while (1)
    {
        if (gotintr) {
            gotintr = 0;
            (void) printf("\n");
        }

        (void) printf("\n%s ", ">");

        /* flush the output streams to clear out the buffer */
        (void) fflush(stdout);

        /* Read in the command line here */
        if (fgets(cmdline, 2 * BUFLEN, stream) == NULL) { /* EOF or err */
            if (feof(stream))
            {
                /* actually end of file; look like "quit" */
                (void) strcpy(cmdline, "quit\n");
                (void) putchar('\n');
            } else {
                /* just an interrupt, so give prompt again */
                continue;
            }
        }

        if (strlen(cmdline) > 0 && strncasecmp(cmdline, ADM_KEY_SELECT, ADM_KEY_SELECT_LEN) == 0)
        {
            strcpy(prevcmd, cmdline);
            selector();
            continue;
        }

        /*
         * Parse the command line here. Note that DCCERR is
         * never returned from "scan".
         */
        if ((err = scan(cmdline)) > 0) {
            printf("%s\n", cmdline);
            continue;
        }

        /*
         * numargs == 0 means that nothing but white space was entered
         * on the command line, just ignore.
         */
        if (numargs == 0) {
            if (prevcmd[0] != 0)
            {
                printf("> %s", prevcmd);
                fflush(stdout);
            }
            continue;
        } else {
            /*
             * Set the variables in case this command line has
             * to be called again
             */
            oldargs = numargs;
            (void) strcpy(prevcmd, cmdline);
        }

        if (*cmdline == '!')
        {
            /* shell command entered */
            if (*(cmdline + 1) == '!') {
                /*
                 * Repeat previous shell command, but first
                 * print what the previous command was.
                 */
                (void) printf("%s", prevsys);
            } else {
                /*
                 * Save for next !! command. (cmdline+1) is
                 * used to remove the leading '!'
                 */
                (void) strcpy(prevsys, cmdline + 1);
            }

            if (system(prevsys) != 0)
            {
                printf("ERROR: Error executing [%s].\n", prevsys);
            }

            continue;
        }

        if (*cmdline == '#') /* comment line */
        {
            continue;
        }

        ct = cmds; /* array of legal commands */
        cur = maxcmds; /* number of commands */

        while (cur--)
        {
            /* cmdargs[0] contains the command name */
            if (EQUAL(cmdargs[0],ct->name) || EQUAL(cmdargs[0],ct->alias))
            {
                if ((err = determ()) > 0)
                    break;

                if (strlen(ct->valid_opts)!=0)
                {
                    for(i=1; i<OPT_NUMS; i++)
                    {
                        if((opts+i)->c_opt)
                        {
                            if(strchr(ct->valid_opts, subs_opt(i))==NULL)
                            {
                                err = DCSYNERR;
                                printf("%c is not a valid option\n", subs_opt(i));
                                (void) printf("%s (%s)\n", ct->name, ct->alias);
                                return err;
                            }
                            /*else
                              printf("%c is a valid option\n", subs_opt(i));*/
                        }
                    }
                }

                err = (*ct->proc)();

                break;
            }

            ct++; /* try next element in command array */
        }

        /*
         * Special conditions: we want to break out of here and
         * let the main routine take control.
         */
        if (err == DCQUIT)
            break;

        if (cur == -1) /* cur == -1 means no such command */
            err = DCBADCMD;

        if (err > 0)
        {
        }

    } /* loop always */

    return (err);
}


static void DC_cpnotice(char *str)
{
    (void) printf("%s - Copyright (c) 2017 Cmbc\n", str);
    (void) printf("All Rights Reserved.\n");
}


static void adm_cleanup()
{
    /* (void)term(); */
}


static int determ()
{
    /*
     * This routine will go thru the cmdargs array and assign the elements
     * in that array to the proper variables and set the appropriate flags.
     * This routine assumes that the elements of cmdargs array are of the
     * form "-oarg"; the switch statement below branches on the
     * value of the option, "o". The argument, "arg", is stored in
     * the variable "temp". For value arguments, (those without
     * options), the switch statement is bypassed entirely.
     * If the argument is "*", then that option should be "unset".
     */

    int cur; /* current element in the cmdargs array */
    int i;

    for (i = 0; i < OPT_NUMS; i++)
    {
        opts[i].c_value = NULL;
        /* Reset all options */
        opts[i].c_opt = FALSE;
    }

    /* Note that cmdargs[0] is the command, so we ignore that element */
    for (cur = 1; cur < numargs; cur++)
    {
        int opt_subscriptor;
        char *temp;

        if (cmdargs[cur][0] == '-')
        {
            opt_subscriptor = opt_subs(cmdargs[cur][1]);
            if (-1 == opt_subscriptor)
            {
                return (DCBADOPT);
            }

            if (opts[opt_subscriptor].c_opt)
            {
                return (DCDUPARGS);
            }

            /* an option element */
            temp = cmdargs[cur] + 2;
            if (EQUAL(temp, "*") && (opts[opt_subscriptor].optflag & OPTSETDFLT))
            {
                opts[opt_subscriptor].c_opt = OFFOPT;
            }
            else
            {
                if ((opts[opt_subscriptor].optflag & OPTNUMVAL) && !isnumeric(temp))
                {
                    (void)printf("ERROR: -%c option: argument must be numeric.\n", cmdargs[cur][1]);
                    return (DCBADOPT);
                }
                opts[opt_subscriptor].c_opt = TRUE;
                opts[opt_subscriptor].c_value = temp;
            }
        } else {
            /* value argument (no option) */
            if (opts[NOOPT].c_opt)
            {
                return (DCDUPARGS);
            }

            if (EQUAL(cmdargs[cur],"*"))
            {
                opts[NOOPT].c_opt = OFFOPT;
            }
            else
            {
                opts[NOOPT].c_opt = TRUE;
            }

            opts[NOOPT].c_value = cmdargs[cur];
        }
    }

    return (0);
}

static int scan(char * s)
{
    /*
     * This routine parses the command line (s) and forms the array cmdargs.
     * It also removes leading AND trailing white space, checks length
     * of arguments, etc. Each element returned in the cmdargs array is of
     * the form "-oarg", where o is the option (such as g) and
     * arg is the argument (such as 400). For those arguments which
     * don't have option letters, such as "printqueue 3487", only
     * the argument itself is returned.
     */

    int c, i, pos, optflag, quoteflag;
    /*
     * i is the position we are currently at in the command line (s).
     * pos is the position we are currently at in a command element.
     * numargs indicates the current element in the cmdargs array.
     * optflag contains the current state of the parser (as described
     * below).
     */

    i = pos = optflag = numargs = quoteflag = 0;

    while (isspace((int)s[i])) /* remove leading white space */
        i++;

    /* don't test this if no leading white space (since i-1 = -1 !!) */
    if ((i) && (s[i - 1] == '\n')) {
        return (0);
    }

    /*
     * If a shell command or a comment is entered, we don't want to
     * parse it, so return immediately.
     */
    if ((s[i] == '!') || (s[i] == '#')) {
        numargs++;
        (void) strcpy(cmdline, s + i);
        return (-1);
    }

    optflag = 0;

    /*CONSTCOND*/
    while (1) {
        /* remove leading white space */
        while ((isspace((int)s[i])) && (s[i] != '\n'))
            i++;
        quoteflag = FALSE; /* not in a quoted string */

        while ((c = s[i++]) != '\n') {
            if (isspace(c) && !quoteflag)
                /* indicates end of current element */
                break;

            /*
             * optflag == 0 means that we haven't encountered
             * a "-" yet.
             * optflag == 1 means that we have encountered a "-".
             * optflag == 2 means that we have an option for "-",
             * such as "-g".
             * optflag > 2 means that we have an option and
             * argument, such as "-g 400".
             */

            if ((optflag != 0) && (c != '-') && !quoteflag)
                optflag++;
            if (!quoteflag && (c == '"'))
                quoteflag++;
            else if (quoteflag && (c == '"'))
                break;
            if (c == '-' && !quoteflag) {
                if (optflag == 0)
                    optflag++; /* leading "-" */
                else if (optflag < 3) { /* just "-" or "-g" */
                    /*
                     * Note that for an embedded "-", such as
                     * "-s enzo-greco", optflag is = 2
                     */
                    return (DCSYNERR);
                }
            }

            if (pos > ARGLEN) {
                return (DCLONGARG); /* argument too long */
            }

            if (numargs >= MAXARGS) {
                return (DCARGCNT); /* too many arguments */
            }

            if ((c != '"') || !quoteflag) /* eliminate quotes in string */
                cmdargs[numargs][pos++] = (char) c; /* assign the char */
        }

        /*
         * At this point, if optflag == 2, we just have the option,
         * "-g", for example, so we need to go through and get the
         * argument now.
         */

        /* if just "-" or "-g", for example */
        if (optflag == 2 && (strchr(noarg_opts, cmdargs[numargs][1]))) {
            /* -v or -t option - no argument */
            optflag = 3;
        } else if ((optflag == 1) || (((optflag == 2) || quoteflag) && (c
                        == '\n'))) {
            return (DCSYNERR);
        }

        if ((optflag == 0) && (pos == 0))
            break; /* for white space at end of line */

        /* a valid element, so assign it and prepare for the next one */
        if ((optflag == 0) || (optflag > 2)) {
            cmdargs[numargs][pos] = '\0';
            optflag = 0;
            numargs++;
            pos = 0;
        }

        if (c == '\n')
            break;
    }

    return (0);
}


static int help()
{
    struct cmds_t *ct; /* command table */
    int len, cur;
    /* We either have a value opt on the cmd line or nothing at all */
    if (numargs != 1)
    {
        if ((numargs != 2) || (!opts[NOOPT].c_opt))
        {
            return (DCSYNERR);
        }
    }

    ct  = cmds; /* ct points to the begin of the cmds array */
    cur = maxcmds; /* Number of commands to consider */

    if ((opts[NOOPT].c_opt) && (!EQUAL(opts[NOOPT].c_value,"all")))
    {
        /* we do this only if a particular command, such as "psr" was given */
        while (cur--)
        {
            /* Now go through the array and look for it */
            if ((EQUAL(opts[NOOPT].c_value,ct->name))
                    || (EQUAL(opts[NOOPT].c_value,ct->alias))) {
                /*
                 * If an invalid command is given,
                 * return "No such command."
                 */
                if ((EQUAL("wizard",ct->name)))
                    cur = -1;
                else
                    cur = 1; /* Only want to print out */
                break; /* 1 entry */
            }
            ct++; /* Go on to next entry */
        }
        if (cur == -1) { /* Couldn't find the desired command */
            return (DCBADCMD);
        }
    }

    for (; cur-- && !gotintr; ct++)
    {
        char *prms_ptr = "";

        /* determine if this is a valid command */
        if ((EQUAL("wizard",ct->name)))
            continue;

        /* catcheck:OFF */
        /* catcheck:ON */

        (void) printf("\n%s (%s)", ct->name, ct->alias);

        if (opts[NOOPT].c_opt) {
            /*
             * If an argument given, then print out a
             * description also.
             */
            (void) putchar('\n');
            /* 4 is added to take into account 2 spaces and "()" */
            len = 4 + strlen(ct->name) + strlen(ct->alias) + strlen(prms_ptr);
            /* Remove an extra underline on the end if no parms. */
            if (EQUAL(prms_ptr,""))
                len--;
            if (len > 75)
                len = 75;

            while (len--) /* Underline */
                (void) putchar('-');
            /* catcheck:OFF */
            /* catcheck:ON */
        }
    }

    if (!opts[NOOPT].c_opt) /* Need to end the last line properly */
        (void) putchar('\n');

    return (0);
}



static int quit()
{
    return DCQUIT;
}


/*
 */
static int processer_template(int (*func)())
{
    int rv;

    rv = func();

    return rv;
}


static int see_processer()
{
    int rv = 0;

    return rv;
}


static int down_processer()
{
    int   rv = 0;

    g_connected = 0;
    if (dc_cache_api_admin_close())
    {
        fprintf(stderr, "error: dc_cache_api_admin_close\n");
        rv = -1;
    }
    else
    {
        printf("down succeeds");
    }

    return rv;
}


static int boot_processer()
{
    char  *p = NULL;

    p = C_VALUE('k');
    if (p == NULL)
    {
        if (g_shm_key < 0)
        {
            printf("usage: boot -k 7788\n");
            return 1;
        }
    }
    else
    {
        g_shm_key = atol(p);
    }

    if (dc_cache_api_is_already_open())
    {
        printf("error: already open\n");
        return -1;
    }

    /* connect to DB */
#if STARRING_INTEGRATE
    if (g_dbname[0] == 0)
    {
        printf("dbname -s xxx\n");
        return -1;
    }

    if (get_db_connect(g_dbname) == -1)
    {
        fprintf(stderr, "error: get_db_connect: %s\n", g_dbname);
        return -1;
    }
#else
    if (g_dbname[0] == 0 || g_db_username[0] == 0 || g_db_password[0] == 0)
    {
        printf("dbname -s dbname -u username -p password\n");
        return -1;
    }

    if (dc_ext_connect_to_db("", g_db_username, g_db_password) == -1)
    {
        fprintf(stderr, "error: dc_ext_connect_to_db: [%s/%s@%s]\n", g_db_username, g_db_password, g_dbname);
        return -1;
    }
#endif

    if (dc_cache_api_admin_init(g_shm_key))
    {
        fprintf(stderr, "error: dc_cache_api_admin_init\n");
        return -1;
    }
    g_connected = 1;
    printf("created");

    return 0;
}

static int see()
{
    return processer_template(see_processer);
}


static int down()
{
    return processer_template(down_processer);
}


static int boot()
{
    return processer_template(boot_processer);
}


static int dump_processer()
{
    char *name = NULL;

    if (!dc_adm_has_connected())
    {
        printf("conntect first!\n");
        return -1;
    }

    name = C_VALUE('t');
    if (name == NULL)
    {
        dc_cache_api_dump_block();
    }
    else
    {
        printf("table: %s\n", name);
        dc_cache_api_dump_table(name);
    }

    return 0;
}


static int dump()
{
    return processer_template(dump_processer);
}


static int conn_processer()
{
    char  *p = NULL;

    p = C_VALUE('k');
    if (p == NULL)
    {
        printf("usage: conn -k 7788\n");
    }
    else
    {
        g_shm_key = atol(p);

        if (dc_cache_api_is_already_open())
        {
            printf("already open\n");
            if (dc_cache_api_close())
            {
                fprintf(stderr, "error: dc_cache_api_close\n");
            }
        }

        if (dc_cache_api_open(g_shm_key))
        {
            fprintf(stderr, "error: dc_cache_api_open\n");
            return -1;
        }
        printf("connect %s succeeds\n", p);

        g_connected = 1;
    }

    return 0;
}


static int conn()
{
    return processer_template(conn_processer);
}


static int bye_processer()
{
    g_connected = 0;
    return dc_cache_api_close();
}


static int bye()
{
    return processer_template(bye_processer);
}


static int selector()
{
    int  n = 0;
    int  len = 0;
    int  is_aggregation = 0;
    char sql[1024] = {0};

    if (!dc_adm_has_connected())
    {
        printf("conntect first!\n");
        return -1;
    }

    snprintf(sql, sizeof(sql), "%s", cmdline);

    len = strlen(sql);
    if (sql[len-1] == '\n')
    {
        sql[len-1] = 0;
        len--;
    }

    if (dc_cache_api_cursor(sql) < 0)
    {
        fprintf(stderr, "error: dc_cache_api_cursor: %s\n", sql);
        return -1;
    }

    is_aggregation = dc_cache_api_is_aggregation_count(&n);
    if (is_aggregation < 0)
    {
        fprintf(stderr,  "error: dc_cache_api_is_aggregation_count\n");
        return -1;
    }

    if (is_aggregation)
    {
        printf("%d\n", n);
    }
    else
    {

        while (dc_cache_api_fetch() != NULL)
        {
            if (dc_cache_api_dump_curr_row() < 0)
            {
                fprintf(stderr, "error: dc_cache_api_dump_curr_row\n");
                return -1;
            }
            printf("==================================================\n");
            n++;
        }

        if (n == 0)
        {
            printf("No data found!\n");
        }
        else
        {
            printf("%d rows\n", n);
        }
    }

    return 0;
}


static int add_processer()
{
    int   max_rows = 0;
    char *table_name = NULL;
    char *p = NULL;

    p = C_VALUE('t');
    if (p == NULL)
    {
        printf("add -t t_bin -m 100\n");
        return -1;
    }
    else
    {
        table_name = p;
    }

    p = C_VALUE('m');
    if (p == NULL)
    {
        printf("add -t t_bin -m 100\n");
        return -1;
    }
    else
    {
        max_rows = atoi(p);
    }

    if (dc_aux_seg_add(table_name, max_rows))
    {
        printf("error: dc_aux_seg_add: %s, %d", table_name, max_rows);
        return -1;
    }
    printf("add: table[%s], max-row[%d] succeeds\n", table_name, max_rows);

    return 0;
}

static int add()
{
    return processer_template(add_processer);
}


static int info_processer()
{
    char *name = NULL;

    if (!dc_adm_has_connected())
    {
        printf("conntect first!\n");
        return -1;
    }

    name = C_VALUE('t');
    if (name == NULL)
    {
        dc_cache_api_shm_info();
    }
    else
    {
    }


    return 0;
}


static int info()
{
    return processer_template(info_processer);
}


static int dc_adm_has_connected()
{
    return g_connected;
}


static int table_processer()
{
    char *name = NULL;

    if (!dc_adm_has_connected())
    {
        printf("conntect first!\n");
        return -1;
    }

    name = C_VALUE('t');
    if (name == NULL)
    {
        dc_cache_api_list_tables();
    }
    else
    {
        dc_cache_api_desc_table(name);
    }


    return 0;
}


static int table_info()
{
    return processer_template(table_processer);
}


static int reload_processer()
{
    char *name = NULL;

    if (!dc_adm_has_connected())
    {
        printf("conntect first!\n");
        return -1;
    }

    name = C_VALUE('t');
    if (name == NULL)
    {
        printf("reload -t T_BIN\n");
        return -1;
    }
    else
    {
        /* connect to db */
#if STARRING_INTEGRATE
        if (!g_pe_connected)
        {
            if (get_db_connect(g_dbname) == -1)
            {
                fprintf(stderr, "error: get_db_connect: %s\n", g_dbname);
                return -1;
            }
        }
#else
        if (!dc_ext_already_connect_db())
        {
            if (g_dbname[0] == 0 || g_db_username[0] == 0 || g_db_password[0] == 0)
            {
                printf("dbname -s dbname -u username -p password\n");
                return -1;
            }

            if (dc_ext_connect_to_db(g_dbname, g_db_username, g_db_password) == -1)
            {
                fprintf(stderr, "error: dc_ext_connect_to_db: [%s/%s@%s]\n", g_db_username, g_db_password, g_dbname);
                return -1;
            }
        }
#endif

        /*
        printf("reloading: [%s]\n", name);
        */
        if (dc_cache_api_reload(name))
        {
            fprintf(stderr, "error: dc_cache_api_reload: %s\n", name);
            return -1;
        }
        printf("table %s reloaded\n", name);
    }

    return 0;
}


static int reload()
{
    return processer_template(reload_processer);
}


#define DC_IS_LOWERHEX(x)     ((x >= 'a') && (x <= 'f'))
#define DC_IS_UPPERHEX(x)     ((x >= 'A') && (x <= 'F'))
#define DC_IS_NUMBER(x)       (x >= '0' && x <= '9')

static int dc_str_addr_to_real_addr(char *_str_addr, void **_addr)
{
    int i   = 0;
    int len = 0;
    int fac = 0;
    char c = 0;
    unsigned long b = 1;
    unsigned long v = 0;

    len = strlen(_str_addr);

    for (i = 0; i < len; i++)
    {
        c = _str_addr[i];

        if (DC_IS_NUMBER(c))
            fac = (c - '0');
        else if(DC_IS_LOWERHEX(c))
            fac = (c - 'a' + 10);
        else if(DC_IS_UPPERHEX(c))
            fac = (c - 'A' + 10);
        else
            return -1;

        v += fac * (b << ((len-i-1)*4));
    }

    *_addr = (void *)v;

    return 0;
}


static int addr_processer()
{
    int len = 0;
    char *str_addr = NULL;
    void *real_addr = NULL;

    if (dc_adm_has_connected())
    {
        printf("already connected!\n");
        return -1;
    }

    str_addr  = C_VALUE('x');
    if (str_addr == NULL)
    {
        printf("addr -x 700000360000000\n");
        return -1;
    }
    else
    {
        len = strlen(str_addr);
        if (len > 16 || len < 15)
        {
            printf("invalid addr length: expect[15, 16]!\n");
            return -1;
        }
        if (dc_str_addr_to_real_addr(str_addr, &real_addr))
        {
            printf("invalid addr format\n");
            return -1;
        }
        dc_aux_ipc_set_shm_init_addr(real_addr);
        printf("set addr %p succeeds\n", real_addr);
    }

    return 0;
}


static int addr()
{
    return processer_template(addr_processer);
}


static int cache_processer()
{
    int  rv = 0;
    char *action = NULL;

    if (!dc_adm_has_connected())
    {
        printf("conntect first!\n");
        return -1;
    }

    action = C_VALUE('s');
    if (action == NULL)
    {
        printf("cache -s {enable|disable}\n");
        return -1;
    }
    else
    {
        /* printf("cache set action: %s\n", action); */
        if (strcasecmp(action, "enable") == 0)
        {
            rv = dc_cache_api_set_mode(1);
            if (rv)
            {
                printf("error: enable cache failure");
                return -1;
            }
            printf("cache mode enabled\n");
        }
        else if (strcasecmp(action, "disable") == 0)
        {
            rv = dc_cache_api_set_mode(2);
            if (rv)
            {
                printf("error: diable cache failure");
                return -1;
            }
            printf("cache mode disabled\n");
        }
        else
        {
            printf("cache -s {enable|disable}\n");
            return -1;
        }
    }

    return 0;
}


static int cache()
{
    return processer_template(cache_processer);
}

#if STARRING_INTEGRATE
static int get_db_connect(char* db_name)
{
    char        filename[255];
    xmlDocPtr   xml;
    xmlNodePtr  node_ptr;
    xmlNodePtr  root_ptr;
    int         ret;

    node_ptr = NULL;
    root_ptr = NULL;

    memset (filename, 0x00, sizeof(filename));
    sprintf (filename, "%s/etc/ESAdmin.xml", (char*)getenv("FAPWORKDIR"));

    xml = xmlParseFile (filename);
    if (xml == NULL)
    {
         fprintf (stderr, "打开XML文件【%s】失败\n", filename);

         return -1;
    }

    root_ptr = xmlDocGetRootElement (xml);
    if (root_ptr == NULL)
    {
         fprintf (stderr, "error: xmlDocGetRootElement\n");
         xmlFreeDoc (xml);

         return -1;
    }

    /* printf ("root_ptr -> name:%s\n", root_ptr -> name); */
    /* printf ("root_ptr -> content:%s\n", root_ptr -> content); */

    node_ptr = root_ptr -> children;

    while (1)
    {
         if (node_ptr == NULL)
         {
              fprintf (stderr, "error: node_ptr\n");
              xmlFreeDoc (xml);

              return -1;
         }

         if (strcmp ((char*)node_ptr -> name, "DataBaseTab") == 0 )
         {
              node_ptr = node_ptr -> children;
              while (1)
              {
                   if (node_ptr == NULL)
                   {
                        fprintf (stderr, "error: [%s]\n",db_name);
                        xmlFreeDoc (xml);

                        return -1;
                   }
                   if (strcmp ((char*)node_ptr -> name,"DataBase") == 0)
                   {
                        if (strcmp ((char*)xmlGetProp(node_ptr,(const unsigned char*)"DBName"), db_name) == 0)
                        {
                             ret=DBOpen( 3, db_name, (char*)xmlGetProp(node_ptr,(const unsigned char*)"DBUserName"), (char*)xmlGetProp(node_ptr,(const unsigned char*)"DBPassword") );
                             if (ret == -1)
                             {
                                  fprintf (stderr, "error: DBOpen\n");
                                  xmlFreeDoc (xml);

                                  return -1;
                             }

                             printf ("connect to db [%s]\n",db_name);

                             break;
                        }
                   }
                   node_ptr = node_ptr -> next;
              }

              break;
         }

         node_ptr = node_ptr -> next;
    }

    xmlFreeDoc (xml);
    g_pe_connected = 1;

    return 0;
} 
#endif


static int dbname_processer()
{
    char *p = NULL;
    char *usage = "dbname -s pocdb(starring-pe only)\n"
        "dbname -s orcl -u scott -p tiger\n";

    /* DB name */
    p = C_VALUE('s');
    if (p == NULL)
    {
        printf("%s", usage);
        return -1;
    }
    else
    {
        snprintf(g_dbname, sizeof(g_dbname), "%s", p);
        printf("database [%s]\n", g_dbname);
    }

    /* DB username */
    p = C_VALUE('u');
    if (p != NULL)
    {
        snprintf(g_db_username, sizeof(g_db_username), "%s", p);
        printf("username [%s]\n", g_db_username);
    }

    /* DB password */
    p = C_VALUE('p');
    if (p != NULL)
    {
        snprintf(g_db_password, sizeof(g_db_password), "%s", p);
        printf("password [%s]\n", g_db_password);
    }

    return 0;
}


static int dbname()
{
    return processer_template(dbname_processer);
}


static int cluster_processer()
{
    char *action = NULL;
    char *table  = NULL;
    char *my_usage = "cluster -s reload -t T_BIN\n"
        "cluster -s enable\n"
        "cluster -s disable\n";

    table  = C_VALUE('t');

    action = C_VALUE('s');
    if (action == NULL)
    {
        printf("%s", my_usage);
        return -1;
    }
    else
    {
        if (strcasecmp(action, "reload") == 0)
        {
            if (table == NULL)
            {
                printf("cluster -s reload -t T_BIN\n");
                return -1;
            }

            if (dc_cache_api_reload_cluster(table))
            {
                fprintf(stderr, "error: dc_cache_api_reload_cluster: %s\n", table);
                return -1;
            }
            printf("table %s reloaded in cluster mode\n", table);
        }
        else if (strcasecmp(action, "enable") == 0)
        {
            if (table == NULL)
            {
                table = "dummy";
            }

            if (dc_cache_api_enable_table_cluster(table))
            {
                printf("error: dc_cache_api_enable_table_cluster failure");
                return -1;
            }
            printf("cache enabled in cluster mode\n");
        }
        else if (strcasecmp(action, "disable") == 0)
        {
            if (table == NULL)
            {
                table = "dummy";
            }

            if (dc_cache_api_disable_table_cluster(table))
            {
                printf("error: dc_cache_api_disable_table_cluster failure");
                return -1;
            }
            printf("cache disabled in cluster mode\n");
        }
        else 
        {
            printf("%s", my_usage);
        }
    }

    return 0;
}


static int cluster()
{
    return processer_template(cluster_processer);
}

/* caadmin.c */
