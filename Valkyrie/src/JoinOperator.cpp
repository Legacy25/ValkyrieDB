//
// Created by vinayak on 12/7/15.
//

#include "../include/JoinOperator.h"
#include "../include/ExpressionParser.h"

using namespace valkyrie;

JoinOperator::JoinOperator(std::vector<std::string> expressionList, std::vector<Operator*> children): Operator(children){
    this->type = "JOIN";
    this->expressions = expressionList;
    Schema *lsch = children[0]->getSchema(), *rsch = children[1]->getSchema();
    ExpressionParser parser;
    joinClause = parser.parse(expressions[0])[0];
    updateExpression(joinClause, lsch->getColumnMap(), rsch->getColumnMap(), lsch->getTableName(), rsch->getTableName());
}

void JoinOperator::consume() {
    children[0]->consume();
    cout << "JOIN DEBUG ====\nLEFT\n" << endl;
    for(auto i : left) {
        cout << i->getColName() << endl;
    }
    cout << "\nRIGHT\n" << endl;
    for(auto i : right) {
        cout << i->getColName() << endl;
    }
}

void JoinOperator::produce() {
    children[0]->produce();
    children[1]->produce();
}

void JoinOperator::updateExpression(Expression *exp, unordered_map<string, Expression *> lm, unordered_map<string, Expression *> rm
        , string ltable, string rtable) {
    ExprType t = exp->getType();
    if(t != ExprType::COLEXPRESSION && t != ExprType::DOUBLEVALUEEXPRESSION && t != ExprType::STRINGVALUEEXPRESSION &&
       t != ExprType::LONGVALUEEXPRESSION && t != ExprType::DATEVALUEEXPRESSION){
        BinaryExpression* bexp = (BinaryExpression*)exp;
        updateExpression(bexp->getLeftExpression(), lm, rm, ltable, rtable);
        updateExpression(bexp->getRightExpression(), lm, rm, ltable, rtable);
    } else if(t == COLEXPRESSION){
        ColExpression* col = (ColExpression*)exp;
        if(lm.find(col->getColName()) != lm.end() && col->getTableName() == ltable){
            ColExpression* e = (ColExpression*)lm[col->getColName()];
            col->setType(e->getDataType());
            col->setColPos(e->getColPos());
            left.push_back(col);
        } else if(rm.find(col->getColName()) != rm.end() && col->getTableName() == rtable){
            ColExpression* e = (ColExpression*)rm[col->getColName()];
            col->setType(e->getDataType());
            col->setColPos(e->getColPos());
            right.push_back(col);
        } else {
            std::cout << "not found in any schema " << std::endl;
        }
    }
}
