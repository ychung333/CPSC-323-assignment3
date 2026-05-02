#include "lexer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_set>
using namespace std;

// Removed token struct as it was already defined in lexer.h

const unordered_set<string> KEYWORDS = 
{ 
    // using an unordered_set for quick lookup times when checking to see if a word in and because order does not matter a keyword
    "integer", "if", "otherwise", "fi", "while", "return", "read", "write", "boolean", "real", "true", "false", "function"
};

Lexer::Lexer(ifstream& inputFile) : fin(inputFile) 
{
    getChar(); // initialize the first character
}
void Lexer::getChar() 
{
    currentChar = fin.get();
}
void Lexer::skipWhitespace() 
{
    while (isspace(currentChar))
      {
        getChar();
      }
}

void Lexer::skipComment() 
{
    if (currentChar == '/' && fin.peek() == '*') 
    {
        getChar();
        getChar();

        while (!(currentChar == '*' && fin.peek() == '/')) 
        {
            getChar();
        }

        getChar();
        getChar();
    }
}

Token Lexer::identifierFSM()
{
    string lexeme = "";

    if (isalpha(currentChar)) 
    {
        lexeme += currentChar;
        getChar();
    } else 
    {
        return {"", ""};
    }
    
    while (isalnum(currentChar) || currentChar == '_') 
    {
        lexeme += currentChar;
        getChar();
    }

    if (KEYWORDS.find(lexeme) != KEYWORDS.end()) 
    {
        return {"keyword", lexeme};
    }

    return {"identifier", lexeme};
}

Token Lexer::realFSM()
{
    enum STATE {IN_INTEGER, IN_DOT, IN_REAL, ACCEPT_INTEGER, ACCEPT_REAL};
    
    STATE state = IN_INTEGER;
    string lexeme = "";

    while (true) 
    {

        switch (state) {
            case IN_INTEGER:                // Reused code from integerFSM. Can't reuse actual integerFSM
                if (isdigit(currentChar))
                {
                    lexeme += currentChar;
                    getChar();
                }
                else if (currentChar == '.')
                {
                    state = IN_DOT;
                    
                    getChar();
                }
                else 
                {
                    state = ACCEPT_INTEGER;
                    
                }
                break;

            case IN_DOT :
                if (isdigit(currentChar))
                {
                    lexeme += '.';
                    state = IN_REAL;
                    lexeme += currentChar;

                    getChar();

                }
                else
                {
                    state = ACCEPT_INTEGER;
                }
                break;

            case IN_REAL :
                if (isdigit(currentChar)) 
                {
                    lexeme += currentChar;
                    getChar();
                }
                else
                {
                    state = ACCEPT_REAL;
                }

                break;

            case ACCEPT_INTEGER :
                return {"integer", lexeme};

            case ACCEPT_REAL :
                return {"real", lexeme};
        }
    }

}

Token Lexer::integerFSM()
{
    enum STATE { IN_INTEGER, ACCEPT };

    STATE state = IN_INTEGER;
    string lexeme = "";

    while (true) 
    {
        switch (state) 
        {

            case IN_INTEGER :
                if (isdigit(currentChar)) 
                {
                    lexeme += currentChar;
                    getChar();
                } else 
                {
                    state = ACCEPT;
                }
                break;

            case ACCEPT :
                return {"integer", lexeme};
        }  
    }
}

Token Lexer::nextToken()
{
// This function will call the actual fsm.
// Don't worry about checking if the first char is a letter or digit in the FSM,
// as that will be checked in this function before calling the FSM
    skipWhitespace();
    skipComment();
    skipWhitespace();

    if (fin.eof()) 
    {
        return {"EOF", "EOF"};
    }

    if (isalpha(currentChar)) 
    {
        return identifierFSM();
    }

    if (isdigit(currentChar))
    {
        return realFSM();
    }

    string lexeme = "";

    if (currentChar == '<' || currentChar == '>' || currentChar == '!' || currentChar == '=') 
    {
        lexeme += currentChar;
        char next = fin.peek();
        if (next == '=') 
        {
            getChar();
            lexeme += currentChar;
        }
        getChar();
        return {"operator", lexeme};
    }
    lexeme += currentChar;
    getChar();

    if (lexeme == "(" || lexeme == ")" || lexeme == ";" || lexeme == "{" || lexeme == "}"
    || lexeme == "," || lexeme == "@") 
    {
        return {"separator", lexeme};
    }

    return {"operator", lexeme};

}
