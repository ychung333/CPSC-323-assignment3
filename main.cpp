#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "parser.h"

using namespace std;

int main()
{
    string inputFileName;

    cout << "Enter input file name: ";
    cin >> inputFileName;

    ifstream fin(inputFileName);

    if (!fin)
    {
        cout << "Error: Could not open input file." << endl;
        return 1;
    }

    string outputFileName = "fout_" + inputFileName;
    ofstream fout(outputFileName);

    if (!fout)
    {
        cout << "Error: Could not create output file." << endl;
        return 1;
    }

    Lexer lexer(fin);
    Parser parser(lexer, fout, true);

    parser.parse();

    cout << "Parsing complete." << endl;
    cout << "Output written to: " << outputFileName << endl;

    fin.close();
    fout.close();

    return 0;
}
