#include "parser.h"

// =======================
// Constructor
// =======================
Parser::Parser(Lexer& lex, ofstream& outFile, bool print)
    : lexer(lex), fout(outFile), printRules(print)
{
    memoryAddress = 10000;
    instructionAddress = 1;

    currentToken = lexer.nextToken();

    if (currentToken.type != "EOF")
    {
        fout << "Token: " << currentToken.type
             << "          Lexeme: " << currentToken.lexeme << endl;
    }
}

// =======================
// Helper Functions
// =======================
void Parser::advance()
{
    currentToken = lexer.nextToken();

    if (currentToken.type != "EOF")
    {
        fout << "Token: " << currentToken.type
             << "          Lexeme: " << currentToken.lexeme << endl;
    }
}

void Parser::error(const string& message)
{
    fout << "Syntax Error: " << message
         << " | Found token: " << currentToken.type
         << " | Lexeme: " << currentToken.lexeme << endl;
}

void Parser::match(const string& expectedLexeme)
{
    if (currentToken.lexeme == expectedLexeme)
    {
        advance();
    }
    else
    {
        error("Expected lexeme '" + expectedLexeme + "'");
    }
}

void Parser::matchType(const string& expectedType)
{
    if (currentToken.type == expectedType)
    {
        advance();
    }
    else
    {
        error("Expected token type '" + expectedType + "'");
    }
}

void Parser::printProduction(const string& rule)
{
    if (printRules)
    {
        fout << rule << endl;
    }
}

// =======================
// Symbol Table Functions
// =======================
bool Parser::symbolExists(const string& id)
{
    for (const Symbol& s : symbolTable)
    {
        if (s.identifier == id)
            return true;
    }
    return false;
}

int Parser::getAddress(const string& id)
{
    for (const Symbol& s : symbolTable)
    {
        if (s.identifier == id)
            return s.memoryLocation;
    }

    error("Identifier '" + id + "' used without declaration");
    return -1;
}

string Parser::getType(const string& id)
{
    for (const Symbol& s : symbolTable)
    {
        if (s.identifier == id)
            return s.type;
    }

    error("Identifier '" + id + "' used without declaration");
    return "";
}

void Parser::insertSymbol(const string& id, const string& type)
{
    if (symbolExists(id))
    {
        error("Identifier '" + id + "' already declared");
        return;
    }

    Symbol s;
    s.identifier = id;
    s.memoryLocation = memoryAddress;
    s.type = type;

    symbolTable.push_back(s);
    memoryAddress++;
}

void Parser::printSymbolTable()
{
    fout << endl;
    fout << "Symbol Table" << endl;
    fout << "Identifier\tMemoryLocation\tType" << endl;

    for (const Symbol& s : symbolTable)
    {
        fout << s.identifier << "\t\t"
             << s.memoryLocation << "\t\t"
             << s.type << endl;
    }
}

// =======================
// Instruction Functions
// =======================
void Parser::generateInstruction(const string& op, const string& operand)
{
    Instruction instr;
    instr.address = instructionAddress;
    instr.op = op;
    instr.operand = operand;

    instructionTable.push_back(instr);
    instructionAddress++;
}

void Parser::backPatch(int instructionIndex, int targetAddress)
{
    if (instructionIndex >= 0 && instructionIndex < (int)instructionTable.size())
    {
        instructionTable[instructionIndex].operand = to_string(targetAddress);
    }
}

void Parser::printInstructionTable()
{
    fout << endl;
    fout << "Assembly Code Listing" << endl;

    for (const Instruction& instr : instructionTable)
    {
        fout << instr.address << " " << instr.op;

        if (instr.operand != "")
        {
            fout << " " << instr.operand;
        }

        fout << endl;
    }
}

// =======================
// Parse
// =======================
void Parser::parse()
{
    Rat26S();

    printInstructionTable();
    printSymbolTable();
}

// =======================
// Grammar Functions
// =======================
void Parser::Rat26S()
{
    printProduction("<Rat26S> -> @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @");

    match("@");

    // Assignment 3 simplified Rat26S has no function definitions.
    OptFunctionDefinitions();

    match("@");
    OptDeclarationList();

    match("@");
    StatementList();

    match("@");
}

