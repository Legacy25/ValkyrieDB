#include <iostream>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "Schema.h"
#include "Codegen.h"
#include "Parser.h"

using namespace std;


/**
 * This is the starting point of the llvm generating phase.
 * This will take as input, the query plan from Java land,
 * and output the llvm code.
 */
int main()
{
	//Parsing
	string json;
	getline(cin, json);

	valkyrie::Parser parser;
	valkyrie::Operator *root = parser.parseJson(json);
	cout << root->queryPlan() << endl;

    /* Initializations */
    codegen::initialize("LLVM");
    //Translator *root = NULL;


    /* Programatically create the AST here
     * Hardcoded for now, this is the part
     * to be done by Vinayak
     *
     * Once the AST is generated and the
     * schemas are materialized, set root
     * to the top of the tree, rest will be
     * handled automatically
     */
    /*Schema nation("nation", "data/nation.tbl");
    nation.addAttribute("nationkey", LONG);
    nation.addAttribute("name", STRING);
    nation.addAttribute("regionkey", LONG);
    nation.addAttribute("comment", STRING);

    nation.materialize();

    ScanTranslator scanTranslator(&nation);
    PrintTranslator printTranslator(&nation);
    printTranslator.setLeft(&scanTranslator);
    scanTranslator.setParent(&printTranslator);

    root = &printTranslator;*/

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