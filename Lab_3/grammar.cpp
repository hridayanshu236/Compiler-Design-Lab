#include "grammar.h"
#include <iostream>
#include <sstream>
#include <algorithm>
// String helpers
std::vector<std::string> Grammar::tokenize(const std::string& s) {
    std::istringstream iss(s);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok)
        tokens.push_back(tok);
    return tokens;
}
std::string Grammar::join(const std::vector<std::string>& tokens) {
    std::string s;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i)
            s += ' ';
        s += tokens[i];
    }
    return s;
}
// Generate primed non-terminal name
std::string Grammar::primedName(const std::string& nt) const {
    std::string candidate = nt + "'";
    while (productions.count(candidate))
        candidate += "'";
    return candidate;
}
// Read grammar from stdin
// Format: one production per line
// Example: A C d | b
void Grammar::readGrammar() {
    int n;
    std::cout << "Enter number of non-terminals: ";
    std::cin >> n;
    std::cin.ignore();
    std::cout << "Enter non-terminals (space separated): ";
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::string nt;
    while (iss >> nt)
        nonTerminals.push_back(nt);
    std::cout
        << "\nEnter productions (use | between alternatives, 'eps' for epsilon)\n";
    for (size_t i = 0; i < nonTerminals.size(); ++i) {
        const std::string& lhs = nonTerminals[i];
        std::cout << lhs << " -> ";
        std::getline(std::cin, line);
        std::vector<std::vector<std::string> > alts;
        std::istringstream altStream(line);
        std::string alt;
        while (std::getline(altStream, alt, '|')) {
            std::vector<std::string> tokens = tokenize(alt);

            if (!tokens.empty())
                alts.push_back(tokens);
        }
        productions[lhs] = alts;
    }
}

// Print grammar
void Grammar::printGrammar() const {
    for (size_t idx = 0; idx < nonTerminals.size(); ++idx) {
        const std::string& nt = nonTerminals[idx];

        if (!productions.count(nt))
            continue;

        std::cout << nt << " -> ";

        const std::vector<std::vector<std::string> >& alts =
            productions.at(nt);

        for (size_t i = 0; i < alts.size(); ++i) {
            if (i)
                std::cout << " | ";

            std::cout << join(alts[i]);
        }

        std::cout << "\n";
    }
}

// Substitute Aj into Ai to resolve indirect left recursion
void Grammar::substituteProductions(const std::string& ai,
                                    const std::string& aj) {
    std::vector<std::vector<std::string> >& aiProds = productions[ai];

    std::vector<std::vector<std::string> > newProds;

    for (size_t i = 0; i < aiProds.size(); ++i) {
        const std::vector<std::string>& alt = aiProds[i];

        if (!alt.empty() && alt[0] == aj) {
            const std::vector<std::vector<std::string> >& ajProds =
                productions[aj];

            for (size_t j = 0; j < ajProds.size(); ++j) {
                std::vector<std::string> expanded = ajProds[j];

                for (size_t k = 1; k < alt.size(); ++k)
                    expanded.push_back(alt[k]);

                newProds.push_back(expanded);
            }
        } else {
            newProds.push_back(alt);
        }
    }

    aiProds = newProds;
}
// Remove direct left recursion from a single non-terminal
void Grammar::removeDirectLR(const std::string& nt) {
    std::vector<std::vector<std::string> >& alts = productions[nt];
    std::vector<std::vector<std::string> > recursive;
    std::vector<std::vector<std::string> > nonRecursive;
    for (size_t i = 0; i < alts.size(); ++i) {
        if (!alts[i].empty() && alts[i][0] == nt)
            recursive.push_back(alts[i]);
        else
            nonRecursive.push_back(alts[i]);
    }
    if (recursive.empty())
        return;

    std::string prime = primedName(nt);

    // A -> βA'
    std::vector<std::vector<std::string> > newMain;

    for (size_t i = 0; i < nonRecursive.size(); ++i) {
        std::vector<std::string> prod = nonRecursive[i];

        if (prod.size() == 1 &&
            (prod[0] == "eps" || prod[0] == "ε")) {
            prod.clear();
            prod.push_back(prime);
        } else {
            prod.push_back(prime);
        }

        newMain.push_back(prod);
    }
    if (nonRecursive.empty())
        newMain.push_back(std::vector<std::string>(1, prime));
    // A' -> αA' | eps
    std::vector<std::vector<std::string> > newPrime;
    for (size_t i = 0; i < recursive.size(); ++i) {
        std::vector<std::string> prod(
            recursive[i].begin() + 1,
            recursive[i].end());
        prod.push_back(prime);
        newPrime.push_back(prod);
    }
    newPrime.push_back(std::vector<std::string>(1, "eps"));
    productions[nt] = newMain;
    productions[prime] = newPrime;
    std::vector<std::string>::iterator it =
        std::find(nonTerminals.begin(),
                  nonTerminals.end(),
                  nt);

    nonTerminals.insert(it + 1, prime);
}
// Remove all left recursion (indirect + direct)
void Grammar::removeLeftRecursion() {
    std::vector<std::string> original = nonTerminals;
    for (size_t i = 0; i < original.size(); ++i) {

        for (size_t j = 0; j < i; ++j)
            substituteProductions(original[i], original[j]);

        removeDirectLR(original[i]);
    }
}