void Parser::OptFunctionDefinitions()
{
    printProduction("<Opt Function Definitions> -> <Empty>");

    if (currentToken.lexeme == "function")
    {
        error("Function definitions are not allowed in Assignment 3 simplified Rat26S");
    }

    Empty();
}

void Parser::FunctionDefinitions()
{
    Empty();
}

void Parser::Function()
{
    error("Function definitions are not allowed in Assignment 3");
}

void Parser::OptParameterList()
{
    Empty();
}

void Parser::ParameterList()
{
    Empty();
}

void Parser::Parameter()
{
    Empty();
}

string Parser::Qualifier()
{
    printProduction("<Qualifier> -> integer | boolean");

    if (currentToken.lexeme == "integer")
    {
        match("integer");
        return "integer";
    }
    else if (currentToken.lexeme == "boolean")
    {
        match("boolean");
        return "boolean";
    }
    else if (currentToken.lexeme == "real")
    {
        error("Real type is not allowed in Assignment 3");
        match("real");
        return "real";
    }
    else
    {
        error("Expected type qualifier");
        return "";
    }
}

void Parser::Body()
{
    Compound();
}

void Parser::OptDeclarationList()
{
    printProduction("<Opt Declaration List> -> <Declaration List> | <Empty>");

    if (currentToken.lexeme == "integer" ||
        currentToken.lexeme == "boolean" ||
        currentToken.lexeme == "real")
    {
        DeclarationList();
    }
    else
    {
        Empty();
    }
}

void Parser::DeclarationList()
{
    printProduction("<Declaration List> -> <Declaration> <Declaration List> | <Empty>");

    while (currentToken.lexeme == "integer" ||
           currentToken.lexeme == "boolean" ||
           currentToken.lexeme == "real")
    {
        Declaration();
    }
}

void Parser::Declaration()
{
    printProduction("<Declaration> -> <Qualifier> <IDs> ;");

    string type = Qualifier();
    IDsDeclaration(type);
    match(";");
}

void Parser::IDsDeclaration(const string& type)
{
    printProduction("<IDs> -> <Identifier> | <Identifier> , <IDs>");

    if (currentToken.type == "identifier")
    {
        string id = currentToken.lexeme;
        insertSymbol(id, type);
        matchType("identifier");
    }
    else
    {
        error("Expected identifier");
    }

    while (currentToken.lexeme == ",")
    {
        match(",");

        if (currentToken.type == "identifier")
        {
            string id = currentToken.lexeme;
            insertSymbol(id, type);
            matchType("identifier");
        }
        else
        {
            error("Expected identifier after comma");
        }
    }
}

void Parser::IDs()
{
    printProduction("<IDs> -> <Identifier> | <Identifier> , <IDs>");

    if (currentToken.type == "identifier")
    {
        string id = currentToken.lexeme;

        if (!symbolExists(id))
        {
            error("Identifier '" + id + "' used without declaration");
        }

        matchType("identifier");
    }
    else
    {
        error("Expected identifier");
    }

    while (currentToken.lexeme == ",")
    {
        match(",");

        if (currentToken.type == "identifier")
        {
            string id = currentToken.lexeme;

            if (!symbolExists(id))
            {
                error("Identifier '" + id + "' used without declaration");
            }

            matchType("identifier");
        }
        else
        {
            error("Expected identifier after comma");
        }
    }
}

vector<string> Parser::IDsList()
{
    vector<string> ids;

    if (currentToken.type == "identifier")
    {
        string id = currentToken.lexeme;

        if (!symbolExists(id))
        {
            error("Identifier '" + id + "' used without declaration");
        }

        ids.push_back(id);
        matchType("identifier");
    }
    else
    {
        error("Expected identifier");
    }

    while (currentToken.lexeme == ",")
    {
        match(",");

        if (currentToken.type == "identifier")
        {
            string id = currentToken.lexeme;

            if (!symbolExists(id))
            {
                error("Identifier '" + id + "' used without declaration");
            }

            ids.push_back(id);
            matchType("identifier");
        }
        else
        {
            error("Expected identifier after comma");
        }
    }

    return ids;
}

