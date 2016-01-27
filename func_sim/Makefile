# 
# Building MIPS single-cycle simulator
# @author Grigory Chirkov <grigoriy.chirkov@phystech.edu>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../

# paths to look for headers
vpath %.h $(TRUNK)/common/
vpath %.h $(TRUNK)/func_sim/
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory/ -I $(TRUNK)/func_sim/func_instr/ -I $(TRUNK)/func_sim/

#
# Enter for building funcsim stand alone program
#
funcsim: main.cpp func_sim.o func_memory.o elf_parser.o func_instr.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ $(INCL) -l elf -std=c++0x
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_sim.o: func_sim.cpp func_sim.h types.h
	$(CXX) -c $< $(INCL) -std=c++0x

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL) -std=c++0x
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL) -std=c++0x

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL) -std=c++0x

clean:
	@-rm *.o
	@-rm funcsim
