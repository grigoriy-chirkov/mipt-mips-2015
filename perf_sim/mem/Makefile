# 
# Building the MIPS cache hit simulator
# @author Grigoriy Chirkov <grigoriy.chirkov@phystech.com>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common


# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/

#
# Enter for building cache hit sim stand alone program
#
miss_rate_sim: miss_rate_sim.o cache_tag_array.o
	$(CXX) -o $@ $^
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	$(CXX) -c $< $(INCL)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm miss_rate_sim
