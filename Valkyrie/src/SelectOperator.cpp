#include "SelectOperator.h"
#include "Codegen.h"

using namespace valkyrie;

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "SELECT";
	this->expressions = expressions;
}

void SelectOperator::produce(){
	for(int i = 0; i < children.size(); i++)
		assert(children[i] != NULL);
	children[0]->produce();
}

void SelectOperator::consume(){
	codegen::selectConsume(expressions, parent);
}