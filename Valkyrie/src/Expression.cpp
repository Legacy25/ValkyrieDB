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

            // Use StringRef's compare()
            // (or)
            // iterate through the characters in the String for comparision
            //------------------------------------------------------------------------
            /*static uint32_t nameCounter = 0;

            FunctionType *loopFunctionType = FunctionType::get(int32Type, false);
            Function *loopFunction = Function::Create(loopFunctionType, Function::ExternalLinkage, "StringCheckStart", module);

            BasicBlock *entry = BasicBlock::Create(context, "entry", loopFunction);
            builder->SetInsertPoint(entry);


            Value *loopIndex = builder->CreateAlloca(int32Type, ConstantInt::get(int32Type, 1), "loopVar" + to_string(nameCounter++));
            builder->CreateStore(ConstantInt::get(int32Type, 0), loopIndex);

            BasicBlock *startLoopBody = BasicBlock::Create(context, "startLoopBody" + to_string(nameCounter++), loopFunction);
            builder->CreateBr(startLoopBody);
            builder->SetInsertPoint(startLoopBody);

            Value *i = builder->CreateLoad(loopIndex);

            Value *indices[1];
            indices[0] = i;
            ArrayRef<Value *> indicesRef(indices);*/

            //TODO
            //Value *charString1 = builder->CreateLoad(   /* Load each character of the String 1 */   );
            //Value *charString2 = builder->CreateLoad(   /* Load each character of the String 2 */   );

            //Value *increment = builder->CreateAdd(i, ConstantInt::get(int32Type, 1));
            //builder->CreateStore(increment, loopIndex);

            // Comapare both characters and use this as an exit condition of the loop
            // Or Run the loop until the length of small String
            //TODO
            //Value *cmp = builder->CreateICmpSLT(increment, /* min of lenString1 and lenString2 */);

            /*BasicBlock *endLoopBody = BasicBlock::Create(context, "endLoopBody"+to_string(nameCounter++), loopFunction);
            builder->CreateCondBr(cmp, startLoopBody, endLoopBody);
            builder->SetInsertPoint(endLoopBody);*/

            //------------------------------------------------------------------------


            /* BackUp - Code to use an if condition within the loop

            // if both characters not equal return false
            // false block which returns FALSE
            BasicBlock *startFalseBlock = BasicBlock::Create(context, "If"+to_string(nameCtr++), loopFunction);
            builder->CreateBr(startFalseBlock);
            builder->SetInsertPoint(startFalseBlock);

            //Compare characters

            BasicBlock *endFalseBlock = BasicBlock::Create(context, "endLoopBody"+to_string(nameCounter++), loopFunction);
            builder->CreateCondBr(cmp, startFalseBlock, endFalseBlock);
            builder->SetInsertPoint(endLoopBody);
            */

        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
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
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
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
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
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
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
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
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
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
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
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

StringValueExpression::StringValueExpression(std::string* data){
    this->data = data;
}

Value* StringValueExpression::getValue() {
    // TODO
    return NULL;
}

Value* DateValueExpression::getValue() {
    // TODO
    return NULL;
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
