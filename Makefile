
TARGET = bench

CXXSOURCES  = exceptions-bench.cpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXASM = $(CXXSOURCES:.cpp=.asm)
CXXFLAGS = $(INCLUDEDIRS) -g -O2 -Wall -fno-inline
#CXXFLAGS = $(INCLUDEDIRS) -g -O0 -Wall -fno-inline
#CXXFLAGS = $(INCLUDEDIRS) -g -m32 -O2 -Wall -fno-inline
#CXXFLAGS = $(INCLUDEDIRS) -g -m32 -O0 -Wall -fno-inline

INCLUDEDIRS =
LIBDIRS =
LIBS = -lrt
LDFLAGS = $(LIBDIRS) $(LIBS)
#LDFLAGS = $(LIBDIRS) $(LIBS) -m32
#(includes -I, libdirs -L, libs -l)

CXX = g++
RM = rm

all: $(TARGET) asm

$(TARGET): $(CXXOBJECTS)
	$(CXX) -o ./$@ $(CXXOBJECTS) $(LDFLAGS)


$(CXXOBJECTS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) -f $(CXXOBJECTS) $(CXXASM) $(TARGET)

asm: $(CXXASM)

$(CXXASM): %.asm: %.cpp
	$(CXX) -S $(CXXFLAGS) -o $@ -c $<
