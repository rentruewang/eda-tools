CC = g++
CCFLAGS = -O3 -std=c++20 -flto -Wall -Wextra 
TARGETS = buckets.o cells.o core.o getset.o init.o io.o helper.o main.o nets.o utils.o
BINARY = fm
FLAGS = 

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(TARGETS)
	$(CC) $(CCFLAGS) $? -o $(BINARY) $(FLAGS)

%.o: src/%.cpp
	$(CC) -c $(CCFLAGS) $< -o $@ $(FLAGS)

clean:
	rm -f $(BINARY) $(TARGETS)
