#include <stdint.h>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "../include/Expression.h"
#include "../include/Codegen.h"

using namespace valkyrie;
using namespace llvm;

Expression *BinaryExpression::getLeftExpression() {
    return leftExpression;
}

void BinaryExpression::setLeftExpression(Expression *expression) {
    leftExpression = expression;
}

Expression *BinaryExpression::getRightExpression() {
    return rightExpression;
}

void BinaryExpression::setRightExpression(Expression *expression) {
    rightExpression = expression;
}

Value* AdditionExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateAdd(leftExpression->getValue(), rightExpression->getValue());
}

Value* SubtractionExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateSub(leftExpression->getValue(), rightExpression->getValue());
}

Value* MultiplicationExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateMul(leftExpression->getValue(), rightExpression->getValue());
}

Value* DivisionExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateFDiv(leftExpression->getValue(), rightExpression->getValue());
}

Value* AndExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateAnd(leftExpression->getValue(), rightExpression->getValue());
}

Value* OrExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateOr(leftExpression->getValue(), rightExpression->getValue());
}

/**
 * TODO The comparisions will be type dependent, now only works for longs and doubles
 * ICmp is for integers (long)
 * For doubles, need to use FCmp
 * For strings, need to iterate over the ConstantArray and compare each element
 * using getelementptr and invoking ICmp on them
 */

static ExprType convertDTtoET(DataType dt) {
    switch(dt) {
        case LONG:
            return LONGVALUEEXPRESSION;
        case DOUBLE:
            return DOUBLEVALUEEXPRESSION;
        case DATE:
            return DATEVALUEEXPRESSION;
        case STRING:
            return STRINGVALUEEXPRESSION;
        default:
            cout << "Unknown dt to et conversion!" << endl;
            exit(-1);
    }
}

