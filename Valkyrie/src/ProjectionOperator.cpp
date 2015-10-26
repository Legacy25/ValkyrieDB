#include "ProjectionOperator.h"

ProjectionOperator::ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children){
	this->type = "PROJECT";
	this->expressions = expressions;
	this->children = children;
}

Tuple ProjectionOperator::produce(){
	return NULL;
}

void ProjectionOperator::consume(Tuple tuple){

}