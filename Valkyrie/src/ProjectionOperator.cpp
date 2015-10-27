#include "ProjectionOperator.h"

ProjectionOperator::ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children){
	this->type = "PROJECT";
	this->expressions = expressions;
	this->children = children;
}

void ProjectionOperator::produce(){
}

void ProjectionOperator::consume(){
}