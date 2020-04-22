#include "Automata.hpp"
#include <iostream>

int main() {

  const int TEST_CNT = 4;

  for (int i = 0; i < TEST_CNT; ++ i) {

    std::cout << "Test #" << i << ":\n";

    Automata *a = new Automata("./tests/automatas/a" + std::to_string(i));
    a->testFile("./tests/test0");
    std::cout << '\n';

    Automata *b = a->nfaFromLnfa();
    b->testFile("./tests/test0");
    std::cout << '\n';

    Automata *c = b->dfaFromNfa();
    c->testFile("./tests/test0");
    std::cout << '\n';

    Automata *d = c->dfaMinFromDfa();
    d->testFile("./tests/test0");
    std::cout << '\n';

    std::cout << *a << "\n\n" << *b << "\n\n" << *c << "\n\n" << *d << '\n';

    delete a;
    delete b;
    delete c;
    delete d;
  }
}
