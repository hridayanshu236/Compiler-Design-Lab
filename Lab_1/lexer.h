#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// Token types
enum TokenType {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_UNKNOWN
};

// A single token with its lexeme and type
struct Token {
    std::string lexeme;
    TokenType   type;
};

class Lexer {
public:
    // Load source from file path
    explicit Lexer(const std::string& filename);

    // Run the scan and return all tokens
    std::vector<Token> tokenize();

    static void printTokens(const std::vector<Token>& tokens);

private:
    std::string source;   // entire file contents
    size_t      pos;      // current scan position

    char        current() const;
    char        peek(int offset = 1) const;
    void        advance();
    void        skipWhitespace();

    Token       readNumber();
    Token       readWord();
    Token       readString();
    Token       readOperator();
    Token       readPunctuation();

    static bool isKeyword(const std::string& word);
    static std::string tokenTypeName(TokenType t);
};

#endif