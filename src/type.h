#ifndef __DC_TYPE_H
#define __DC_TYPE_H

typedef short                   INT16;
typedef unsigned short          UINT16;
typedef char                    INT8;
typedef unsigned char           UINT8;

#if defined(_LP64) || defined(__64BIT__) || defined(__LP64__)
    #define  _OS_64BIT_
#endif  

#ifdef _OS_64BIT_
    typedef int                 INT32;
    typedef unsigned int        UINT32;
    typedef long                INT64;
    typedef unsigned long       UINT64;
#else
    typedef long                INT32;
    typedef unsigned long       UINT32;
    typedef long long           INT64;
    typedef unsigned long long  UINT64;
#endif

#ifndef LLONG_MIN
#  define LLONG_MIN         (-9223372036854775807LL -1) 
#endif

#ifndef LLONG_MAX
#  define LLONG_MAX         (9223372036854775807LL) 
#endif

#ifndef ULLONG_MAX
#  define ULLONG_MAX        (18446744073709551615ULL) 
#endif

#define FAILURE                  -1
#define SUCCESS                  0

#define TRUE                     1
#define FALSE                    0

#define YES                      '1'
#define NO                       '0'

int dc_errno;

#define DC_WORD_WIDTH           (sizeof(long))
#define dc_alignment_up(n)      ((n+DC_WORD_WIDTH-1) & (~(DC_WORD_WIDTH-1)))

#define DC_TYPEOF 		__typeof__

#endif /* __DC_TYPE_H */
