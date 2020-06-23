CXX = g++ -std=c++11
DEPS += -Irapidjson/include -Iinclude
FLAGS = -g -Wno-deprecated -fPIC -m64 -fno-inline -Wno-write-strings
SOURCES := $(basename $(wildcard *.cpp))
EXES := $(addsuffix .exe, $(SOURCES))


#--------------------------------------------


all: 
	make exe

exe: $(EXES)

%.exe: %.o
	@echo "--- make executable $@"
	$(CXX) -o $@ $<

%.o: %.cpp
	@echo "----- build $@ -----"
	$(CXX) -c $^ -o $@ $(FLAGS) $(DEPS)

clean:
	@echo "----- clean -----"
	$(RM) $(EXES)
