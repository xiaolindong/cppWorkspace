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
CURRENTDIR    = /Users/Ehsan/Documents/workspace_c++/Skill_Set_TSP
INCLUDEDIR    = $(CURRENTDIR)/include
SRCDIR        = $(CURRENTDIR)/src
OBJDIR        = $(CURRENTDIR)/obj

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------




CCC = clang++ -O0
CC  = clang -O0
JAVAC = javac




# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

#CCOPT = -m64 -O0 -fPIC -fexceptions -DNDEBUG -DIL_STD -g

CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -stdlib=libstdc++
COPT  = -m64 -fPIC
JOPT  = -classpath $(CPLEXDIR)/lib/cplex.jar -O



# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

#CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -L$(CURRENTDIR)


CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CLNDIRS   = -L$(CPLEXLIBDIR)
CCLNFLAGS = -L$(CONCERTLIBDIR) -lconcert  -L$(CPLEXLIBDIR)  -lilocplex -lcplex -m64 -lm -lpthread -framework CoreFoundation -framework IOKit -stdlib=libstdc++ -L$(CURRENTDIR)
CLNFLAGS  = -lcplex -m64 -lm -lpthread -framework CoreFoundation -framework IOKit




all:
	make all_cpp

execute: all
	make execute_cpp

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include


EXDIR         = $(CPLEXDIR)/examples
EXINC         = $(EXDIR)/include
EXDATA        = $(EXDIR)/data
EXSRCCPP      = $(EXDIR)/src/cpp



CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(CURRENTDIR) -I$(INCLUDEDIR)




#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------


CPP_EX = mtsp


all_cpp: $(CPP_EX)




execute_cpp: $(CPP_EX)
	 ./mtsp

OBJECTS = $(OBJDIR)/main.o


# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ *.class $(OBJDIR)/*.o 
	/bin/rm -rf $(C_EX) $(CURRENTDIR)/bin/* 
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

# ------------------------------------------------------------
#
# The examples
#


mtsp: $(OBJECTS)
	$(CCC) $(CCFLAGS) $(OBJECTS) -o $(CURRENTDIR)/bin/mtsp $(CCLNFLAGS)



$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/NEW_IPCUT_MST.h 
	$(CCC) -c $(CCFLAGS) $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o





# Local Variables:
# mode: makefile
# End:
