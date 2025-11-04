CXX := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -march=native
LDFLAGS :=

.PHONY: all clean

all: code

code: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f code *.o
