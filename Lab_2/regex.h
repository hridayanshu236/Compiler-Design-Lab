#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <vector>
#include <set>

//NFA structures
struct Transition {
    int   from;
    char  symbol;   // '\0' = epsilon
    int   to;
};

struct NFA {
    int start;
    int accept;
    std::vector<Transition> transitions;
    int stateCount;
};

// Builder: Thompson's Construction 
class NFABuilder {
public:
    NFABuilder();
    NFA build(const std::string& regex);   // entry point

private:
    int nextState;

    int  newState();
    NFA  parseExpr();
    NFA  parseTerm();
    NFA  parseFactor();
    NFA  parseAtom();

    NFA  makeChar(char c);
    NFA  makeConcat(NFA a, NFA b);
    NFA  makeUnion(NFA a, NFA b);
    NFA  makeStar(NFA a);
    NFA  makePlus(NFA a);
    NFA  makeQuestion(NFA a);

    std::string pattern;
    size_t      pos;
};

// Simulator
class RegexEngine {
public:
    // Compile the pattern
    bool compile(const std::string& pattern);

    // Match entire input string against compiled pattern
    bool match(const std::string& input);

private:
    NFA nfa;
    bool compiled = false;

    std::set<int> epsilonClosure(const std::set<int>& states);
    std::set<int> move(const std::set<int>& states, char c);
};

#endif 