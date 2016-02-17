################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Functions.cpp \
../src/branch_price.cpp \
../src/colgen.cpp \
../src/column_generation_voting.cpp \
../src/dynamic_solver.cpp \
../src/election_class.cpp \
../src/ilp_solver.cpp 

OBJS += \
./src/Functions.o \
./src/branch_price.o \
./src/colgen.o \
./src/column_generation_voting.o \
./src/dynamic_solver.o \
./src/election_class.o \
./src/ilp_solver.o 

CPP_DEPS += \
./src/Functions.d \
./src/branch_price.d \
./src/colgen.d \
./src/column_generation_voting.d \
./src/dynamic_solver.d \
./src/election_class.d \
./src/ilp_solver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DIL_STD -I/rcg/software/Linux/RHEL/6/x86_64/TOOLS/CPLEX/12.5/cplex/include -I/rcg/software/Linux/RHEL/6/x86_64/TOOLS/CPLEX/12.5/concert/include -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


