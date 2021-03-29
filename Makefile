CXX = g++
CXXFLAGS = -O3 -std=c++11 -flto -Wall -Wextra
TARGETS = assertions.o buckets.o cells.o fm.o init.o io.o main.o nets.o
BINARY = fm
FLAGS = 

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(TARGETS)
	$(CXX) $(CXXFLAGS) $? -o $(BINARY) $(FLAGS)

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@ $(FLAGS)

clean:
	rm -f $(BINARY) $(TARGETS)
