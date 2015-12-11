#include "../include/ProjectionOperator.h"
#include "../include/Codegen.h"
#include "../include/ExpressionParser.h"
#include "../include/Expression.h"

using namespace valkyrie;

ProjectionOperator::ProjectionOperator(std::vector<std::string> expressionList, std::vector<Operator*> children) : Operator(children){
	this->type = "PROJECT";
	this->expressions = expressionList;
	schema = children[0]->getSchema();
	ExpressionParser parser;
	bool allFound = false;
	vector<string> attrs = schema->getAttributes();
	for(int i = 0; i < expressions.size(); i++){
		if(expressions[i] == "*"){
			expressions.erase(expressions.begin()+i);
			if(!allFound)
				expressions.insert(expressions.begin()+i, attrs.begin(), attrs.end());
			allFound = true;
		}
	}
	for(int i = 0; i < expressions.size(); i++){
		std::vector<Expression*> exps = parser.parse(expressions[i]);
		projectionClauses.insert(projectionClauses.end(), exps.begin(), exps.end());
	}
}

void ProjectionOperator::updateSchema(){
	assert(projectionClauses.size() == expressions.size());
    Schema *old = codegen::getSchema();
    Schema *schema = new Schema(old->getTableName());
    schema->setTuples(old->getTuples());
	for(int i = 0; i < expressions.size(); i++){
		std::size_t pos = expressions[i].find(" AS ");
        //TODO check the random column name assignment
		std::string colName = pos == std::string::npos ? "default_" + std::to_string(i) : expressions[i].substr(0, pos);
		updateExpression(projectionClauses[i], old->getColumnMap(), old->getTableName());
		schema->addAttribute(colName, projectionClauses[i]->getDataType(), projectionClauses[i]);
	}
    codegen::setSchema(schema);
}

void ProjectionOperator::updateExpression(Expression *newExp, unordered_map<std::string, Expression *> m, string tableName) {
	ExprType t = newExp->getType();
	if(t != ExprType::COLEXPRESSION && t != ExprType::DOUBLEVALUEEXPRESSION && t != ExprType::STRINGVALUEEXPRESSION &&
			t != ExprType::LONGVALUEEXPRESSION && t != ExprType::DATEVALUEEXPRESSION){
		BinaryExpression* b = (BinaryExpression*)newExp;
		updateExpression(b->getLeftExpression(), m, tableName);
		updateExpression(b->getRightExpression(), m, tableName);
	} else if(t == ExprType::COLEXPRESSION){
		if(((ColExpression*)newExp)->getTableName() == "")
			((ColExpression*)newExp)->setTableName(tableName);
		ColExpression* col = (ColExpression*)m[((ColExpression*)newExp)->getQualifiedName()];
		((ColExpression*)newExp)->setColPos(col->getColPos());
		((ColExpression*)newExp)->setType(col->getDataType());
	}
}

void ProjectionOperator::produce(){
	children[0]->produce();
}

void ProjectionOperator::consume(){
	updateSchema();
	parent->consume();
}