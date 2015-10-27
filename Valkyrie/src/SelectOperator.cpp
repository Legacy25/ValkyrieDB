#include "SelectOperator.h"

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children){
	this->type = "SELECT";
	this->expressions = expressions;
	this->children = children;
}

void SelectOperator::produce(){
}

void SelectOperator::consume(){

}