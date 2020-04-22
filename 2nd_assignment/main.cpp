#include "Automata.hpp"
#include <iostream>

int main() {

  const int TEST_CNT = 2;

  for (int i = 0; i < TEST_CNT; ++ i) {

    std::cout << "Test #" << i << ":\n";

    Automata *a = new Automata("./tests/automatas/a" + std::to_string(i));

    Automata *b = a->nfaFromLnfa();
    b->testFile("./tests/test0");

    /* std::cout << '\n' << *a << "\n\n" << *b << '\n'; */

    delete a;
    delete b;
  }
}