Value* EqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());
    IRBuilder<>* builder = codegen::getBuilder();
    int c;
    if(leftExpression->getType() != COLEXPRESSION) {
        c = leftExpression->getType();
    } else {
        c = convertDTtoET(leftExpression->getDataType());
    }
    switch(c) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpEQ(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOEQ(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            return codegen::stringCmp(leftExpression->getValue(), rightExpression->getValue(), EQUALEXPRESSION);
        default:
            cout << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* NotEqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    int c;
    if(leftExpression->getType() != COLEXPRESSION) {
        c = leftExpression->getType();
    } else {
        c = convertDTtoET(leftExpression->getDataType());
    }
    switch(c) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpNE(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpONE(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            return codegen::stringCmp(leftExpression->getValue(), rightExpression->getValue(), NOTEQUALEXPRESSION);
        default:
            cout << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* GreaterThanExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    int c;
    if(leftExpression->getType() != COLEXPRESSION) {
        c = leftExpression->getType();
    } else {
        c = convertDTtoET(leftExpression->getDataType());
    }
    switch(c) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSGT(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOGT(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            return codegen::stringCmp(leftExpression->getValue(), rightExpression->getValue(), GREATERTHANEXPRESSION);
        default:
            cout << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* GreaterThanEqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    int c;
    if(leftExpression->getType() != COLEXPRESSION) {
        c = leftExpression->getType();
    } else {
        c = convertDTtoET(leftExpression->getDataType());
    }
    switch(c) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSGE(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOGE(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            return codegen::stringCmp(leftExpression->getValue(), rightExpression->getValue(), GREATERTHANEQUALEXPRESSION);
        default:
            cout << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* LessThanExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    int c;
    if(leftExpression->getType() != COLEXPRESSION) {
        c = leftExpression->getType();
    } else {
        c = convertDTtoET(leftExpression->getDataType());
    }
    switch(c) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSLT(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOLT(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            return codegen::stringCmp(leftExpression->getValue(), rightExpression->getValue(), LESSTHANEXPRESSION);
        default:
            cout << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* LessThanEqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    int c;
    if(leftExpression->getType() != COLEXPRESSION) {
        c = leftExpression->getType();
    } else {
        c = convertDTtoET(leftExpression->getDataType());
    }
    switch(c) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSLE(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOLE(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            return codegen::stringCmp(leftExpression->getValue(), rightExpression->getValue(), LESSTHANEQUALEXPRESSION);
        default:
            cout << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

LongValueExpression::LongValueExpression(long data){
    this->data = data;
}

Value* LongValueExpression::getValue() {
    Type* int64Ty = Type::getInt64Ty(getGlobalContext());
    return ConstantInt::get(int64Ty, data, true);
}

DoubleValueExpression::DoubleValueExpression(double data){
    this->data = data;
}

Value* DoubleValueExpression::getValue() {
    Type* doubleTy = Type::getDoubleTy(getGlobalContext());
    return ConstantFP::get(doubleTy, data);
}

StringValueExpression::StringValueExpression(char* data){
    this->data = data;
}

Value* StringValueExpression::getValue() {
    Type* int64Ty = Type::getInt64Ty(getGlobalContext());
    return ConstantInt::get(int64Ty, (uint64_t) data, true);
}

Value* DateValueExpression::getValue() {
    Type* int64Ty = Type::getInt64Ty(getGlobalContext());
    return ConstantInt::get(int64Ty, (uint64_t) data, true);
}

ColExpression::ColExpression(std::string name){
    setColName(name);
}

ColExpression::ColExpression(string name, int colPos) {
    setColName(name);
    index = colPos;
}

ColExpression::ColExpression(string name, int colPos, DataType type) {
    setColName(name);
    this->index = colPos;
    this->type = type;
}

void ColExpression::setColPos(int pos){
    this->index = pos;
}

int ColExpression::getColPos(){
    return this->index;
}

void ColExpression::setType(DataType type){
    this->type = type;
}
DataType ColExpression::getDataType(){
    return this->type;
}

string ColExpression::getQualifiedName() {
    return tablename + "." + colname;
}

void ColExpression::setTableName(string tablename) {
    this->tablename = tablename;
}

string ColExpression::getTableName() {
    return tablename;
}

string ColExpression::getColName() {
    return this->colname;
}

void ColExpression::setColName(string name) {
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    size_t pos = name.find(".");
    if(pos != std::string::npos){
        colname = name.substr(pos+1);
        tablename = name.substr(0, pos);
    } else{
        tablename = "";
        colname = name;
    }
}

Value* ColExpression::getValue() {
    IRBuilder<>* builder = codegen::getBuilder();
    DataType dt = codegen::getAttType(index);
    Value* tupleptr = codegen::getTupleptr();

    Value *indices[1];
    indices[0] = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), (uint64_t) index);
    ArrayRef<Value*> indicesRef(indices);

    Value *dataptr = builder->CreateInBoundsGEP(tupleptr, indicesRef);

    switch(dt){
        case DOUBLE:
            return builder->CreateLoad(
                    builder->CreateBitCast(dataptr, Type::getDoublePtrTy(getGlobalContext()))
            );
        case LONG:
        case STRING:
        case DATE:
            return builder->CreateLoad(dataptr);
    }
}

DataType Expression::getDataType() {
    ExprType t = this->getType();
    if(t == STRINGVALUEEXPRESSION)
        return STRING;
    if(t == DOUBLEVALUEEXPRESSION)
        return DOUBLE;
    if(t == LONGVALUEEXPRESSION)
        return LONG;
    if(t == DATEVALUEEXPRESSION)
        return DATE;
    if(t == ADDITIONEXPRESSION || t == SUBTRACTIONEXPRESSION || t == MULTIPLICATIONEXPRESSION || t == DIVISIONEXPRESSION){
        BinaryExpression* bexp = (BinaryExpression*)this;
        DataType t1 = bexp->getLeftExpression()->getDataType();
        DataType t2 = bexp->getRightExpression()->getDataType();
        if(t1 == DOUBLE || t2 == DOUBLE)
            return DOUBLE;
        return LONG;
    }
    if(t == COLEXPRESSION)
        return getDataType();
    //Boolean
    return LONG;
}
