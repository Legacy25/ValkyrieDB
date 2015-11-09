#include "../include/ProjectionOperator.h"
#include "../include/Codegen.h"

using namespace valkyrie;

ProjectionOperator::ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "PROJECT";
	this->expressions = expressions;
	schema = children[0]->getSchema();
}

void ProjectionOperator::produce(){
	children[0]->produce();
}

void ProjectionOperator::consume(){
	parent->consume();
}