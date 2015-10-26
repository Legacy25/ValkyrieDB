#include "SelectOperator.h"

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children){
	this->type = "SELECT";
	this->expressions = expressions;
	this->children = children;
}

Tuple SelectOperator::produce(){
	return NULL;
}

void SelectOperator::consume(Tuple tuple){

}