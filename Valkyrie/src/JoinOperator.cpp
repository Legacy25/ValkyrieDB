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
            cout << "JOIN DEBUG ====\nLEFT\n" << endl;
            for(auto i : left) {
                cout << i->getColName() << endl;
            }
            status = 1;
            codegen::joinLeftConsume(this);
            break;
        case 1:
            cout << "\nRIGHT\n" << endl;
            for(auto i : right) {
                cout << i->getColName() << endl;
            }
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
    LeafValue *dummy = new LeafValue[schema->getAttributes().size()];
    schema->addTuple(dummy);
    codegen::joinConsume(*schema, parent);
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
            col->setColPos(e->getColPos()+(int)lm.size());
            right.push_back(col);
        } else {
            std::cout << "not found in any schema " << std::endl;
        }
    }
}

Schema* JoinOperator::mergeSchemas(Schema *lsch, Schema *rsch) {
    Schema *res = new Schema("JOIN" + lsch->getTableName() + "_" + rsch->getTableName());
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
