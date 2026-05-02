#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"

using namespace std;

class Parser
{
private:
    Lexer& lexer;
    Token currentToken;
    ofstream& fout;
    bool printRules;

    void advance();
    void error(const string& message);
    void match(const string& expectedLexeme);
    void matchType(const string& expectedType);
    void printProduction(const string& rule);

    // Grammar functions
    void Rat26S();
    void OptFunctionDefinitions();
    void FunctionDefinitions();
    void Function();
    void OptParameterList();
    void ParameterList();
    void Parameter();
    void Qualifier();
    void Body();
    void OptDeclarationList();
    void DeclarationList();
    void Declaration();
    void IDs();
    void StatementList();
    void Statement();
    void Compound();
    void Assign();
    void If();
    void IfPrime();
    void Return();
    void Print();
    void Scan();
    void While();
    void Condition();
    void Relop();
    void Expression();
    void ExpressionPrime();
    void Term();
    void TermPrime();
    void Factor();
    void Primary();
    void PrimaryPrime();
    void Empty();

public:
    Parser(Lexer& lex, ofstream& outFile, bool print = true);
    void parse();
};

#endif
