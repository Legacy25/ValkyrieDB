#include "../include/SelectOperator.h"
#include "../include/Codegen.h"

using namespace valkyrie;

SelectOperator::SelectOperator(std::vector<std::string> expressions, std::vector<Operator *> children) : Operator(
        children) {
    this->type = "SELECT";
    this->expressions = expressions;
    ExpressionParser parser;
    selectClause = parser.parse(expressions[0])[0];
    schema = children[0]->getSchema();
    updateExpression(selectClause, schema->getColumnMap(), schema->getTableName());
}

void SelectOperator::updateExpression(Expression *newExp, unordered_map<std::string, Expression *> m, std::string tableName) {
    ExprType t = newExp->getType();
    if (t != ExprType::COLEXPRESSION && t != ExprType::DOUBLEVALUEEXPRESSION && t != ExprType::STRINGVALUEEXPRESSION &&
        t != ExprType::LONGVALUEEXPRESSION && t != ExprType::DATEVALUEEXPRESSION) {
        BinaryExpression *b = (BinaryExpression *) newExp;
        updateExpression(b->getLeftExpression(), m, tableName);
        updateExpression(b->getRightExpression(), m, tableName);
    } else if (t == ExprType::COLEXPRESSION) {
        if(((ColExpression*)newExp)->getTableName() == "")
            ((ColExpression*)newExp)->setTableName(tableName);
        ColExpression *col = (ColExpression *) m[((ColExpression *) newExp)->getQualifiedName()];
        ((ColExpression *) newExp)->setColPos(col->getColPos());
        ((ColExpression *) newExp)->setType(col->getDataType());
    }
}

void SelectOperator::produce() {
    assert(children[0] != NULL);
    children[0]->produce();
}

void SelectOperator::consume() {
    assert(parent != NULL);
    codegen::selectConsume(selectClause, parent);
}