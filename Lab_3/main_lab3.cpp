#include <iostream>
#include "grammar.h"

int main() {
    std::cout << "=== Left Recursion Removal ===\n\n";

    Grammar g;
    g.readGrammar();

    std::cout << "\n--- Original Grammar ---\n";
    g.printGrammar();

    g.removeLeftRecursion();

    std::cout << "\n--- Grammar After Left Recursion Removal ---\n";
    g.printGrammar();

    return 0;
}