CC = g++
CC_FLAGS = -O3
CC_STANDARD = -std=c++11
BINARY = fm
FILES = src/main.cpp


.PHONY: all release_silent release debug debug_fast
all: release_silent

release:
	$(CC) $(CC_FLAGS) -Wall $(CC_STANDARD) $(FILES) -o $(BINARY) -DNDEBUG

release_silent:
	$(CC) $(CC_FLAGS) -Wall $(CC_STANDARD) $(FILES) -o $(BINARY) -DNDEBUG -DSILENT
	
debug:
	$(CC) -g -rdynamic -Wall $(CC_STANDARD) $(FILES) -o $(BINARY)

debug_fast:
	$(CC) -O3 -Wall $(CC_STANDARD) $(FILES) -o $(BINARY)

clean:
	rm -f $(BINARY)
