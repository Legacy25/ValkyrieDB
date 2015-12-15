#include <iostream>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "../include/Parser.h"
#include "../include/Codegen.h"

using namespace std;


/**
 * This is the starting point of the llvm generating phase.
 * This will take as input, the query plan from Java land,
 * and output the llvm code.
 */
int main(int argc, char** argv)
{
    if(argc == 2 && strcmp(*argv, "-nollvm")) {
        codegen::nollvm();
    }
	//Parsing
	string json;
	getline(cin, json);

	valkyrie::Parser parser;
	valkyrie::Operator *root = parser.parseJson(json);
	cout << root->queryPlan() << endl;

    /* Initializations */
    codegen::initialize("LLVM");

    /* Generate LLVM */
    assert(root != NULL);
    root->produce();

    /* Compile the generated module */
    ExecutionEngine *engine = codegen::compile();
    cout << "\n\nCompilation successful, now executing...\n\n";

    /* Get a handle to the llvm function */
    int (*executePlan)() = (int (*)())(engine->getPointerToNamedFunction("llvmStart", true));

    /* Execute it */
    executePlan();
}