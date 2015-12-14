//
// Created by vinayak on 12/7/15.
//

#include "../include/JoinOperator.h"
#include "../include/ExpressionParser.h"
#include "../include/Codegen.h"

using namespace valkyrie;

JoinOperator::JoinOperator(std::vector<std::string> expressionList, std::vector<Operator*> children): Operator(children){
    this->type = "JOIN";
    this->expressions = expressionList;
    Schema *lsch = children[0]->getSchema(), *rsch = children[1]->getSchema();
    ExpressionParser parser;
    joinClause = parser.parse(expressions[0])[0];

    updateExpression(joinClause, lsch->getColumnMap(), rsch->getColumnMap(), lsch->getTableName(), rsch->getTableName());
    schema = mergeSchemas(lsch, rsch);
}

void JoinOperator::consume() {
    switch(status) {
        case 0:
            status = 1;
            codegen::joinLeftConsume(this);
            break;
        case 1:
            status = 2;
            codegen::joinRightConsume(this);
            break;
        default:
            cout << "Unknown status: " << status << endl;
            exit(-1);
    }
}

void JoinOperator::produce() {
    children[0]->produce();
    children[1]->produce();

    Schema *lsch = children[0]->getSchema(), *rsch = children[1]->getSchema();
    unordered_map<string, Expression*> colMap = schema->getColumnMap();
    for(unordered_map<string, Expression*>::iterator it = colMap.begin(); it != colMap.end(); it++)
        updateExpression(it->second, lsch->getColumnMap(), rsch->getColumnMap(), lsch->getTableName(), rsch->getTableName());
    codegen::joinConsume(*schema, parent);
}

void JoinOperator::updateExpression(Expression *exp, unordered_map<string, Expression*> lm, unordered_map<string, Expression*> rm
        , string ltable, string rtable) {
    ExprType t = exp->getType();
    if(t != ExprType::COLEXPRESSION && t != ExprType::DOUBLEVALUEEXPRESSION && t != ExprType::STRINGVALUEEXPRESSION &&
       t != ExprType::LONGVALUEEXPRESSION && t != ExprType::DATEVALUEEXPRESSION){
        BinaryExpression* bexp = (BinaryExpression*)exp;
        updateExpression(bexp->getLeftExpression(), lm, rm, ltable, rtable);
        updateExpression(bexp->getRightExpression(), lm, rm, ltable, rtable);
    } else if(t == COLEXPRESSION){
        ColExpression* col = (ColExpression*)exp;
        if(lm.find(col->getQualifiedName()) != lm.end()){
            ColExpression* e = (ColExpression*)lm[col->getQualifiedName()];
            col->setType(e->getDataType());
            col->setColPos(e->getColPos());
            left.push_back(col);
        } else if(rm.find(col->getQualifiedName()) != rm.end()){
            ColExpression* e = (ColExpression*)rm[col->getQualifiedName()];
            col->setType(e->getDataType());
            col->setColPos(e->getColPos() + (int)lm.size());
            ColExpression *rcol = new ColExpression(col->getQualifiedName(), col->getColPos()-(int)lm.size(), col->getDataType());
            right.push_back(rcol);
        } else {
            std::cout << "column : " << col->getQualifiedName() << " not found in any schema " << std::endl;
        }
    }
}

Schema* JoinOperator::mergeSchemas(Schema *lsch, Schema *rsch) {
    Schema *res = new Schema("JOIN_" + lsch->getTableName() + "_" + rsch->getTableName());
    std::vector<std::string> attr = lsch->getAttributes();
    for(int i = 0; i < attr.size(); i++)
        res->addAttribute(attr[i], lsch->getAttributeType(attr[i]), lsch->getAttrExpression(attr[i]));
    attr = rsch->getAttributes();
    for(int i = 0; i < attr.size(); i++)
        res->addAttribute(attr[i], rsch->getAttributeType(attr[i]), rsch->getAttrExpression(attr[i]));
    return res;
}

vector<ColExpression*> JoinOperator::getLeftJoinAttrs() {
    return left;
}

vector<ColExpression*> JoinOperator::getRightJoinAttrs() {
    return right;
}
