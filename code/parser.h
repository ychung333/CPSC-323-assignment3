#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include "lexer.h"

using namespace std;

// =======================
// Symbol Table Entry
// =======================
struct Symbol
{
    string identifier;
    int memoryLocation;
    string type;
};

// =======================
// Instruction Table Entry
// =======================
struct Instruction
{
    int address;
    string op;
    string operand;
};

class Parser
{
private:
    Lexer& lexer;
    Token currentToken;
    ofstream& fout;
    bool printRules;

    // =======================
    // Assignment 3 Data
    // =======================
    vector<Symbol> symbolTable;
    vector<Instruction> instructionTable;
    stack<int> jumpStack;

    int memoryAddress;
    int instructionAddress;

    // =======================
    // Helper Functions
    // =======================
    void advance();
    void error(const string& message);
    void match(const string& expectedLexeme);
    void matchType(const string& expectedType);
    void printProduction(const string& rule);

    // =======================
    // Symbol Table Functions
    // =======================
    bool symbolExists(const string& id);
    int getAddress(const string& id);
    string getType(const string& id);
    void insertSymbol(const string& id, const string& type);
    void printSymbolTable();

    // =======================
    // Assembly Code Functions
    // =======================
    void generateInstruction(const string& op, const string& operand = "");
    void backPatch(int instructionIndex, int targetAddress);
    void printInstructionTable();

    // =======================
    // Grammar Functions
    // =======================
    void Rat26S();
    void OptFunctionDefinitions();
    void FunctionDefinitions();
    void Function();
    void OptParameterList();
    void ParameterList();
    void Parameter();
    string Qualifier();
    void Body();
    void OptDeclarationList();
    void DeclarationList();
    void Declaration();
    void IDs();
    void IDsDeclaration(const string& type);
    vector<string> IDsList();

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
    string Relop();
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
