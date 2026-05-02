#include "parser.h"

// =======================
// Constructor Functions
// =======================
Parser::Parser(Lexer& lex, ofstream& outFile, bool print)
    : lexer(lex), fout(outFile), printRules(print)
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
// Grammar Functions
// =======================

void Parser::parse()
{
    Rat26S();
}

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
    printProduction("<Opt Function Definitions> -> <Function Definitions> | <Empty>");

    if (currentToken.lexeme == "function")
    {
        FunctionDefinitions();
    }
    else
    {
        Empty();
    }
}

void Parser::FunctionDefinitions()
{
    printProduction("<Function Definitions> -> <Function> <Function Definitions> | <Empty>");

    if (currentToken.lexeme == "function")
    {
        Function();
        FunctionDefinitions();
    }
    else
    {
        Empty();
    }
}

void Parser::Function()
{
    printProduction("<Function> -> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>");

    match("function");
    matchType("identifier");
    match("(");
    OptParameterList();
    match(")");
    OptDeclarationList();
    Body();
}

void Parser::OptParameterList()
{
    printProduction("<Opt Parameter List> -> <Parameter List> | <Empty>");

    if (currentToken.type == "identifier")
    {
        ParameterList();
    }
    else
    {
        Empty();
    }
}

void Parser::ParameterList()
{
    printProduction("<Parameter List> -> <Parameter> | <Parameter> , <Parameter List>");

    Parameter();

    if (currentToken.lexeme == ",")
    {
        match(",");
        ParameterList();
    }
}

void Parser::Parameter()
{
    printProduction("<Parameter> -> <IDs> <Qualifier>");
    IDs();
    Qualifier();
}

void Parser::Qualifier()
{
    printProduction("<Qualifier> -> integer | boolean | real");

    if (currentToken.lexeme == "integer" ||
        currentToken.lexeme == "boolean" ||
        currentToken.lexeme == "real")
    {
        match(currentToken.lexeme);
    }
    else
    {
        error("Expected type qualifier (integer, boolean, or real)");
    }
}

void Parser::Body()
{
    printProduction("<Body> -> <Compound>");
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
    Qualifier();
    IDs();
    match(";");
}

void Parser::IDs()
{
    printProduction("<IDs> -> <Identifier> | <Identifier> , <IDs>");

    matchType("identifier");

    while (currentToken.lexeme == ",")
    {
        match(",");
        matchType("identifier");
    }
}

void Parser::StatementList()
{
    printProduction("<Statement List> -> <Statement> <Statement List> | <Empty>");

    while (currentToken.lexeme != "@" && currentToken.lexeme != "}")
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
    matchType("identifier");
    match("=");
    Expression();
    match(";");
}

void Parser::If()
{
    printProduction("<If> -> if ( <Condition> ) <Statement> <If Prime>");

    match("if");
    match("(");
    Condition();
    match(")");
    Statement();
    IfPrime();
}

void Parser::IfPrime()
{
    if (currentToken.lexeme == "otherwise")
    {
        printProduction("<If Prime> -> otherwise <Statement> fi");
        match("otherwise");
        Statement();
        match("fi");
    }
    else if (currentToken.lexeme == "fi")
    {
        printProduction("<If Prime> -> fi");
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
    match(")");
    match(";");
}

void Parser::Scan()
{
    printProduction("<Scan> -> read ( <IDs> ) ;");
    match("read");
    match("(");
    IDs();
    match(")");
    match(";");
}

void Parser::While()
{
    printProduction("<While> -> while ( <Condition> ) <Statement>");
    match("while");
    match("(");
    Condition();
    match(")");
    Statement();
}

void Parser::Condition()
{
    printProduction("<Condition> -> <Expression> <Relop> <Expression>");
    Expression();
    Relop();
    Expression();
}

void Parser::Relop()
{
    printProduction("<Relop> -> == | != | > | < | >= | <=");

    if (currentToken.lexeme == "==" ||
        currentToken.lexeme == "!=" ||
        currentToken.lexeme == ">"  ||
        currentToken.lexeme == "<"  ||
        currentToken.lexeme == ">=" ||
        currentToken.lexeme == "<=")
    {
        advance();
    }
    else
    {
        error("Expected relational operator");
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
        ExpressionPrime();
    }
    else if (currentToken.lexeme == "-")
    {
        printProduction("<Expression Prime> -> - <Term> <Expression Prime>");
        match("-");
        Term();
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
        TermPrime();
    }
    else if (currentToken.lexeme == "/")
    {
        printProduction("<Term Prime> -> / <Factor> <Term Prime>");
        match("/");
        Factor();
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
        Primary();
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
        printProduction("<Primary> -> <Identifier> <Primary Prime>");
        matchType("identifier");
        PrimaryPrime();
    }
    else if (currentToken.type == "integer")
    {
        printProduction("<Primary> -> <Integer>");
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
        matchType("real");
    }
    else if (currentToken.lexeme == "true")
    {
        printProduction("<Primary> -> true");
        match("true");
    }
    else if (currentToken.lexeme == "false")
    {
        printProduction("<Primary> -> false");
        match("false");
    }
    else
    {
        error("Expected primary (identifier, integer, real, true, false, or parenthesized expression)");
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
