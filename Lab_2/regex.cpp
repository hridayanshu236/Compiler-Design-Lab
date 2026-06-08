#include "regex.h"
#include <stdexcept>
#include <queue>

// NFABuilder
NFABuilder::NFABuilder() : nextState(0), pos(0) {}

int NFABuilder::newState() { return nextState++; }

NFA NFABuilder::makeChar(char c) {
    NFA n;
    n.stateCount = nextState;
    n.start  = newState();
    n.accept = newState();
    n.transitions.push_back({n.start, c, n.accept});
    return n;
}
NFA NFABuilder::makeConcat(NFA a, NFA b) {
    // Connect a's accept to b's start via epsilon
    a.transitions.insert(a.transitions.end(),
                         b.transitions.begin(), b.transitions.end());
    a.transitions.push_back({a.accept, '\0', b.start});
    a.accept = b.accept;
    return a;
}
NFA NFABuilder::makeUnion(NFA a, NFA b) {
    NFA n;
    n.start  = newState();
    n.accept = newState();
    // Merge transitions
    n.transitions = a.transitions;
    n.transitions.insert(n.transitions.end(),
                         b.transitions.begin(), b.transitions.end());
    // Epsilon from new start to both
    n.transitions.push_back({n.start,  '\0', a.start});
    n.transitions.push_back({n.start,  '\0', b.start});
    // Epsilon from both accepts to new accept
    n.transitions.push_back({a.accept, '\0', n.accept});
    n.transitions.push_back({b.accept, '\0', n.accept});
    return n;
}
NFA NFABuilder::makeStar(NFA a) {
    NFA n;
    n.start  = newState();
    n.accept = newState();
    n.transitions = a.transitions;
    n.transitions.push_back({n.start,  '\0', a.start});
    n.transitions.push_back({n.start,  '\0', n.accept});
    n.transitions.push_back({a.accept, '\0', a.start});
    n.transitions.push_back({a.accept, '\0', n.accept});
    return n;
}
NFA NFABuilder::makePlus(NFA a) {
    // A+ = A · A*
    NFA star = makeStar(a);
    // We need a fresh copy of a's structure — re-parse isn't ideal,
    // so we record transitions directly
    NFA n;
    n.start  = newState();
    n.accept = star.accept;
    n.transitions = a.transitions;
    n.transitions.insert(n.transitions.end(),
                         star.transitions.begin(), star.transitions.end());
    n.transitions.push_back({n.start,  '\0', a.start});
    n.transitions.push_back({a.accept, '\0', star.start});
    return n;
}
NFA NFABuilder::makeQuestion(NFA a) {
    // A? = A | ε
    NFA n;
    n.start  = newState();
    n.accept = newState();
    n.transitions = a.transitions;
    n.transitions.push_back({n.start,  '\0', a.start});
    n.transitions.push_back({n.start,  '\0', n.accept});   // skip path
    n.transitions.push_back({a.accept, '\0', n.accept});
    return n;
}
//Recursive descent parser 
// Grammar:
//   expr   = term ('|' term)*
//   term   = factor factor*
//   factor = atom ('*' | '+' | '?')?
//   atom   = char | '(' expr ')'

NFA NFABuilder::parseAtom() {
    if (pos >= pattern.size())
        throw std::runtime_error("Unexpected end of pattern");

    if (pattern[pos] == '(') {
        ++pos; // consume '('
        NFA n = parseExpr();
        if (pos >= pattern.size() || pattern[pos] != ')')
            throw std::runtime_error("Missing closing ')'");
        ++pos; // consume ')'
        return n;
    }
    // Escape sequences
    if (pattern[pos] == '\\' && pos + 1 < pattern.size()) {
        ++pos;
        char c = pattern[pos++];
        return makeChar(c);
    }

    // Literal character (not a meta-char)
    char c = pattern[pos++];
    return makeChar(c);
}
NFA NFABuilder::parseFactor() {
    NFA atom = parseAtom();
    if (pos < pattern.size()) {
        if (pattern[pos] == '*') { ++pos; return makeStar(atom); }
        if (pattern[pos] == '+') { ++pos; return makePlus(atom); }
        if (pattern[pos] == '?') { ++pos; return makeQuestion(atom); }
    }
    return atom;
}
NFA NFABuilder::parseTerm() {
    NFA result = parseFactor();
    // Concatenate as long as there's something that's not '|' or ')'
    while (pos < pattern.size() &&
           pattern[pos] != '|' &&
           pattern[pos] != ')') {
        result = makeConcat(result, parseFactor());
    }
    return result;
}
NFA NFABuilder::parseExpr() {
    NFA result = parseTerm();
    while (pos < pattern.size() && pattern[pos] == '|') {
        ++pos;
        NFA right = parseTerm();
        result = makeUnion(result, right);
    }
    return result;
}
NFA NFABuilder::build(const std::string& regex) {
    pattern    = regex;
    pos        = 0;
    nextState  = 0;
    NFA n      = parseExpr();
    n.stateCount = nextState;
    return n;
}
//NFA simulator
std::set<int> RegexEngine::epsilonClosure(const std::set<int>& states) {
    std::set<int> closure = states;
    std::queue<int> worklist;
    for (int s : states) worklist.push(s);

    while (!worklist.empty()) {
        int cur = worklist.front(); worklist.pop();
        for (const auto& t : nfa.transitions) {
            if (t.from == cur && t.symbol == '\0') {
                if (closure.find(t.to) == closure.end()) {
                    closure.insert(t.to);
                    worklist.push(t.to);
                }
            }
        }
    }
    return closure;
}

std::set<int> RegexEngine::move(const std::set<int>& states, char c) {
    std::set<int> result;
    for (const auto& t : nfa.transitions)
        if (states.count(t.from) && t.symbol == c)
            result.insert(t.to);
    return result;
}

bool RegexEngine::compile(const std::string& pattern) {
    try {
        NFABuilder builder;
        nfa      = builder.build(pattern);
        compiled = true;
        return true;
    } catch (...) {
        compiled = false;
        return false;
    }
}

bool RegexEngine::match(const std::string& input) {
    if (!compiled) return false;

    // Start from epsilon-closure of start state
    std::set<int> current = epsilonClosure({nfa.start});

    for (char c : input) {
        current = epsilonClosure(move(current, c));
        if (current.empty()) return false;
    }

    return current.count(nfa.accept) > 0;
}