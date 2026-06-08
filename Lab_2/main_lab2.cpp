#include <iostream>
#include "regex.h"

int main() {
    RegexEngine engine;
    std::string pattern, input;

    std::cout << "=== Regular Expression Validator (No Library) ===\n\n";
    std::cout << "Supported operators: | (OR), * (star), + (plus), ? (optional), () grouping\n\n";

    std::cout << "Enter regex pattern: ";
    std::cin  >> pattern;

    if (!engine.compile(pattern)) {
        std::cerr << "Invalid pattern!\n";
        return 1;
    }

    char again = 'y';
    while (again == 'y' || again == 'Y') {
        std::cout << "Enter input string to validate: ";
        std::cin  >> input;

        if (engine.match(input))
            std::cout << "Result: VALID  (\"" << input << "\" matches pattern \"" << pattern << "\")\n";
        else
            std::cout << "Result: INVALID (\"" << input << "\" does not match pattern \"" << pattern << "\")\n";

        std::cout << "Test another string? (y/n): ";
        std::cin  >> again;
    }

    return 0;
}