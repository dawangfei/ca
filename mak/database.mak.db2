#|----------------------------------------------------------------------------|
#|                           TOPMake 2.0                                      |
#| Copyright (c) 2000-2004 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME     : database.ora10.mak                                         |
#|----------------------------------------------------------------------------|

DBINCP = -I${DB2PATH}/include
DBLIBP = -L$(DB2LIB)
DBLIBS = -ldb2
DBD    =
DBDEF  = -DDB2
DBFIX  = .sqc
DBTYPE = db2

DBFLGS = $(DBLIBP) $(DBLIBS) $(DBDEF)

DBCPL  = $(DBINCP) $(DBDEF)
DBLINK = $(DBLIBP) $(DBLIBS)
