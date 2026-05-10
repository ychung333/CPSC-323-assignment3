#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// stores token type and actual lexeme
struct Token 
{
    string type;
    string lexeme;
};

// reads input file and returns tokens
class Lexer 
{
private:
    ifstream &fin;
    char currentChar;

    void getChar();
    void skipWhitespace();
    void skipComment();

    Token identifierFSM();
    Token integerFSM();
    Token realFSM();

public:
    Lexer(ifstream& inputFile);  // constructor
    Token nextToken();      // returns next token
};

#endif
