#include "ScanOperator.h"

ScanOperator::ScanOperator(std::vector<std::string> expressions, std::vector<Operator*> children){
	this->type = "TABLE";
	this->expressions = expressions;
	this->children = children;
}

void ScanOperator::produce(){
}

void ScanOperator::consume(){
}