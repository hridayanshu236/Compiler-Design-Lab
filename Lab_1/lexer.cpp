#include "lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <stdexcept>

//Reserved keywords
static const std::string KEYWORDS[] = {
    "int","float","double","char","bool","void","string",
    "if","else","while","for","do","switch","case","break",
    "continue","return","class","struct","public","private",
    "protected","new","delete","true","false","nullptr","const","static"
};

//Constructor: read entire file into `source`
Lexer::Lexer(const std::string& filename) : pos(0) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);
    std::ostringstream ss;
    ss << file.rdbuf();
    source = ss.str();
}

//Character helpers
char Lexer::current() const {
    return (pos < source.size()) ? source[pos] : '\0';
}
char Lexer::peek(int offset) const {
    size_t p = pos + offset;
    return (p < source.size()) ? source[p] : '\0';
}
void Lexer::advance() { if (pos < source.size()) ++pos; }

void Lexer::skipWhitespace() {
    while (pos < source.size() && std::isspace(static_cast<unsigned char>(current())))
        advance();
}

//Token readers
Token Lexer::readNumber() {
    std::string lexeme;
    bool hasDot = false;
    while (std::isdigit(static_cast<unsigned char>(current())) ||
           (current() == '.' && !hasDot)) {
        if (current() == '.') hasDot = true;
        lexeme += current();
        advance();
    }
    return {lexeme, TOKEN_NUMBER};
}

Token Lexer::readWord() {
    std::string lexeme;
    while (std::isalnum(static_cast<unsigned char>(current())) || current() == '_') {
        lexeme += current();
        advance();
    }
    TokenType t = isKeyword(lexeme) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    return {lexeme, t};
}

Token Lexer::readString() {
    std::string lexeme;
    char quote = current();   // ' or "
    lexeme += current(); advance();
    while (pos < source.size() && current() != quote) {
        if (current() == '\\') { lexeme += current(); advance(); } // escape
        lexeme += current(); advance();
    }
    if (current() == quote) { lexeme += current(); advance(); }
    return {lexeme, TOKEN_STRING};
}

Token Lexer::readOperator() {
    std::string lexeme(1, current());
    char c  = current();
    char nx = peek();
    advance();

    // Two-character operators
    if ((c == '=' && nx == '=') || (c == '!' && nx == '=') ||
        (c == '<' && nx == '=') || (c == '>' && nx == '=') ||
        (c == '&' && nx == '&') || (c == '|' && nx == '|') ||
        (c == '+' && nx == '+') || (c == '-' && nx == '-') ||
        (c == '+' && nx == '=') || (c == '-' && nx == '=') ||
        (c == '*' && nx == '=') || (c == '/' && nx == '=')) {
        lexeme += nx;
        advance();
    }
    return {lexeme, TOKEN_OPERATOR};
}

Token Lexer::readPunctuation() {
    std::string lexeme(1, current());
    advance();
    return {lexeme, TOKEN_PUNCTUATION};
}

//Keyword check
bool Lexer::isKeyword(const std::string& word) {
    for (const auto& kw : KEYWORDS)
        if (kw == word) return true;
    return false;
}

//Main tokenize loop
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < source.size()) {
        skipWhitespace();
        if (pos >= source.size()) break;

        char c = current();

        // Single-line comment
        if (c == '/' && peek() == '/') {
            while (pos < source.size() && current() != '\n') advance();
            continue;
        }
        // Multi-line comment
        if (c == '/' && peek() == '*') {
            advance(); advance();
            while (pos < source.size()) {
                if (current() == '*' && peek() == '/') { advance(); advance(); break; }
                advance();
            }
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c)))
            tokens.push_back(readNumber());
        else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
            tokens.push_back(readWord());
        else if (c == '"' || c == '\'')
            tokens.push_back(readString());
        else if (c == '+' || c == '-' || c == '*' || c == '/' ||
                 c == '=' || c == '<' || c == '>' || c == '!' ||
                 c == '&' || c == '|' || c == '%' || c == '^')
            tokens.push_back(readOperator());
        else if (c == '(' || c == ')' || c == '{' || c == '}' ||
                 c == '[' || c == ']' || c == ';' || c == ',' ||
                 c == '.' || c == ':' || c == '?' || c == '#')
            tokens.push_back(readPunctuation());
        else {
            tokens.push_back({std::string(1, c), TOKEN_UNKNOWN});
            advance();
        }
    }
    return tokens;
}

std::string Lexer::tokenTypeName(TokenType t) {
    switch (t) {
        case TOKEN_KEYWORD:     return "KEYWORD";
        case TOKEN_IDENTIFIER:  return "IDENTIFIER";
        case TOKEN_NUMBER:      return "NUMBER";
        case TOKEN_STRING:      return "STRING";
        case TOKEN_OPERATOR:    return "OPERATOR";
        case TOKEN_PUNCTUATION: return "PUNCTUATION";
        default:                return "UNKNOWN";
    }
}

void Lexer::printTokens(const std::vector<Token>& tokens) {
    std::cout << std::left
              << std::setw(25) << "LEXEME"
              << std::setw(20) << "TOKEN TYPE" << "\n";
    std::cout << std::string(45, '-') << "\n";
    for (const auto& tok : tokens)
        std::cout << std::setw(25) << tok.lexeme
                  << std::setw(20) << tokenTypeName(tok.type) << "\n";
}