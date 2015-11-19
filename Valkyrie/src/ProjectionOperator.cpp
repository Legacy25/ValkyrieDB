#include "../include/ProjectionOperator.h"
#include "../include/Codegen.h"
#include "../include/ExpressionParser.h"

using namespace valkyrie;

ProjectionOperator::ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "PROJECT";
	this->expressions = expressions;
	schema = children[0]->getSchema();
	ExpressionParser parser;
	for(int i = 0; i < expressions.size(); i++){
		std::vector<Expression*> exps = parser.parse(expressions[i]);
		projectionClauses.insert(projectionClauses.end(), exps.begin(), exps.end());
	}
}

void ProjectionOperator::produce(){
	children[0]->produce();
}

void ProjectionOperator::consume(){
	parent->consume();
}