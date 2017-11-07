/* Copyright (c)  2017 Cmbc
 All rights reserved

 THIS IS UNPUBLISHED PROPRIETARY
 SOURCE CODE OF dc Systems, Inc.
 The copyright notice above does not
 evidence any actual or intended
 publication of such source code.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "aux.h"
#include "caapi.h"

#if STARRING_INTEGRATE
#include <libxml/parser.h>
#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"
#endif


#define BUFLEN      1024


