#include "Automata.hpp"
#include <iostream>

int main() {

  const int TEST_CNT = 5;

  for (int i = 0; i < TEST_CNT; ++ i) {

    std::cout << "Test #" << i << ":\n";

    Automata *a = new Automata("./tests/automatas/a" + std::to_string(i));
    std::cout << "Result for L-NFA\n";
    a->testFile("./tests/test0");
    std::cout << '\n';

    Automata *b = a->nfaFromLnfa();
    std::cout << "Result for NFA\n";
    b->testFile("./tests/test0");
    std::cout << '\n';

    Automata *c = b->dfaFromNfa();
    std::cout << "Result for DFA\n";
    c->testFile("./tests/test0");
    std::cout << '\n';

    Automata *d = c->dfaMinFromDfa();
    std::cout << "Result for M-DFA\n";
    d->testFile("./tests/test0");
    std::cout << '\n';

    std::cout << "L-NFA\n";
    std::cout << *a << "\n\n";

    std::cout << "NFA\n";
    std::cout << *b << "\n\n";

    std::cout << "DFA\n";
    std::cout << *c << "\n\n";

    std::cout << "M-DFA\n";
    std::cout << *d << "\n\n";

    delete a;
    delete b;
    delete c;
    delete d;
  }
}
