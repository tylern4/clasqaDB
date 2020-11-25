CXX = g++ -std=c++11
DEPS += -I$(JNI_HEADER_PATH) -I$(JNI_MD_HEADER_PATH)
LIBS += -L$(JVM_LIB_PATH) -ljvm

FLAGS = -g -Wno-deprecated -fPIC -m64 -fno-inline -Wno-write-strings
SOURCES := $(basename $(wildcard *.cpp))
EXES := $(addsuffix .exe, $(SOURCES))


#--------------------------------------------


all: 
	make exe

exe: $(EXES)

%.exe: %.o
	@echo "--- make executable $@"
	$(CXX) -o $@ $< $(LIBS)

%.o: %.cpp
	@echo "----- build $@ -----"
	$(CXX) -c $^ -o $@ $(FLAGS) $(DEPS)

clean:
	@echo "----- clean -----"
	$(RM) $(EXES)
