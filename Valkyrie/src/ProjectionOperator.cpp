#include "../include/ProjectionOperator.h"
#include "../include/Codegen.h"
#include "../include/ExpressionParser.h"
#include "../include/Expression.h"

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

void ProjectionOperator::updateSchema(){
	assert(projectionClauses.size() == expressions.size());
    Schema *old = codegen::getSchema();
    Schema *schema = new Schema(old->getTableName());
    schema->setTypes(old->getTypes());
    schema->setTuples(old->getTuples());
    unordered_map<string, Expression*> nschema;
	for(int i = 0; i < expressions.size(); i++){
		std::size_t pos = expressions[i].find(" AS ");
        //TODO check the random column name assignment
		std::string colName = pos == std::string::npos ? "default_" + std::to_string(i) : expressions[i].substr(0, pos);
		updateExpesssion(projectionClauses[i], old->getColumnMap());
        nschema.insert(std::make_pair(colName, projectionClauses[i]));
	}
	schema->setColumnMap(nschema);
    codegen::setSchema(schema);
}

void ProjectionOperator::updateExpesssion(Expression *newExp, unordered_map<std::string, Expression *> m) {
	ExprType t = newExp->getType();
	if(t != ExprType::COLEXPRESSION && t != ExprType::DOUBLEVALUEEXPRESSION && t != ExprType::STRINGVALUEEXPRESSION &&
			t != ExprType::LONGVALUEEXPRESSION && t != ExprType::DATEVALUEEXPRESSION){
		BinaryExpression* b = (BinaryExpression*)newExp;
		if(b->getLeftExpression()->getType() == ExprType::COLEXPRESSION){
			ColExpression* col = (ColExpression*)b->getLeftExpression();
			//TODO check if the index is set on all leaf columns of all trees
			if(col->getColPos() == -1){
				assert(m.find(col->getColName()) != m.end());
				b->setLeftExpression(m[col->getColName()]);
			}
		} else {
			updateExpesssion(b->getLeftExpression(), m);
		}
		if(b->getRightExpression()->getType() == ExprType::COLEXPRESSION){
			ColExpression* col = (ColExpression*)b->getRightExpression();
			if(col->getColPos() == -1){
				assert(m.find(col->getColName()) != m.end());
				b->setRightExpression(m[col->getColName()]);
			}
		} else {
			updateExpesssion(b->getRightExpression(), m);
		}
	}
}

void ProjectionOperator::produce(){
	children[0]->produce();
}

void ProjectionOperator::consume(){
	updateSchema();
	parent->consume();
}