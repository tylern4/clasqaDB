# compiler and flags
CXX = g++ -std=c++11
FLAGS = -g -Wno-deprecated -fPIC -m64 -fno-inline -Wno-write-strings

# extra flags for valgrind
FLAGS += -O0

# ROOT
DEPS = $(shell root-config --cflags)
LIBS = $(shell root-config --glibs)
LIBS += -lMinuit -lRooFitCore -lRooFit -lRooStats -lProof -lMathMore

# QADB
DEPS += -I$(QADB)/srcC/rapidjson/include -I$(QADB)/srcC/include

# shared object name and source directory
OBJNAME = TestMultiDef
LIBDIR = ../lib
OBJ := $(LIBDIR)/lib$(OBJNAME).so

# rootcling dictionary
DICT := $(OBJNAME)Dict.cxx
PCM := $(OBJNAME)Dict_rdict.pcm
LINKDEF = LinkDef.h

# source code (with $(DICT) and $(LINKDEF) moved to end of lists for rootcling)
SOURCES := $(filter-out $(DICT), $(wildcard *.cxx)) $(DICT)
HEADERS := $(filter-out $(LINKDEF), $(wildcard *.h)) $(LINKDEF)

# make library dir
$(shell mkdir -p $(LIBDIR))

#-----------------------------------------------


$(OBJ): $(DICT) $(HEADERS) $(SOURCES)
	@echo "----- build $@ -----"
	$(CXX) $(SOURCES) -shared -o $@ $(FLAGS) $(DEPS) $(LIBS)

$(DICT): $(HEADERS)
	@echo "----- generate dictionary $@ -----"
	@rootcling -f $@ -c $(DEPS) $^

clean:
	$(RM) $(OBJ) $(DICT) $(PCM)
