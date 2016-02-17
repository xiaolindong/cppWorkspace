SYSTEM     = x86-64_osx
LIBFORMAT  = static_pic
#------------------------------------------------------------
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#------------------------------------------------------------
CPLEXDIR      = /Users/Ehsan/Applications/IBM/ILOG/CPLEX_Studio1261/cplex
CONCERTDIR    = /Users/Ehsan/Applications/IBM/ILOG/CPLEX_Studio1261/concert
CURRENTDIR    = /Users/Ehsan/Documents/workspace_c++/Kemeny_project/src
BOOSTINCDIR   =  /opt/local/include
BOOSTLIBDIR   = /opt/local/lib
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
CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) -L$(BOOSTLIBDIR)
CLNDIRS   = -L$(CPLEXLIBDIR)
CCLNFLAGS = -L$(CONCERTLIBDIR) -lconcert  -L$(CPLEXLIBDIR)  -lilocplex -lcplex -m64 -lm -lpthread -framework CoreFoundation -framework IOKit  -stdlib=libstdc++ -L$(CURRENTDIR) -L$(BOOSTLIBDIR) 
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
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(CURRENTDIR) -I$(BOOSTINCDIR)
#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------
CPP_EX = kemeny
all_cpp: $(CPP_EX)
execute_cpp: $(CPP_EX)
	 ./kemeny
OBJECTS = main.o Functions.o ranking.o experiments.o triple_solver.o all_triple_formul_solver.o \
	 new_ilp_solver.o old_ilp_solver.o map_data_structure.o old_lp_solver.o heuristic_solver.o pair_search_tree.o \
	 EdgeManager.o CycleManager.o
# ------------------------------------------------------------
clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf $(C_EX) $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp
# ------------------------------------------------------------
kemeny : $(OBJECTS)
	$(CCC) $(CCFLAGS) $(OBJECTS) -o kemeny $(CCLNFLAGS)
main.o : $(CURRENTDIR)/main.cpp $(CURRENTDIR)/ranking.h $(CURRENTDIR)/new_ilp_solver.h $(CURRENTDIR)/heuristic_solver.h $(CURRENTDIR)/old_ilp_solver.h $(CURRENTDIR)/experiments.h $(CURRENTDIR)/old_lp_solver.h $(CURRENTDIR)/triple_solver.h $(CURRENTDIR)/all_triple_formul_solver.h $(CURRENTDIR)/EdgeManager.h $(CURRENTDIR)/CycleManager.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/main.cpp -o main.o
experiments.o : $(CURRENTDIR)/experiments.cpp $(CURRENTDIR)/ranking.h $(CURRENTDIR)/new_ilp_solver.h  $(CURRENTDIR)/old_ilp_solver.h $(CURRENTDIR)/old_lp_solver.h $(CURRENTDIR)/triple_solver.h $(CURRENTDIR)/all_triple_formul_solver.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/experiments.cpp -o experiments.o
ranking.o: $(CURRENTDIR)/ranking.cpp $(CURRENTDIR)/Functions.h $(CURRENTDIR)/map_data_structure.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/ranking.cpp -o ranking.o 
new_ilp_solver.o: $(CURRENTDIR)/new_ilp_solver.cpp $(CURRENTDIR)/ranking.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/new_ilp_solver.cpp -o new_ilp_solver.o
old_ilp_solver.o: $(CURRENTDIR)/old_ilp_solver.cpp $(CURRENTDIR)/ranking.h $(CURRENTDIR)/Functions.h $(CURRENTDIR)/pair_search_tree.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/old_ilp_solver.cpp -o old_ilp_solver.o
pair_search_tree.o: $(CURRENTDIR)/pair_search_tree.cpp $(CURRENTDIR)/ranking.h $(CURRENTDIR)/Functions.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/pair_search_tree.cpp -o pair_search_tree.o
heuristic_solver.o: $(CURRENTDIR)/heuristic_solver.cpp $(CURRENTDIR)/ranking.h $(CURRENTDIR)/Functions.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/heuristic_solver.cpp -o heuristic_solver.o
triple_solver.o: $(CURRENTDIR)/triple_solver.cpp $(CURRENTDIR)/ranking.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/triple_solver.cpp -o triple_solver.o
all_triple_formul_solver.o: $(CURRENTDIR)/all_triple_formul_solver.cpp $(CURRENTDIR)/ranking.h $(CURRENTDIR)/Functions.h 
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/all_triple_formul_solver.cpp -o all_triple_formul_solver.o
old_lp_solver.o: $(CURRENTDIR)/old_lp_solver.cpp $(CURRENTDIR)/ranking.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/old_lp_solver.cpp -o old_lp_solver.o
Functions.o: $(CURRENTDIR)/Functions.cpp
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/Functions.cpp -o Functions.o
EdgeManager.o: $(CURRENTDIR)/EdgeManager.cpp $(CURRENTDIR)/master_header.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/EdgeManager.cpp -o EdgeManager.o
CycleManager.o: $(CURRENTDIR)/CycleManager.cpp $(CURRENTDIR)/master_header.h
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/CycleManager.cpp -o CycleManager.o
map_data_structure.o: $(CURRENTDIR)/map_data_structure.cpp
	$(CCC) -c $(CCFLAGS) $(CURRENTDIR)/map_data_structure.cpp -o map_data_structure.o
# Local Variables:
# mode: makefile
# End:
