#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#include "tm.h"


#ifndef TZ_MAX_TIMES
/*
 ** The TZ_MAX_TIMES value below is enough to handle a bit more than a
 ** year's worth of solar time (corrected daily to the nearest second) or
 ** 138 years of Pacific Presidential Election time
 ** (where there are three time zone transitions every fourth year).
 */
#define TZ_MAX_TIMES    370
#endif /* !defined TZ_MAX_TIMES */

#ifndef TZ_MAX_TYPES
#ifndef NOSOLAR
#define TZ_MAX_TYPES    256 /* Limited by what (unsigned char)'s can hold */
#endif /* !defined NOSOLAR */
#ifdef NOSOLAR
/*
 ** Must be at least 14 for Europe/Riga as of Jan 12 1995,
 ** as noted by Earl Chew <earl@hpato.aus.hp.com>.
 */
#define TZ_MAX_TYPES    20  /* Maximum number of local time types */
#endif /* !defined NOSOLAR */
#endif /* !defined TZ_MAX_TYPES */

#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS    50  /* Maximum number of abbreviation characters */
/* (limited by what unsigned chars can hold) */
#endif /* !defined TZ_MAX_CHARS */

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS    50  /* Maximum number of leap second corrections */
#endif /* !defined TZ_MAX_LEAPS */

#define SECSPERMIN  60
#define MINSPERHOUR 60
#define HOURSPERDAY 24
#define DAYSPERWEEK 7
#define DAYSPERNYEAR    365
#define DAYSPERLYEAR    366
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY  ((long) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR 12

#define TM_SUNDAY   0
#define TM_MONDAY   1
#define TM_TUESDAY  2
#define TM_WEDNESDAY    3
#define TM_THURSDAY 4
#define TM_FRIDAY   5
#define TM_SATURDAY 6

#define TM_JANUARY  0
#define TM_FEBRUARY 1
#define TM_MARCH    2
#define TM_APRIL    3
#define TM_MAY      4
#define TM_JUNE     5
#define TM_JULY     6
#define TM_AUGUST   7
#define TM_SEPTEMBER    8
#define TM_OCTOBER  9
#define TM_NOVEMBER 10
#define TM_DECEMBER 11

#define TM_YEAR_BASE    1900

#define EPOCH_YEAR  1970
#define EPOCH_WDAY  TM_THURSDAY

/*
 ** Accurate only for the past couple of centuries;
 ** that will probably do.
 */

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

#define EAT8  8


static const char   gmt[] = "GMT";

struct ttinfo {             /* time type information */
    long        tt_gmtoff;  /* GMT offset in seconds */
    int     tt_isdst;   /* used to set tm_isdst */
    int     tt_abbrind; /* abbreviation list index */
    int     tt_ttisstd; /* TRUE if transition is std time */
    int     tt_ttisgmt; /* TRUE if transition is GMT */
};

struct lsinfo {             /* leap second information */
    time_t      ls_trans;   /* transition time */
    long        ls_corr;    /* correction to apply */
};

#define BIGGEST(a, b)   (((a) > (b)) ? (a) : (b))

#define MY_TZNAME_MAX   255

struct state {
    int     leapcnt;
    int     timecnt;
    int     typecnt;
    int     charcnt;
    time_t      ats[TZ_MAX_TIMES];
    unsigned char   types[TZ_MAX_TIMES];
    struct ttinfo   ttis[TZ_MAX_TYPES];
    char        chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + 1, sizeof gmt),
            (2 * (MY_TZNAME_MAX + 1)))];
    struct lsinfo   lsis[TZ_MAX_LEAPS];
};


static const int    mon_lengths[2][MONSPERYEAR] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int    year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};


static void timesub(const time_t * const timep,
        const long              offset,
        register const struct state * const sp,
        register struct tm * const      tmp);


static void localsub(const time_t * const timep, struct tm * const tmp);

/*ARGSUSED*/
static void localsub(const time_t * const timep, struct tm * const tmp)
{
    register struct state *     sp;
    register const struct ttinfo *  ttisp;
    register int            i;
    const time_t            t = *timep;
    struct state lclmem;

    memset(&lclmem, 0, sizeof(lclmem));
#define lclptr      (&lclmem)

    sp = lclptr;

    if (sp->timecnt == 0 || t < sp->ats[0]) {
        i = 0;
        while (sp->ttis[i].tt_isdst)
            if (++i >= sp->typecnt) {
                i = 0;
                break;
            }
    } else {
        for (i = 1; i < sp->timecnt; ++i)
            if (t < sp->ats[i])
                break;
        i = sp->types[i - 1];
    }
    ttisp = &sp->ttis[i];
    /*
     ** To get (wrong) behavior that's compatible with System V Release 2.0
     ** you'd replace the statement below with
     **  t += ttisp->tt_gmtoff;
     **  timesub(&t, 0L, sp, tmp);
     */
    timesub(&t, ttisp->tt_gmtoff, sp, tmp);
    tmp->tm_isdst = ttisp->tt_isdst;
}



static void timesub(const time_t * const timep,
        const long              offset,
        register const struct state * const sp,
        register struct tm * const      tmp)
{
    register const struct lsinfo *  lp;
    register long           days;
    register long           rem;
    register int            y;
    register int            yleap;
    register const int *        ip;
    register long           corr;
    register int            hit;
    register int            i;

    corr = 0;
    hit = 0;

    i = (sp == NULL) ? 0 : sp->leapcnt;

    while (--i >= 0) {
        lp = &sp->lsis[i];
        if (*timep >= lp->ls_trans) {
            if (*timep == lp->ls_trans) {
                hit = ((i == 0 && lp->ls_corr > 0) ||
                        lp->ls_corr > sp->lsis[i - 1].ls_corr);
                if (hit)
                    while (i > 0 &&
                            sp->lsis[i].ls_trans ==
                            sp->lsis[i - 1].ls_trans + 1 &&
                            sp->lsis[i].ls_corr ==
                            sp->lsis[i - 1].ls_corr + 1) {
                        ++hit;
                        --i;
                    }
            }
            corr = lp->ls_corr;
            break;
        }
    }
    days = *timep / SECSPERDAY;
    rem  = *timep % SECSPERDAY;
#ifdef mc68k
    if (*timep == 0x80000000) {
        /*
         ** A 3B1 muffs the division on the most negative number.
         */
        days = -24855;
        rem = -11648;
    }
#endif /* defined mc68k */
    rem += (offset - corr);
    while (rem < 0) {
        rem += SECSPERDAY;
        --days;
    }
    while (rem >= SECSPERDAY) {
        rem -= SECSPERDAY;
        ++days;
    }
    tmp->tm_hour = (int) (rem / SECSPERHOUR);
    rem = rem % SECSPERHOUR;
    tmp->tm_min = (int) (rem / SECSPERMIN);
    /*
     ** A positive leap second requires a special
     ** representation.  This uses "... ??:59:60" et seq.
     */
    tmp->tm_sec = (int) (rem % SECSPERMIN) + hit;
    tmp->tm_wday = (int) ((EPOCH_WDAY + days) % DAYSPERWEEK);
    if (tmp->tm_wday < 0)
        tmp->tm_wday += DAYSPERWEEK;
    y = EPOCH_YEAR;
#define LEAPS_THRU_END_OF(y)    ((y) / 4 - (y) / 100 + (y) / 400)
    while (days < 0 || days >= (long) year_lengths[yleap = isleap(y)]) {
        register int    newy;

        newy = y + days / DAYSPERNYEAR;
        if (days < 0)
            --newy;
        days -= (newy - y) * DAYSPERNYEAR +
            LEAPS_THRU_END_OF(newy - 1) -
            LEAPS_THRU_END_OF(y - 1);
        y = newy;
    }
    tmp->tm_year = y - TM_YEAR_BASE;
    tmp->tm_yday = (int) days;
    ip = mon_lengths[yleap];
    for (tmp->tm_mon = 0; days >= (long) ip[tmp->tm_mon]; ++(tmp->tm_mon))
        days = days - (long) ip[tmp->tm_mon];
    tmp->tm_mday = (int) (days + 1);
    tmp->tm_isdst = 0;
}



/**
 *  parameter:
 *      _tz [-24, 24]
 *      e.g.: TZ=EAT-8, set _tz = 8
 *
 */
struct tm * localtime_tz(const time_t * const timep, const int _tz, struct tm *p_tm)
{
    const time_t  t = *timep + (_tz * SECSPERHOUR);

    localsub(&t, p_tm);

    return(p_tm);
}


struct tm * localtime_cn(const time_t * const  timep, struct tm *p_tm)
{
    return localtime_tz(timep, EAT8, p_tm);
}



/**
 *  parameter:
 *      _tz [-24, 24]
 *      e.g.: TZ=EAT-8, set _tz = 8
 *
 */
struct tm * dctime_tz(const int _tz, struct tm *p_tm)
{
    struct timeval tv; 

    gettimeofday(&tv, NULL);

    if(tv.tv_sec == 0)
    {
        tv.tv_sec = time(NULL) ;
    }

    const time_t  t = tv.tv_sec + (_tz * SECSPERHOUR);

    localsub(&t, p_tm);

    return(p_tm);
}


struct tm * dctime_cn(struct tm *p_tm)
{
    return dctime_tz(EAT8, p_tm);
}


/**
 *  parameter:
 *      _tv_sec: input: time in second from Epoch
 *      _fmt   : output 
 */
char *dctime_format(time_t _tv_sec, char *_fmt, int _size)
{
    struct tm m;
    const time_t t = _tv_sec + (EAT8 * SECSPERHOUR);

    localsub(&t, &m);

    snprintf(_fmt, _size, "%04d-%02d-%02d %02d:%02d:%02d",
            m.tm_year+1900, m.tm_mon+1, m.tm_mday,
            m.tm_hour, m.tm_min,   m.tm_sec);

    return _fmt;
}


/**
 *  parameter:
 *      _us     : input: time in micro second from Epoch
 *      _fmt    : output 
 */
char *dctime_format_micro(long _us, char *_fmt, int _size)
{
    time_t sec;
    long   mic;
    time_t t;
    struct tm m;

    sec = _us / 1000000;
    mic = _us % 1000000;

    t = sec + (EAT8 * SECSPERHOUR);
    localsub(&t, &m);

    snprintf(_fmt, _size, "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
            m.tm_year+1900, m.tm_mon+1, m.tm_mday,
            m.tm_hour,      m.tm_min,   m.tm_sec,
            mic);

    return _fmt;
}


#if RUN_TM
int main(int argc, char *argv[])
{
    char buffer[20] = {0};
    struct timeval tv; 

    (void)argc;
    (void)argv;

    gettimeofday(&tv, NULL);

    memset(buffer, 0, sizeof(buffer));

    dctime_format(tv.tv_sec, buffer, sizeof(buffer));

    printf("[%s.%ld]\n", buffer, tv.tv_usec);

    return 0;
}
#endif

/* tm.c */
