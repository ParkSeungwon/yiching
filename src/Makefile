CXX = g++-10
PKGS = gtkmm-3.0
CPPFLAGS = -I../ $(shell pkg-config --cflags $(PKGS))
CXXFLAGS = -g -c -std=c++20 -fmax-errors=1
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ) $(OOBJ)

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

../obj/%.$(DIR).x : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
