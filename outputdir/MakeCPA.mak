#########################################################################
#	MAKE file for release application
#########################################################################
# ENVIORNMENT VARIABLES 

SDK=C:\PROGRA~1\CASIO\CLASSP~1

#OPTLINKER
LNK="C:\PROGRA~1\CASIO\CLASSP~1\BIN\FXSH.EXE"

FXOPT=-o
STARTUP=$(SDK)\obj\startaplAddin.obj
SETUP=$(SDK)\obj\setup.obj
SECT=$(SDK)\obj\sectionAddin.obj

BLIB=$(SDK)\lib\bioslib.lib
LLIB=$(SDK)\lib\libc.lib
LIBGCALL=$(SDK)\lib\libgcall.lib
LIBJMPTBL=$(SDK)\lib\JmpTblApl.lib
ADDCOMMON=$(SDK)\lib\ADDCOMMON.lib
APLMAIN=E:\Anderain\git\BCE_CP\OUTPUT~1\aplmain.lib
HILIB = $(SDK)\lib\HiLIB.lib
SHCLIB = $(SDK)\lib\SHCLIBapl.lib
MAKEFILE = E:\Anderain\git\BCE_CP\OUTPUT~1\MakeCPA.mak
LNKINF0=$(BCOM_H) $(LLIB) $(BLIB) $(SHCLIB) $(STARTUP) $(MAKEFILE) $(LIBGCALL) $(LIBJMPTBL) $(ADDCOMMON) $(APLMAIN)
LNKFLAGS=-OU=E:\Anderain\git\BCE_CP\OUTPUT~1\ADDINAPL.rld -CH=warning=2310 -FO=sectrel -NOPRE -NOM -LIS=E:\Anderain\git\BCE_CP\OUTPUT~1\ADDINAPL.map -SH=SY -NOLO
ALLOBJ= $(SDK)\obj\RSYSRAM.obj $(SDK)\obj\BootINF.obj $(SDK)\obj\main.obj

#########################################################################

#########################################################################
# ALL STATEMENT.  LINKS TO CREATE ADDINAPL.RLD IN THE OBJ DIRECTORY
#########################################################################

all:	E:\Anderain\git\BCE_CP\OUTPUT~1\ADDINAPL.rld

E:\Anderain\git\BCE_CP\OUTPUT~1\ADDINAPL.rld : $(ALLOBJ) $(LNKINF0)
	$(LNK) $(FXOPT) $(STARTUP) $(SETUP) $(SECT) $(ALLOBJ) -LIB="$(BLIB), $(LLIB), $(HILIB), $(SHCLIB), $(LIBGCALL), $(LIBJMPTBL), $(ADDCOMMON), $(APLMAIN)" $(LNKFLAGS)

