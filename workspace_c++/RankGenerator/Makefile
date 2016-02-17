SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic
#------------------------------------------------------------
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#------------------------------------------------------------
CURRENTDIR    = /Users/Ehsan/Documents/workspace_c++/RankGenerator
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
CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG  -stdlib=libc++
COPT  = -m64 -fPIC
JOPT  = -classpath $(CPLEXDIR)/lib/cplex.jar -O
# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------
# ------------------------------------------------------------------------
all:
	make all_cpp	
execute: all
	make execute_cpp
#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------
CPP_EX = generator
all_cpp: $(CPP_EX)
execute_cpp: $(CPP_EX)
	 ./generator
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


generator: $(OBJECTS)
	$(CCC) $(CCFLAGS) $(OBJECTS) -o $(CURRENTDIR)/bin/generator $(CCLNFLAGS)

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp 
	$(CCC) -c $(CCFLAGS) $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o

# Local Variables:
# mode: makefile
# End:
