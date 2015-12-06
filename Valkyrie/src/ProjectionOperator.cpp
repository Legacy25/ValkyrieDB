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
    unordered_map<string, Expression*> nschema;
	//
	vector<string> att = old->getAttributes();
	for(int i = 0; i < att.size(); i++)
		std::cout << " ;; " << att[i];
	std::cout << std::endl;
	std::cout << "----------------" << std::endl;
	for(int i = 0; i < expressions.size(); i++)
		std::cout << " ;; " << expressions[i] << " .. " << projectionClauses[i]->toString();
	std::cout << std::endl;
	//
	ExpressionParser parser;
	for(int i = 0; i < expressions.size(); i++){
		std::size_t pos = expressions[i].find(" AS ");
        //TODO check the random column name assignment
		std::string colName = pos == std::string::npos ? "default_" + std::to_string(i) : expressions[i].substr(0, pos);
		updateExpression(projectionClauses[i], old->getColumnMap());
        nschema.insert(std::make_pair(colName, projectionClauses[i]));
		schema->addAttribute(colName, parser.evaluateType(projectionClauses[i]));
	}
	schema->setColumnMap(nschema);
	//
	std::cout << "updated " << std::endl;
 	std::vector<string> a = schema->getAttributes();
	std::vector<DataType> t = schema->getTypes();
	std::unordered_map<string, Expression*> m = schema->getColumnMap();
	for(int i = 0; i < a.size(); i++)
		std::cout << " ;; " << a[i] << " // " << m[a[i]]->toString() << "//" << t[i];
	std::cout << std::endl;
	//
    codegen::setSchema(schema);
}

void ProjectionOperator::updateExpression(Expression *newExp, unordered_map<std::string, Expression *> m) {
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

void ProjectionOperator::produce(){
	children[0]->produce();
}

void ProjectionOperator::consume(){
	updateSchema();
	parent->consume();
}