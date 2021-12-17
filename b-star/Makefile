CXX = g++
CXXFLAGS = -O3 -std=c++11 -Wall
TARGETS = b-star.o boundaries.o costs.o pins.o io.o main.o nets.o sa.o
BINARY = fp
FLAGS =

.PHONY: all clean

all:
	$(MAKE) clean
	$(MAKE) $(BINARY) -j

$(BINARY): $(TARGETS)
	$(CXX) $(CXXFLAGS) $? -o $(BINARY) $(FLAGS)

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@ $(FLAGS)

clean:
	rm -f $(BINARY) $(TARGETS)
