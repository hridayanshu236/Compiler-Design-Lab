#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <string>
#include <vector>
#include <map>
// Represents a Context-Free Grammar and performs left-recursion removal
class Grammar {
public:
    // Read grammar from user input
    void readGrammar();
    // Print current productions
    void printGrammar() const;
    // Remove all left recursion (direct + indirect)
    void removeLeftRecursion();
private:
    // Ordered list of non-terminals (order matters for indirect LR)
    std::vector<std::string> nonTerminals;
    // Productions: NT -> list of alternatives (each alternative = vector of symbols)
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    // Generate a fresh primed name (A -> A', A'' if needed)
    std::string primedName(const std::string& nt) const;
    // Remove direct left recursion from a single non-terminal
    void removeDirectLR(const std::string& nt);
    // Substitute Aj into Ai productions to expose indirect LR
    void substituteProductions(const std::string& ai, const std::string& aj);
    // Split a production string "A B C" into tokens
    static std::vector<std::string> tokenize(const std::string& s);
    // Join tokens into a string
    static std::string join(const std::vector<std::string>& tokens);
};
#endif 