#include <iostream>
#include "lexer.h"
#include <vector>

int main() {
    std::string filename;
    std::cout << "Enter source file name (.txt): ";
    std::cin  >> filename;

    try {
        Lexer lexer(filename);
        std::vector<Token> tokens = lexer.tokenize();
        std::cout << "\nTotal tokens found: " << tokens.size() << "\n\n";
        Lexer::printTokens(tokens);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}