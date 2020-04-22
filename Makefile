CXX = g++
CXXFLAGS = -O3 -std=c++11
TARGETS = main.o buckets.o nets.o cells.o assertions.o init.o fm.o
FLAGS = 

.PHONY: all clean

all: $(TARGETS)
	$(CXX) $(CXXFLAGS) $? -o $@ $(FLAGS)

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(BINARY) $(TARGETS)
