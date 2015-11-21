#include "../include/PrintOperator.h"
#include "../include/Codegen.h"

using namespace valkyrie;

PrintOperator::PrintOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "PRINT";
	this->expressions = expressions;
	schema = children[0]->getSchema();
	types = (int *)&(schema->getTypes().front());
}

void PrintOperator::produce(){
	for(int i = 0; i < children.size(); i++)
		assert(children[i] != NULL);
	children[0]->produce();
}

void PrintOperator::consume(){
	codegen::printConsume(types);
}
