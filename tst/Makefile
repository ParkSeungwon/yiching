CXX = g++
CPPFLAGS = -I../
CXXFLAGS = -g -c -std=c++17 -fmax-errors=1
OBJ = $(patsubst %.cc, ../obj/%.t, $(wildcard *.cc))

all : $(OBJ) ../obj/catch

../obj/%.t : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

../obj/catch : catch.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