void Parser::StatementList()
{
    printProduction("<Statement List> -> <Statement> <Statement List> | <Empty>");

    while (currentToken.lexeme != "@" &&
           currentToken.lexeme != "}" &&
           currentToken.lexeme != "fi" &&
           currentToken.lexeme != "otherwise" &&
           currentToken.type != "EOF")
    {
        Statement();
    }
}

void Parser::Statement()
{
    printProduction("<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>");

    if (currentToken.lexeme == "{")
    {
        Compound();
    }
    else if (currentToken.type == "identifier")
    {
        Assign();
    }
    else if (currentToken.lexeme == "if")
    {
        If();
    }
    else if (currentToken.lexeme == "while")
    {
        While();
    }
    else if (currentToken.lexeme == "return")
    {
        Return();
    }
    else if (currentToken.lexeme == "write")
    {
        Print();
    }
    else if (currentToken.lexeme == "read")
    {
        Scan();
    }
    else
    {
        error("Expected start of statement");
        advance();
    }
}

void Parser::Compound()
{
    printProduction("<Compound> -> { <Statement List> }");

    match("{");
    StatementList();
    match("}");
}

void Parser::Assign()
{
    printProduction("<Assign> -> <Identifier> = <Expression> ;");

    string id = currentToken.lexeme;

    if (!symbolExists(id))
    {
        error("Identifier '" + id + "' used without declaration");
    }

    matchType("identifier");
    match("=");

    Expression();

    int address = getAddress(id);
    if (address != -1)
    {
        generateInstruction("POPM", to_string(address));
    }

    match(";");
}

void Parser::If()
{
    printProduction("<If> -> if ( <Condition> ) <Statement> fi | if ( <Condition> ) <Statement> otherwise <Statement> fi");

    match("if");
    match("(");

    Condition();

    match(")");

    generateInstruction("JMPZ", "");
    int falseJumpIndex = instructionTable.size() - 1;

    Statement();

    if (currentToken.lexeme == "otherwise")
    {
        generateInstruction("JMP", "");
        int endJumpIndex = instructionTable.size() - 1;

        backPatch(falseJumpIndex, instructionAddress);

        match("otherwise");
        Statement();
        match("fi");

        backPatch(endJumpIndex, instructionAddress);
    }
    else
    {
        match("fi");
        backPatch(falseJumpIndex, instructionAddress);
    }
}

void Parser::IfPrime()
{
    // Not used in Assignment 3 version.
}

void Parser::Return()
{
    printProduction("<Return> -> return ; | return <Expression> ;");

    match("return");

    if (currentToken.lexeme != ";")
    {
        Expression();
    }

    match(";");
}

void Parser::Print()
{
    printProduction("<Print> -> write ( <Expression> ) ;");

    match("write");
    match("(");

    Expression();

    match(")");
    match(";");

    generateInstruction("SOUT", "");
}

void Parser::Scan()
{
    printProduction("<Scan> -> read ( <IDs> ) ;");

    match("read");
    match("(");

    vector<string> ids = IDsList();

    match(")");
    match(";");

    for (const string& id : ids)
    {
        int address = getAddress(id);

        generateInstruction("SIN", "");

        if (address != -1)
        {
            generateInstruction("POPM", to_string(address));
        }
    }
}

void Parser::While()
{
    printProduction("<While> -> while ( <Condition> ) <Statement>");

    match("while");

    int loopStart = instructionAddress;

    generateInstruction("LABEL", "");

    match("(");

    Condition();

    match(")");

    generateInstruction("JMPZ", "");
    int exitJumpIndex = instructionTable.size() - 1;

    Statement();

    generateInstruction("JMP", to_string(loopStart));

    backPatch(exitJumpIndex, instructionAddress);
}

void Parser::Condition()
{
    printProduction("<Condition> -> <Expression> <Relop> <Expression>");

    Expression();

    string op = Relop();

    Expression();

    if (op == "<")
        generateInstruction("LES", "");
    else if (op == ">")
        generateInstruction("GRT", "");
    else if (op == "==")
        generateInstruction("EQU", "");
    else if (op == "!=")
        generateInstruction("NEQ", "");
    else if (op == ">=" || op == "=>")
        generateInstruction("GEQ", "");
    else if (op == "<=")
        generateInstruction("LEQ", "");
    else
        error("Invalid relational operator");
}

