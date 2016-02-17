SYSTEM     = x86-64_osx
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------
CPLEXDIR      = /Users/Ehsan/Applications/IBM/ILOG/CPLEX_Studio1261/cplex
CONCERTDIR    = /Users/Ehsan/Applications/IBM/ILOG/CPLEX_Studio1261/concert
CURRENTDIR    = /Users/Ehsan/Documents/workspace_c++/vrpsss/src
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

#CCC = g++


CCC = clang++ -O0
CC  = clang -O0
JAVAC = javac


CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD  -stdlib=libstdc++
COPT  = -m64 -fPIC 
JOPT  = -classpath $(CPLEXDIR)/lib/cplex.jar -O

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)



CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CLNDIRS   = -L$(CPLEXLIBDIR)
CCLNFLAGS = -L$(CONCERTLIBDIR) -lconcert  -L$(CPLEXLIBDIR)  -lilocplex -lcplex -m64 -lm -lpthread -framework CoreFoundation -framework IOKit -stdlib=libstdc++ -L$(CURRENTDIR)
CLNFLAGS  = -lcplex -m64 -lm -lpthread -framework CoreFoundation -framework IOKit
JAVA      = java  -d64 -Djava.library.path=$(CPLEXDIR)/bin/x86-64_osx -classpath $(CPLEXJARDIR):


all:
	make all_c
	make all_cpp
	make all_java

execute: all
	make execute_c
	make execute_cpp
	make execute_java
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = $(CPLEXDIR)/examples
EXINC         = $(EXDIR)/include
EXDATA        = $(EXDIR)/data
EXSRCC        = $(EXDIR)/src/c
EXSRCCPP      = $(EXDIR)/src/cpp
EXSRCJAVA     = $(EXDIR)/src/java


CFLAGS  = $(COPT)  -I$(CPLEXINCDIR)
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 
JCFLAGS = $(JOPT)


#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------


CPP_EX = ilocode

all_c: $(C_EX)

all_cpp: $(CPP_EX)

all_java: $(JAVA_EX)
execute_cpp: $(CPP_EX)
	 ./ilocode

# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf $(C_EX) $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

# ------------------------------------------------------------
#
# The examples
#

ilocode: ilocode.o
	$(CCC) $(CCFLAGS) ilocode.o -o ilocode $(CCLNFLAGS)
ilocode.o: ilocode.cpp
	$(CCC) -c $(CCFLAGS) ilocode.cpp -o ilocode.o

# Local Variables:
# mode: makefile
# End:
