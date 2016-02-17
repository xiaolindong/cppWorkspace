SYSTEM     = x86-64_osx
LIBFORMAT  = static_pic
#------------------------------------------------------------
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#------------------------------------------------------------
CPLEXDIR      = /Users/Ehsan/Applications/IBM/ILOG/CPLEX_Studio1261/cplex
CONCERTDIR    = /Users/Ehsan/Applications/IBM/ILOG/CPLEX_Studio1261/concert
CURRENTDIR    = /Users/Ehsan/Documents/workspace_c++/column_generation_voting/src
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------
#CCC = g++
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
JAVA      = java  -d64 -Djava.library.path=$(CPLEXDIR)/bin/x86-64_osx -classpath $(CPLEXJARDIR):
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


CPP_EX = colgeneration
  


all_cpp: $(CPP_EX)




execute_cpp: $(CPP_EX)
	 ./colgeneration

OBJECTS = column_generation_voting.o experiments.o election_class.o colgen.o\
	ufl_colgen.o cfl_colgen.o dynamic_solver.o ilp_solver.o Functions.o heuristic.o


# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf $(C_EX) $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

# ------------------------------------------------------------
#
# The examples
#

all : colgeneration


colgeneration : $(OBJECTS)
	$(CCC) $(CCFLAGS) $(OBJECTS) -o colgeneration $(CCLNFLAGS)



column_generation_voting.o: $(CURRENTDIR)/column_generation_voting.cpp $(CURRENTDIR)/election_class.h $(CURRENTDIR)/colgen.h $(CURRENTDIR)/ufl_colgen.h $(CURRENTDIR)/cfl_colgen.h  $(CURRENTDIR)/ilp_solver.h $(CURRENTDIR)/dynamic_solver.h $(CURRENTDIR)/experiments.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/column_generation_voting.cpp -o column_generation_voting.o


experiments.o: $(CURRENTDIR)/experiments.cpp $(CURRENTDIR)/election_class.h $(CURRENTDIR)/colgen.h $(CURRENTDIR)/ufl_colgen.h $(CURRENTDIR)/cfl_colgen.h $(CURRENTDIR)/dynamic_solver.h $(CURRENTDIR)/ilp_solver.h $(CURRENTDIR)/heuristic.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/experiments.cpp -o experiments.o 




election_class.o: $(CURRENTDIR)/election_class.cpp $(CURRENTDIR)/Functions.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/election_class.cpp -o election_class.o 



colgen.o: $(CURRENTDIR)/colgen.cpp $(CURRENTDIR)/election_class.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/colgen.cpp -o colgen.o



ufl_colgen.o: $(CURRENTDIR)/ufl_colgen.cpp $(CURRENTDIR)/election_class.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/ufl_colgen.cpp -o ufl_colgen.o





cfl_colgen.o: $(CURRENTDIR)/cfl_colgen.cpp $(CURRENTDIR)/election_class.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/cfl_colgen.cpp -o cfl_colgen.o



dynamic_solver.o: $(CURRENTDIR)/dynamic_solver.cpp $(CURRENTDIR)/election_class.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/dynamic_solver.cpp -o dynamic_solver.o



ilp_solver.o: $(CURRENTDIR)/ilp_solver.cpp $(CURRENTDIR)/election_class.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/ilp_solver.cpp -o ilp_solver.o


heuristic.o: $(CURRENTDIR)/heuristic.cpp $(CURRENTDIR)/election_class.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/heuristic.cpp -o heuristic.o




Functions.o: $(CURRENTDIR)/Functions.cpp
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/Functions.cpp -o Functions.o



# Local Variables:
# mode: makefile
# End:
