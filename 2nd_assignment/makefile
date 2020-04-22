CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Wfatal-errors
OBJECTS = main.o Automata.o Dsu.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o main $(OBJECTS)

main.o: main.cpp Automata.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp $(LIBS)

Automata.o: Automata.cpp Automata.hpp Dsu.hpp
	$(CXX) $(CXXFLAGS) -c Automata.cpp

Dsu.o: Dsu.cpp Dsu.hpp
	$(CXX) $(CXXFLAGS) -c Dsu.cpp

run:
	./main

clean:
	rm *.o

