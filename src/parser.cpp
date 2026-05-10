#include "parser.h"

// =======================
// Constructor Functions
// =======================
Parser::Parser(Lexer& lex, ofstream& outFile, bool print)
    : lexer(lex), fout(outFile), printRules(print),
      memoryAddress(10000), instructionAddress(1)
{
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
    fout << "Error: " << message
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
        advance();
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
        advance();
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
// Main Parse
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

    // no function definitions
    if (currentToken.lexeme == "function")
    {
        error("Function definitions are not allowed in Assignment 3 simplified Rat26S");

        // so skip function definition area until the next @ so parsing can continue
        while (currentToken.lexeme != "@" && currentToken.type != "EOF")
        {
            advance();
        }
    }
    else
    {
        Empty();
    }
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

    string type = currentToken.lexeme;

    if (type == "real")
    {
        error("Real type is not allowed in Assignment 3 simplified Rat26S");
    }

    if (type == "integer" || type == "boolean" || type == "real")
    {
        advance();
    }
    else
    {
        error("Expected type qualifier");
        return;
    }

    IDsDeclaration(type);
    match(";");
}

void Parser::IDsDeclaration(const string& type)
{
    printProduction("<IDsDeclaration> -> <Identifier> | <Identifier> , <IDsDeclaration>");

    if (currentToken.type == "identifier")
    {
        insertSymbol(currentToken.lexeme, type);
        matchType("identifier");
    }
    else
    {
        error("Expected identifier in declaration");
        return;
    }

    while (currentToken.lexeme == ",")
    {
        match(",");

        if (currentToken.type == "identifier")
        {
            insertSymbol(currentToken.lexeme, type);
            matchType("identifier");
        }
        else
        {
            error("Expected identifier after comma in declaration");
            return;
        }
    }
}

void Parser::IDs()
{
    printProduction("<IDs> -> <Identifier> | <Identifier> , <IDs>");

    if (currentToken.type == "identifier")
    {
        if (!symbolExists(currentToken.lexeme))
        {
            error("Identifier '" + currentToken.lexeme + "' used but not declared");
        }

        matchType("identifier");
    }
    else
    {
        error("Expected identifier");
        return;
    }

    while (currentToken.lexeme == ",")
    {
        match(",");

        if (currentToken.type == "identifier")
        {
            if (!symbolExists(currentToken.lexeme))
            {
                error("Identifier '" + currentToken.lexeme + "' used but not declared");
            }

            matchType("identifier");
        }
        else
        {
            error("Expected identifier after comma");
            return;
        }
    }
}

vector<string> Parser::IDsList()
{
    printProduction("<IDsList> -> <Identifier> | <Identifier> , <IDsList>");

    vector<string> ids;

    if (currentToken.type == "identifier")
    {
        ids.push_back(currentToken.lexeme);
        matchType("identifier");
    }
    else
    {
        error("Expected identifier");
        return ids;
    }

    while (currentToken.lexeme == ",")
    {
        match(",");

        if (currentToken.type == "identifier")
        {
            ids.push_back(currentToken.lexeme);
            matchType("identifier");
        }
        else
        {
            error("Expected identifier after comma");
            return ids;
        }
    }

    return ids;
}

void Parser::StatementList()
{
    printProduction("<Statement List> -> <Statement> <Statement List> | <Empty>");

    while (currentToken.lexeme != "@" &&
           currentToken.lexeme != "}" &&
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
        error("Identifier '" + id + "' used but not declared");
    }

    string lhsType = getType(id);
    int addr = getAddress(id);

    matchType("identifier");
    match("=");

    string rhsType = Expression();

    if (lhsType != "unknown" && rhsType != "unknown" && lhsType != rhsType)
    {
        error("Type mismatch in assignment to '" + id + "'");
    }

    generateInstruction("POPM", to_string(addr));

    match(";");
}

void Parser::If()
{
    printProduction("<If> -> if ( <Condition> ) <Statement> <If Prime>");

    match("if");
    match("(");

    Condition();

    int falseJump = instructionAddress;
    generateInstruction("JMPZ", "");

    match(")");

    Statement();

    IfPrime(falseJump);
}

void Parser::IfPrime(int falseJump)
{
    if (currentToken.lexeme == "otherwise")
    {
        printProduction("<If Prime> -> otherwise <Statement> fi");

        int skipElseJump = instructionAddress;
        generateInstruction("JMP", "");

        backPatch(falseJump, instructionAddress);

        match("otherwise");
        Statement();

        backPatch(skipElseJump, instructionAddress);

        match("fi");
    }
    else if (currentToken.lexeme == "fi")
    {
        printProduction("<If Prime> -> fi");

        backPatch(falseJump, instructionAddress);

        match("fi");
    }
    else
    {
        error("Expected 'fi' or 'otherwise'");
    }
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

    generateInstruction("SOUT");

    match(")");
    match(";");
}

void Parser::Scan()
{
    printProduction("<Scan> -> read ( <IDs> ) ;");

    match("read");
    match("(");

    vector<string> ids = IDsList();

    for (const string& id : ids)
    {
        if (!symbolExists(id))
        {
            error("Identifier '" + id + "' used but not declared");
        }

        generateInstruction("SIN");
        generateInstruction("POPM", to_string(getAddress(id)));
    }

    match(")");
    match(";");
}

void Parser::While()
{
    printProduction("<While> -> while ( <Condition> ) <Statement>");

    match("while");

    int loopStart = instructionAddress;
    generateInstruction("LABEL");

    match("(");

    Condition();

    int falseJump = instructionAddress;
    generateInstruction("JMPZ", "");

    match(")");

    Statement();

    generateInstruction("JMP", to_string(loopStart));

    backPatch(falseJump, instructionAddress);
}

void Parser::Condition()
{
    printProduction("<Condition> -> <Expression> <Relop> <Expression>");

    string leftType = Expression();

    string op = currentToken.lexeme;
    Relop();

    string rightType = Expression();

    if (leftType != "unknown" && rightType != "unknown" && leftType != rightType)
    {
        error("Type mismatch in condition");
    }

    if (op == "<")
    {
        generateInstruction("LES");
    }
    else if (op == ">")
    {
        generateInstruction("GRT");
    }
    else if (op == "==")
    {
        generateInstruction("EQU");
    }
    else if (op == "!=")
    {
        generateInstruction("NEQ");
    }
    else if (op == ">=" || op == "=>")
    {
        generateInstruction("GEQ");
    }
    else if (op == "<=")
    {
        generateInstruction("LEQ");
    }
    else
    {
        error("Invalid relational operator");
    }
}

void Parser::Relop()
{
    printProduction("<Relop> -> == | != | > | < | >= | <= | =>");

    if (currentToken.lexeme == "==" ||
        currentToken.lexeme == "!=" ||
        currentToken.lexeme == ">"  ||
        currentToken.lexeme == "<"  ||
        currentToken.lexeme == ">=" ||
        currentToken.lexeme == "<=" ||
        currentToken.lexeme == "=>")
    {
        advance();
    }
    else
    {
        error("Expected relational operator");
    }
}

string Parser::Expression()
{
    printProduction("<Expression> -> <Term> <Expression Prime>");

    string termType = Term();
    return ExpressionPrime(termType);
}

string Parser::ExpressionPrime(string inheritedType)
{
    if (currentToken.lexeme == "+")
    {
        printProduction("<Expression Prime> -> + <Term> <Expression Prime>");

        match("+");
        string rightType = Term();

        if (inheritedType != "integer" || rightType != "integer")
        {
            error("Arithmetic operator '+' requires integer operands");
        }

        generateInstruction("A");

        return ExpressionPrime("integer");
    }
    else if (currentToken.lexeme == "-")
    {
        printProduction("<Expression Prime> -> - <Term> <Expression Prime>");

        match("-");
        string rightType = Term();

        if (inheritedType != "integer" || rightType != "integer")
        {
            error("Arithmetic operator '-' requires integer operands");
        }

        generateInstruction("S");

        return ExpressionPrime("integer");
    }
    else
    {
        printProduction("<Expression Prime> -> <Empty>");
        Empty();
        return inheritedType;
    }
}

string Parser::Term()
{
    printProduction("<Term> -> <Factor> <Term Prime>");

    string factorType = Factor();
    return TermPrime(factorType);
}

string Parser::TermPrime(string inheritedType)
{
    if (currentToken.lexeme == "*")
    {
        printProduction("<Term Prime> -> * <Factor> <Term Prime>");

        match("*");
        string rightType = Factor();

        if (inheritedType != "integer" || rightType != "integer")
        {
            error("Arithmetic operator '*' requires integer operands");
        }

        generateInstruction("M");

        return TermPrime("integer");
    }
    else if (currentToken.lexeme == "/")
    {
        printProduction("<Term Prime> -> / <Factor> <Term Prime>");

        match("/");
        string rightType = Factor();

        if (inheritedType != "integer" || rightType != "integer")
        {
            error("Arithmetic operator '/' requires integer operands");
        }

        generateInstruction("D");

        return TermPrime("integer");
    }
    else
    {
        printProduction("<Term Prime> -> <Empty>");
        Empty();
        return inheritedType;
    }
}

string Parser::Factor()
{
    printProduction("<Factor> -> - <Primary> | <Primary>");

    if (currentToken.lexeme == "-")
    {
        match("-");

        generateInstruction("PUSHI", "0");

        string primaryType = Primary();

        if (primaryType != "integer")
        {
            error("Unary minus requires integer operand");
        }

        generateInstruction("S");

        return "integer";
    }
    else
    {
        return Primary();
    }
}

string Parser::Primary()
{
    if (currentToken.type == "identifier")
    {
        printProduction("<Primary> -> <Identifier>");

        string id = currentToken.lexeme;

        if (!symbolExists(id))
        {
            error("Identifier '" + id + "' used but not declared");
        }

        string type = getType(id);
        int addr = getAddress(id);

        matchType("identifier");

        if (currentToken.lexeme == "(")
        {
            error("Function calls are not allowed in Assignment 3 simplified Rat26S");
            match("(");
            IDs();
            match(")");
            return "unknown";
        }

        generateInstruction("PUSHM", to_string(addr));

        return type;
    }
    else if (currentToken.type == "integer")
    {
        printProduction("<Primary> -> <Integer>");

        string value = currentToken.lexeme;
        generateInstruction("PUSHI", value);

        matchType("integer");

        return "integer";
    }
    else if (currentToken.lexeme == "(")
    {
        printProduction("<Primary> -> ( <Expression> )");

        match("(");
        string type = Expression();
        match(")");

        return type;
    }
    else if (currentToken.type == "real")
    {
        printProduction("<Primary> -> <Real>");

        error("Real numbers are not allowed in Assignment 3 simplified Rat26S");
        matchType("real");

        return "unknown";
    }
    else if (currentToken.lexeme == "true")
    {
        printProduction("<Primary> -> true");

        generateInstruction("PUSHI", "1");
        match("true");

        return "boolean";
    }
    else if (currentToken.lexeme == "false")
    {
        printProduction("<Primary> -> false");

        generateInstruction("PUSHI", "0");
        match("false");

        return "boolean";
    }
    else
    {
        error("Expected primary");
        advance();
        return "unknown";
    }
}

void Parser::Empty()
{
    printProduction("<Empty> -> epsilon");
}

// =======================
// Symbol Table Functions
// =======================
bool Parser::symbolExists(const string& id)
{
    for (const Symbol& s : symbolTable)
    {
        if (s.identifier == id)
        {
            return true;
        }
    }

    return false;
}

void Parser::insertSymbol(const string& id, const string& type)
{
    if (symbolExists(id))
    {
        fout << "Semantic Error: Identifier '" << id << "' already declared." << endl;
        return;
    }

    Symbol sym;
    sym.identifier = id;
    sym.memoryLocation = memoryAddress++;
    sym.type = type;

    symbolTable.push_back(sym);
}

int Parser::getAddress(const string& id)
{
    for (const Symbol& s : symbolTable)
    {
        if (s.identifier == id)
        {
            return s.memoryLocation;
        }
    }

    fout << "Semantic Error: Identifier '" << id << "' used but not declared." << endl;
    return -1;
}

string Parser::getType(const string& id)
{
    for (const Symbol& s : symbolTable)
    {
        if (s.identifier == id)
        {
            return s.type;
        }
    }

    fout << "Semantic Error: Identifier '" << id << "' used but not declared." << endl;
    return "unknown";
}

void Parser::printSymbolTable()
{
    fout << "\n=== Symbol Table ===" << endl;
    fout << "Identifier\t\tMemoryLocation\t\tType" << endl;
    fout << "-------------------------------------------" << endl;

    for (const Symbol& s : symbolTable)
    {
        fout << s.identifier << "\t\t\t"
             << s.memoryLocation << "\t\t\t"
             << s.type << endl;
    }
}

// =======================
// Assembly Code Functions
// =======================
void Parser::generateInstruction(const string& op, const string& operand)
{
    if (instructionTable.size() >= 1000)
    {
        error("Instruction table overflow. Maximum is 1000 instructions.");
        return;
    }

    Instruction instr;
    instr.address = instructionAddress++;
    instr.op = op;
    instr.operand = operand;

    instructionTable.push_back(instr);
}

void Parser::backPatch(int instructionIndex, int targetAddress)
{
    for (Instruction& instr : instructionTable)
    {
        if (instr.address == instructionIndex)
        {
            instr.operand = to_string(targetAddress);
            return;
        }
    }

    error("Backpatch failed: instruction address not found");
}

void Parser::printInstructionTable()
{
    fout << "\n=== Assembly Code ===" << endl;

    for (const Instruction& instr : instructionTable)
    {
        fout << instr.address << "\t" << instr.op;

        if (!instr.operand.empty())
        {
            fout << "\t" << instr.operand;
        }

        fout << endl;
    }
}
