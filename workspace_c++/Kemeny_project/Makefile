SYSTEM     = x86-64_sles10_4.1
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
CURRENTDIR    = /Users/Ehsan/Documents/workspace_c++/Kemeny_project/src
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++


# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -g


# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -L$(CURRENTDIR)




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



CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(CURRENTDIR)




#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------


CPP_EX = kemeny
  


all_cpp: $(CPP_EX)




execute_cpp: $(CPP_EX)
	 ./kemeny


# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf $(C_EX) $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

# ------------------------------------------------------------
#
# The examples
#

all : kemeny

colgeneration : column_generation_voting.o election_class.o colgen.o ilp_solver.o dynamic_solver.o Functions.o
	$(CCC) $(CCFLAGS) column_generation_voting.o election_class.o colgen.o ilp_solver.o dynamic_solver.o Functions.o -o colgeneration $(CCLNFLAGS)

column_generation_voting.o: $(CURRENTDIR)/column_generation_voting.cpp $(CURRENTDIR)/election_class.h $(CURRENTDIR)/colgen.h $(CURRENTDIR)/ilp_solver.h $(CURRENTDIR)/dynamic_solver.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/column_generation_voting.cpp -o column_generation_voting.o



election_class.o: $(CURRENTDIR)/election_class.cpp $(CURRENTDIR)/Functions.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/election_class.cpp -o election_class.o $(CURRENTDIR)/Functions.o


colgen.o: $(CURRENTDIR)/colgen.cpp 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/colgen.cpp -o colgen.o


dynamic_solver.o: $(CURRENTDIR)/dynamic_solver.cpp
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/dynamic_solver.cpp -o dynamic_solver.o

ilp_solver.o: $(CURRENTDIR)/ilp_solver.cpp
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/ilp_solver.cpp -o ilp_solver.o


Functions.o: $(CURRENTDIR)/Functions.cpp
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/Functions.cpp -o Functions.o

ilocode: ilocode.o
	$(CCC) $(CCFLAGS) ilocode.o -o ilocode $(CCLNFLAGS)
ilocode.o: ~/ilocode.cpp
	$(CCC) -c $(CCFLAGS) ~/ilocode.cpp -o ilocode.o



# Local Variables:
# mode: makefile
# End:
