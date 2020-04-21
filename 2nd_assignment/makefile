CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Wfatal-errors
OBJECTS = main.o Automata.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o main $(OBJECTS)

main.o: main.cpp Automata.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp $(LIBS)

run:
	./main

clean:
	rm *.o

