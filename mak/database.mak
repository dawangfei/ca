#|----------------------------------------------------------------------------|
#|                           TOPMake 2.0                                      |
#| Copyright (c) 2000-2004 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME     : database.ora10.mak                                         |
#|----------------------------------------------------------------------------|

DBINCP = -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/precomp/public
DBLIBP = -L$(ORACLE_HOME)/lib
DBLIBS = -lclntsh `cat $(ORACLE_HOME)/lib/sysliblist`
DBD    = _DB_ORA
DBDEF  = -DORACLE -DORA10 -D$(DBD)
DBFIX  = .pc
DBTYPE = ora

DBFLGS = $(DBLIBP) $(DBLIBS) $(DBDEF)

DBCPL  = $(DBINCP) $(DBDEF)
DBLINK = $(DBLIBP) $(DBLIBS)
