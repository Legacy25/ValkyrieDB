#include "../include/SelectOperator.h"
#include "../include/Codegen.h"

using namespace valkyrie;

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "SELECT";
	this->expressions = expressions;
	ExpressionParser parser;
	selectClause = parser.parse(expressions[0])[0];
	schema = children[0]->getSchema();
	updateExpression(selectClause, schema->getColumnMap());
}

void SelectOperator::updateExpression(Expression *newExp, unordered_map<std::string, Expression *> m) {
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
			updateExpression(b->getLeftExpression(), m);
		}
		if(b->getRightExpression()->getType() == ExprType::COLEXPRESSION){
			ColExpression* col = (ColExpression*)b->getRightExpression();
			if(col->getColPos() == -1){
				assert(m.find(col->getColName()) != m.end());
				b->setRightExpression(m[col->getColName()]);
			}
		} else {
			updateExpression(b->getRightExpression(), m);
		}
	} else {
		if(t == ExprType::COLEXPRESSION){
			ColExpression* col = (ColExpression*)m[((ColExpression*)newExp)->getColName()];
			((ColExpression*)newExp)->setColPos(col->getColPos());
			((ColExpression*)newExp)->setType(col->getDataType());
		}
	}
}

void SelectOperator::produce(){
	assert(children[0] != NULL);
	children[0]->produce();
}

void SelectOperator::consume(){
	assert(parent != NULL);
	codegen::selectConsume(selectClause, parent);
}