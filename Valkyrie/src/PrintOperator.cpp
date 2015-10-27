#include "PrintOperator.h"

PrintOperator::PrintOperator(std::vector<std::string> expressions, std::vector<Operator*> children){
	this->type = "PRINT";
	this->expressions = expressions;
	this->children = children;
}

void PrintOperator::produce(){
}

void PrintOperator::consume(){
}