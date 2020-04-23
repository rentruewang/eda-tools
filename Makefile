CXX = g++
CXXFLAGS = -O3 -std=c++11
TARGETS = assertions.o buckets.o cells.o fm.o init.o io.o main.o nets.o
FLAGS = 

.PHONY: all clean

all: fm

fm: $(TARGETS)
	$(CXX) $(CXXFLAGS) $? -o $@ $(FLAGS)

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(BINARY) $(TARGETS)
