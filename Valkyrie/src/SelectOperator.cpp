#include "SelectOperator.h"
#include "Codegen.h"

using namespace valkyrie;

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "SELECT";
	this->expressions = expressions;
}

void SelectOperator::produce(){
}

void SelectOperator::consume(){
}