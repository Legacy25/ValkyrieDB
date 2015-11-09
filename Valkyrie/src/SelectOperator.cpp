#include "../include/SelectOperator.h"
#include "../include/Codegen.h"

using namespace valkyrie;

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "SELECT";
	this->expressions = expressions;
}

void SelectOperator::produce(){
	assert(children[0] != NULL);
	children[0]->produce();
}

void SelectOperator::consume(){
	assert(parent != NULL);
	codegen::selectConsume(expressions, parent);
}