string Parser::Relop()
{
    printProduction("<Relop> -> == | != | > | < | >= | => | <=");

    if (currentToken.lexeme == "==" ||
        currentToken.lexeme == "!=" ||
        currentToken.lexeme == ">"  ||
        currentToken.lexeme == "<"  ||
        currentToken.lexeme == ">=" ||
        currentToken.lexeme == "=>" ||
        currentToken.lexeme == "<=")
    {
        string op = currentToken.lexeme;
        advance();
        return op;
    }
    else
    {
        error("Expected relational operator");
        return "";
    }
}

void Parser::Expression()
{
    printProduction("<Expression> -> <Term> <Expression Prime>");

    Term();
    ExpressionPrime();
}

void Parser::ExpressionPrime()
{
    if (currentToken.lexeme == "+")
    {
        printProduction("<Expression Prime> -> + <Term> <Expression Prime>");

        match("+");
        Term();

        generateInstruction("A", "");

        ExpressionPrime();
    }
    else if (currentToken.lexeme == "-")
    {
        printProduction("<Expression Prime> -> - <Term> <Expression Prime>");

        match("-");
        Term();

        generateInstruction("S", "");

        ExpressionPrime();
    }
    else
    {
        printProduction("<Expression Prime> -> <Empty>");
        Empty();
    }
}

void Parser::Term()
{
    printProduction("<Term> -> <Factor> <Term Prime>");

    Factor();
    TermPrime();
}

void Parser::TermPrime()
{
    if (currentToken.lexeme == "*")
    {
        printProduction("<Term Prime> -> * <Factor> <Term Prime>");

        match("*");
        Factor();

        generateInstruction("M", "");

        TermPrime();
    }
    else if (currentToken.lexeme == "/")
    {
        printProduction("<Term Prime> -> / <Factor> <Term Prime>");

        match("/");
        Factor();

        generateInstruction("D", "");

        TermPrime();
    }
    else
    {
        printProduction("<Term Prime> -> <Empty>");
        Empty();
    }
}

void Parser::Factor()
{
    printProduction("<Factor> -> - <Primary> | <Primary>");

    if (currentToken.lexeme == "-")
    {
        match("-");

        generateInstruction("PUSHI", "0");
        Primary();
        generateInstruction("S", "");
    }
    else
    {
        Primary();
    }
}

void Parser::Primary()
{
    if (currentToken.type == "identifier")
    {
        printProduction("<Primary> -> <Identifier>");

        string id = currentToken.lexeme;

        if (!symbolExists(id))
        {
            error("Identifier '" + id + "' used without declaration");
        }

        int address = getAddress(id);

        if (address != -1)
        {
            generateInstruction("PUSHM", to_string(address));
        }

        matchType("identifier");

        // Assignment 3 simplified version does not use function calls.
        if (currentToken.lexeme == "(")
        {
            error("Function calls are not allowed in Assignment 3 simplified Rat26S");
            PrimaryPrime();
        }
    }
    else if (currentToken.type == "integer")
    {
        printProduction("<Primary> -> <Integer>");

        string value = currentToken.lexeme;
        generateInstruction("PUSHI", value);

        matchType("integer");
    }
    else if (currentToken.lexeme == "(")
    {
        printProduction("<Primary> -> ( <Expression> )");

        match("(");
        Expression();
        match(")");
    }
    else if (currentToken.type == "real")
    {
        printProduction("<Primary> -> <Real>");

        error("Real values are not allowed in Assignment 3");
        matchType("real");
    }
    else if (currentToken.lexeme == "true")
    {
        printProduction("<Primary> -> true");

        generateInstruction("PUSHI", "1");
        match("true");
    }
    else if (currentToken.lexeme == "false")
    {
        printProduction("<Primary> -> false");

        generateInstruction("PUSHI", "0");
        match("false");
    }
    else
    {
        error("Expected primary");
        advance();
    }
}

void Parser::PrimaryPrime()
{
    if (currentToken.lexeme == "(")
    {
        printProduction("<Primary Prime> -> ( <IDs> )");

        match("(");
        IDs();
        match(")");
    }
    else
    {
        printProduction("<Primary Prime> -> <Empty>");
        Empty();
    }
}

void Parser::Empty()
{
    printProduction("<Empty> -> ε");
}
