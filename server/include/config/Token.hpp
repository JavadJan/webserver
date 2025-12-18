#ifndef TOKEN_HPP
# define TOKEN_HPP
#include <string>  

enum TokenType {
    TOKEN_WORD,
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }
    TOKEN_SEMICOLON, // ;
    TOKEN_EOF
};

struct Token{
    TokenType type;
    std::string   value;

    Token(TokenType t, const std::string& v = "")
        : type(t), value(v) {
            std::cout << "TOKEN Type: " << t <<" ,TOKEN Value: " << v << "\n";
        }
  /*   value() is the only meaningful for TOKEN_WORD 
   v = "" : This is useful because not all tokens need a value.*/
};

#endif

/* You split input into tokens based on:

Whitespace,  { , }, and ; 

Everything else becomes part of a TOKEN_WORD with value.*/